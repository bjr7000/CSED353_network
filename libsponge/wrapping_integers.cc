#include "wrapping_integers.hh"

using namespace std;

WrappingInt32 wrap(uint64_t n, WrappingInt32 isn) { return isn + static_cast<uint32_t>(n); }

uint64_t unwrap(WrappingInt32 n, WrappingInt32 isn, uint64_t checkpoint) {
    const uint64_t screen = 0x0000000100000000;
    const WrappingInt32 index(n - isn);
    const uint64_t ret = (checkpoint & 0xFFFFFFFF00000000) + index.raw_value();
    if (abs(int64_t(ret - checkpoint)) > abs(int64_t(ret + screen - checkpoint))) {
        return ret + screen;
    }
    if (ret > screen && abs(int64_t(ret - checkpoint)) > abs(int64_t(ret - screen - checkpoint))) {
        return ret - screen;
    }
    return ret;
}
