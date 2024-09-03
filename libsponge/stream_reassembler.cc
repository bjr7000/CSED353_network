#include "stream_reassembler.hh"

using namespace std;

StreamReassembler::StreamReassembler(const size_t capacity)
    : _output(capacity)
    , _capacity(capacity)
    , eofIndex(numeric_limits<size_t>::max())
    , passedIndex(0)
    , dataStr()
    , validStr() {}

//! \details This function accepts a substring (aka a segment) of bytes,
//! possibly out-of-order, from the logical stream, and assembles any newly
//! contiguous substrings and writes them into the output stream in order.
void StreamReassembler::push_substring(const string &data, const size_t index, const bool eof) {
    if (eof) {
        eofIndex = data.size() + index;
    }
    if (data.size() == 0) {
        if (eofIndex <= passedIndex) {
            _output.end_input();
        }
        return;
    }
    size_t dataIndex = passedIndex > index ? 0 : index - passedIndex;

    string _data;
    if (index + data.size() <= passedIndex) {
        return;
    } else if (index < passedIndex) {
        _data = data.substr(passedIndex - index);
    } else {
        _data = data;
    }
    if (_capacity < index - passedIndex + _data.size()) {
        _data = _data.substr(0, _capacity - index + passedIndex);
    }

    if (_data.size() + dataIndex > dataStr.size()) {
        dataStr.resize(_data.size() + dataIndex);
        validStr.resize(_data.size() + dataIndex);
    }
    dataStr.replace(dataIndex, _data.size(), _data);
    string temp;
    temp.resize(_data.size(), '1');
    validStr.replace(dataIndex, _data.size(), temp);

    if (!validStr.empty() && validStr[0] == '1' && _output.remaining_capacity() > 0) {
        size_t len = 0;
        size_t remainingCap = _output.remaining_capacity();
        size_t dataSize = dataStr.size();
        while (validStr[len] == '1' && len < remainingCap && len < dataSize) {
            len++;
        }
        if (len > 0) {
            len = _output.write(dataStr.substr(0, len));
            passedIndex += len;
            dataStr = dataStr.substr(len);
            validStr = validStr.substr(len);
        }
    }

    if (eofIndex <= passedIndex) {
        _output.end_input();
    }
}

size_t StreamReassembler::unassembled_bytes() const {
    size_t cnt = 0;
    for (char c : validStr) {
        if (c == '1') {
            cnt++;
        }
    }
    return cnt;
}

bool StreamReassembler::empty() const { return dataStr.empty(); }
