#include "byte_stream.hh"

using namespace std;

ByteStream::ByteStream(const size_t capacity)
    : capacitySize(capacity), buffer(0), writtenBytes(0), readBytes(0), endInput(false) {}

size_t ByteStream::write(const string &data) {
    size_t counter = 0;
    for (const char c : data) {
        if (capacitySize > buffer) {
            Stream += c;
            buffer++;
            writtenBytes++;
            counter++;
        } else {
            break;
        }
    }
    return counter;
}

//! \param[in] len bytes will be copied from the output side of the buffer
string ByteStream::peek_output(const size_t len) const {
    size_t temp = len > buffer ? buffer : len;
    return Stream.substr(0, temp);
}

//! \param[in] len bytes will be removed from the output side of the buffer
void ByteStream::pop_output(const size_t len) {
    size_t temp = len > buffer ? buffer : len;
    readBytes += temp;
    buffer -= temp;
    Stream.erase(0, temp);
}

//! Read (i.e., copy and then pop) the next "len" bytes of the stream
//! \param[in] len bytes will be popped and returned
//! \returns a string
std::string ByteStream::read(const size_t len) {
    const string readStr = peek_output(len);
    pop_output(len);
    return {readStr};
}

void ByteStream::end_input() { endInput = true; }

bool ByteStream::input_ended() const { return endInput; }

size_t ByteStream::buffer_size() const { return buffer; }

bool ByteStream::buffer_empty() const { return Stream.size() == 0; }

bool ByteStream::eof() const { return input_ended() && buffer_empty(); }

size_t ByteStream::bytes_written() const { return writtenBytes; }

size_t ByteStream::bytes_read() const { return readBytes; }

size_t ByteStream::remaining_capacity() const { return capacitySize - buffer; }
