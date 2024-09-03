#include "router.hh"

#include <iostream>

using namespace std;

// Given an incoming Internet datagram, the router decides
// (1) which interface to send it out on, and
// (2) what next hop address to send it to.

//! \param[in] route_prefix The "up-to-32-bit" IPv4 address prefix to match the datagram's destination address against
//! \param[in] prefix_length For this route to be applicable, how many high-order (most-significant) bits of the route_prefix will need to match the corresponding bits of the datagram's destination address?
//! \param[in] next_hop The IP address of the next hop. Will be empty if the network is directly attached to the router (in which case, the next hop address should be the datagram's final destination).
//! \param[in] interface_num The index of the interface to send the datagram out on.
void Router::add_route(const uint32_t route_prefix,
                       const uint8_t prefix_length,
                       const optional<Address> next_hop,
                       const size_t interface_num) {
    cerr << "DEBUG: adding route " << Address::from_ipv4_numeric(route_prefix).ip() << "/" << int(prefix_length)
         << " => " << (next_hop.has_value() ? next_hop->ip() : "(direct)") << " on interface " << interface_num << "\n";

    _rule.push_back(routeRule{route_prefix, prefix_length, next_hop, interface_num});
}

//! \param[in] dgram The datagram to be routed
void Router::route_one_datagram(InternetDatagram &dgram) {
    if (dgram.header().ttl > 1) {
        uint32_t dest = dgram.header().dst;
        int match = -1;
        int matchlen = -1;
        for (size_t i = 0; i < _rule.size(); i++) {
            uint8_t prefixlen = _rule[i].prefix_length;
            uint32_t mask = (prefixlen == 0 ? 0 : (0xFFFFFFFF << (32 - prefixlen)));
            if ((mask & dest) == _rule[i].route_prefix && matchlen < prefixlen) {
                match = i;
                matchlen = prefixlen;
            }
        }
        if (match != -1) {
            dgram.header().ttl--;
            std::optional<Address> next_hop = _rule[match].next_hop;
            size_t interface_num = _rule[match].interface_num;
            _interfaces[interface_num].send_datagram(
                dgram, (next_hop.has_value() ? next_hop.value() : Address::from_ipv4_numeric(dest)));
        }
    }
    return;
}

void Router::route() {
    // Go through all the interfaces, and route every incoming datagram to its proper outgoing interface.
    for (auto &interface : _interfaces) {
        auto &queue = interface.datagrams_out();
        while (not queue.empty()) {
            route_one_datagram(queue.front());
            queue.pop();
        }
    }
}
