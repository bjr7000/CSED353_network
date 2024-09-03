#include "tcp_receiver.hh"

using namespace std;

void TCPReceiver::segment_received(const TCPSegment &seg) {
    const TCPHeader head = seg.header();
    if (head.syn) {
        synFlag = true;
        absSeq = 0;
        isn_ = head.seqno;
    }
    if (synFlag) {
        absSeq = unwrap(head.seqno, isn_, absSeq);
        if (absSeq != 0 || head.syn) {
            _reassembler.push_substring(seg.payload().copy(), absSeq + head.syn - 1, head.fin);
        }
    }
}

optional<WrappingInt32> TCPReceiver::ackno() const {
    if (synFlag) {
        return wrap(stream_out().bytes_written() + 1 + stream_out().input_ended(), isn_);
    } else {
        return nullopt;
    }
}

size_t TCPReceiver::window_size() const { return stream_out().remaining_capacity(); }
