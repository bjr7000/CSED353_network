#include "network_interface.hh"

#include "arp_message.hh"
#include "ethernet_frame.hh"

#include <iostream>

using namespace std;

//! \param[in] ethernet_address Ethernet (what ARP calls "hardware") address of the interface
//! \param[in] ip_address IP (what ARP calls "protocol") address of the interface
NetworkInterface::NetworkInterface(const EthernetAddress &ethernet_address, const Address &ip_address)
    : _ethernet_address(ethernet_address), _ip_address(ip_address) {
    cerr << "DEBUG: Network interface has Ethernet address " << to_string(_ethernet_address) << " and IP address "
         << ip_address.ip() << "\n";
}

//! \param[in] dgram the IPv4 datagram to be sent
//! \param[in] next_hop the IP address of the interface to send it to (typically a router or default gateway, but may also be another host if directly connected to the same network as the destination)
//! (Note: the Address type can be converted to a uint32_t (raw 32-bit IP address) with the Address::ipv4_numeric() method.)
void NetworkInterface::send_datagram(const InternetDatagram &dgram, const Address &next_hop) {
    const uint32_t next_hop_ip = next_hop.ipv4_numeric();
    auto cachefind = _cache.find(next_hop_ip);
    EthernetFrame frame;
    frame.header().src = _ethernet_address;
    if (cachefind != _cache.end()) {
        frame.header().type = EthernetHeader::TYPE_IPv4;
        frame.header().dst = cachefind->second.first;
        frame.payload() = dgram.serialize();
        _frames_out.push(frame);
    } else {
        auto whensent = _timeflow.find(next_hop_ip);
        if (whensent->second + 5000 < time || whensent == _timeflow.end()) {
            ARPMessage arpmsg;
            arpmsg.sender_ip_address = _ip_address.ipv4_numeric();
            arpmsg.target_ip_address = next_hop_ip;
            arpmsg.sender_ethernet_address = _ethernet_address;
            arpmsg.opcode = ARPMessage::OPCODE_REQUEST;
            frame.header().type = EthernetHeader::TYPE_ARP;
            frame.header().dst = ETHERNET_BROADCAST;
            frame.payload() = BufferList(arpmsg.serialize());
            _frames_out.push(frame);
            if (whensent != _timeflow.end()) {
                whensent->second = time;
            } else {
                _timeflow.emplace(next_hop_ip, time);
            }
            _waitDgram.push_back(make_pair(next_hop_ip, dgram));
        }
    }
}

//! \param[in] frame the incoming Ethernet frame
optional<InternetDatagram> NetworkInterface::recv_frame(const EthernetFrame &frame) {
    optional<InternetDatagram> ret = nullopt;
    const auto header = frame.header();
    if (header.dst == _ethernet_address || header.dst == ETHERNET_BROADCAST) {
        if (header.type == EthernetHeader::TYPE_IPv4) {
            InternetDatagram dg;
            if (dg.parse(Buffer(frame.payload())) == ParseResult::NoError) {
                ret = dg;
            }
        } else if (header.type == EthernetHeader::TYPE_ARP) {
            ARPMessage arp;
            if (arp.parse(Buffer(frame.payload())) == ParseResult::NoError) {
                const auto arpip = arp.sender_ip_address;
                const auto arpeth = arp.sender_ethernet_address;
                auto cachefind = _cache.find(arpip);
                auto req = _timeflow.find(arpip);
                auto wd = _waitDgram.begin();
                if (cachefind != _cache.end()) {
                    cachefind->second.first = arpeth;
                    cachefind->second.second = time;
                } else {
                    _cache.emplace(arpip, make_pair(arpeth, time));
                }
                if (req != _timeflow.end()) {
                    _timeflow.erase(req);
                }
                while (wd != _waitDgram.end()) {
                    if (wd->first == arpip) {
                        EthernetFrame fr;
                        fr.header().type = EthernetHeader::TYPE_IPv4;
                        fr.header().src = _ethernet_address;
                        fr.header().dst = _cache[arpip].first;
                        fr.payload() = BufferList(wd->second.serialize());
                        _frames_out.push(fr);
                        wd = _waitDgram.erase(wd);
                    } else {
                        wd++;
                    }
                }
                if (arp.target_ip_address == _ip_address.ipv4_numeric() && arp.opcode == ARPMessage::OPCODE_REQUEST) {
                    ARPMessage msg;
                    EthernetFrame fr;
                    msg.sender_ip_address = _ip_address.ipv4_numeric();
                    msg.sender_ethernet_address = _ethernet_address;
                    msg.target_ip_address = arpip;
                    msg.target_ethernet_address = arpeth;
                    msg.opcode = ARPMessage::OPCODE_REPLY;
                    fr.header().type = EthernetHeader::TYPE_ARP;
                    fr.header().src = _ethernet_address;
                    fr.header().dst = arpeth;
                    fr.payload() = BufferList(msg.serialize());
                    _frames_out.push(fr);
                }
            }
        }
    }

    return ret;
}

//! \param[in] ms_since_last_tick the number of milliseconds since the last call to this method
void NetworkInterface::tick(const size_t ms_since_last_tick) {
    time += ms_since_last_tick;
    auto cache = _cache.begin();
    while (cache != _cache.end()) {
        if (time > cache->second.second + 30000) {
            cache = _cache.erase(cache);
        } else {
            cache++;
        }
    }
}
