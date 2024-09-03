#include "tcp_connection.hh"

#include <iostream>

using namespace std;

size_t TCPConnection::remaining_outbound_capacity() const { return _sender.stream_in().remaining_capacity(); }

size_t TCPConnection::bytes_in_flight() const { return _sender.bytes_in_flight(); }

size_t TCPConnection::unassembled_bytes() const { return _receiver.unassembled_bytes(); }

size_t TCPConnection::time_since_last_segment_received() const { return _last_rcved_time; }

void TCPConnection::send_to_the_world() {
    if (_sender.segments_out().empty() && rstFlag) {
        _sender.send_empty_segment();
    }
    while (!_sender.segments_out().empty()) {
        TCPSegment &seg = _sender.segments_out().front();
        _sender.segments_out().pop();
        uint16_t maxwin = numeric_limits<uint16_t>::max();
        seg.header().win = _receiver.window_size() > maxwin ? maxwin : _receiver.window_size();
        if (_receiver.ackno().has_value()) {
            seg.header().ack = true;
            seg.header().ackno = _receiver.ackno().value();
        }
        seg.header().rst = rstFlag;
        if (seg.header().fin) {
            finSent = true;
        }
        if (!finAck || finRcv < 2) {
            _segments_out.push(seg);
        } else if (seg.length_in_sequence_space() > 0) {
            seg.header().ack = false;
            _segments_out.push(seg);
        }
    }
}

void TCPConnection::shutdown() {
    rstFlag = true;
    _sender.stream_in().set_error();
    _receiver.stream_out().set_error();
    _active = false;
    connected = false;
}

void TCPConnection::segment_received(const TCPSegment &seg) {
    _last_rcved_time = 0;
    const TCPHeader &hdr = seg.header();
    if (hdr.rst && !_sender.stream_in().input_ended()) {
        shutdown();
    } else if (connected || hdr.syn) {
        connected = true;
        if (hdr.ack) {
            _sender.ack_received(hdr.ackno, hdr.win);
            _sender.fill_window();
        }
        finAck = hdr.fin;
        finRcv += hdr.fin;
        if ((_receiver.ackno().has_value() && (seg.length_in_sequence_space() == 0) &&
             (hdr.seqno == _receiver.ackno().value() - 1)) ||
            (seg.length_in_sequence_space() > 0)) {
            _sender.fill_window();
            if (_sender.segments_out().empty()) {
                _sender.send_empty_segment();
            }
        }
        _linger_after_streams_finish &= (!seg.header().fin || _sender.stream_in().input_ended());
        _receiver.segment_received(seg);
        if (connected) {
            send_to_the_world();
        }
    }
}

bool TCPConnection::active() const { return _active; }

size_t TCPConnection::write(const string &data) {
    size_t written_size = data.size() == 0 ? 0 : _sender.stream_in().write(data);
    if (written_size != 0) {
        _sender.fill_window();
        send_to_the_world();
    }
    return written_size;
}

//! \param[in] ms_since_last_tick number of milliseconds since the last call to this method
void TCPConnection::tick(const size_t ms_since_last_tick) {
    _last_rcved_time += ms_since_last_tick;
    _sender.tick(ms_since_last_tick);
    if (_sender.consecutive_retransmissions() > _cfg.MAX_RETX_ATTEMPTS) {
        shutdown();
    } else if (_receiver.stream_out().input_ended() && _sender.stream_in().eof() &&
               (_sender.next_seqno_absolute() == _sender.stream_in().bytes_written() + 2) &&
               (_sender.bytes_in_flight() == 0) &&
               (!_linger_after_streams_finish || (_last_rcved_time >= 10 * _cfg.rt_timeout))) {
        _active = false;
    }
    if (!finSent) {
        _sender.fill_window();
    }
    if (!_sender.segments_out().empty() || rstFlag) {
        send_to_the_world();
    }
}

void TCPConnection::end_input_stream() {
    _sender.stream_in().end_input();
    _sender.fill_window();
    finSent = false;
    send_to_the_world();
}

void TCPConnection::connect() {
    _sender.fill_window();
    connected = true;
    send_to_the_world();
}

TCPConnection::~TCPConnection() {
    try {
        if (active()) {
            cerr << "Warning: Unclean shutdown of TCPConnection\n";

            // Your code here: need to send a RST segment to the peer
            shutdown();
            send_to_the_world();
        }
    } catch (const exception &e) {
        std::cerr << "Exception destructing TCP FSM: " << e.what() << std::endl;
    }
}
