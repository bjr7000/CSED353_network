#include "tcp_sender.hh"

#include "tcp_config.hh"

#include <random>
using namespace std;

//! \param[in] capacity the capacity of the outgoing byte stream
//! \param[in] retx_timeout the initial amount of time to wait before retransmitting the oldest outstanding segment
//! \param[in] fixed_isn the Initial Sequence Number to use, if set (otherwise uses a random ISN)
TCPSender::TCPSender(const size_t capacity, const uint16_t retx_timeout, const std::optional<WrappingInt32> fixed_isn)
    : _isn(fixed_isn.value_or(WrappingInt32{random_device()()}))
    , _initial_retransmission_timeout{retx_timeout}
    , _stream(capacity)
    , timeLimit{retx_timeout} {}

uint64_t TCPSender::bytes_in_flight() const { return _bytes_in_flight; }

void TCPSender::fill_window() {
    TCPSegment seg;

    if (!synFlag) {
        seg.header().syn = true;
        synFlag = true;
        send_seg(seg);
        _receiver_space = 0;
    }

    else if (ack_arrived) {
        while (_receiver_space > 0 && !finFlag) {
            size_t payloadSize = min({_stream.buffer_size(), _receiver_space, TCPConfig::MAX_PAYLOAD_SIZE});
            if (payloadSize == 0 && !_stream.eof()) {
                break;
            }
            seg.payload() = Buffer(_stream.read(payloadSize));
            if (_receiver_space > payloadSize && _stream.eof()) {
                seg.header().fin = true;
                finFlag = true;
            }
            send_seg(seg);
        }
    }
}

void TCPSender::send_seg(TCPSegment seg) {
    size_t segLen = seg.length_in_sequence_space();
    _receiver_space -= segLen;
    seg.header().seqno = next_seqno();
    _next_seqno += segLen;
    _bytes_in_flight += segLen;
    _segments_out.push(seg);
    _segments_outstanding.push(seg);
    if (!timer_activated) {
        timer_activated = true;
        timer = 0;
    }
}

//! \param ackno The remote receiver's ackno (acknowledgment number)
//! \param window_size The remote receiver's advertised window size
void TCPSender::ack_received(const WrappingInt32 ackno, const uint16_t window_size) {
    const uint64_t ack_seqno = unwrap(ackno, _isn, _next_seqno);
    if (ack_seqno > _next_seqno) {
        return;
    }  // ACK from future

    if (window_size == 0) {
        zero_size_window = true;
        _receiver_space = 1;
    } else {
        zero_size_window = false;
        _receiver_space = window_size;
    }

    while (!_segments_outstanding.empty()) {
        TCPSegment seg = _segments_outstanding.front();
        const uint64_t segNo = unwrap(seg.header().seqno, _isn, _next_seqno) + seg.length_in_sequence_space();
        if (segNo <= ack_seqno) {
            _segments_outstanding.pop();
            _bytes_in_flight -= seg.length_in_sequence_space();
            _consecutive_retransmissions = 0;
            timer = 0;
            timeLimit = _initial_retransmission_timeout;
            ack_arrived = true;
        } else {
            ack_arrived = false;
            break;
        }
    }
    if (_segments_outstanding.empty()) {
        timer_activated = false;
    }
}

//! \param[in] ms_since_last_tick the number of milliseconds since the last call to this method
void TCPSender::tick(const size_t ms_since_last_tick) {
    if (!timer_activated) {
        return;
    }
    timer += ms_since_last_tick;
    if (timer >= timeLimit) {
        TCPSegment retrans = _segments_outstanding.front();
        while (retrans.length_in_sequence_space() == 0) {
            _segments_outstanding.pop();
            if (_segments_outstanding.empty()) {
                break;
            } else {
                retrans = _segments_outstanding.front();
            }
        }
        if (retrans.length_in_sequence_space() != 0) {
            _segments_out.push(retrans);
        } else {
            send_empty_segment();
        }
        if (!zero_size_window) {
            timeLimit *= 2;
            _consecutive_retransmissions++;
        }
        timer = 0;
    }
}

unsigned int TCPSender::consecutive_retransmissions() const { return _consecutive_retransmissions; }

void TCPSender::send_empty_segment() {
    TCPSegment seg;
    send_seg(seg);
}
