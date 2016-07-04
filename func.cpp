#include "func.h"
#include "math.h"

quint32 rotateLeft(quint32 value, int shift)
{
    if ((shift &= sizeof(value)*8 - 1) == 0)
        return value;
    return (value << shift) | (value >> (sizeof(value)*8 - shift));
}

quint32 rotateRight(quint32 value, int shift)
{
    if ((shift &= sizeof(value)*8 - 1) == 0)
        return value;
    return (value >> shift) | (value << (sizeof(value)*8 - shift));
}

quint32 reverse(quint32 value, int bits)
{
    value = (((value & 0xaaaaaaaa) >> 1) | ((value & 0x55555555) << 1));
    value = (((value & 0xcccccccc) >> 2) | ((value & 0x33333333) << 2));
    value = (((value & 0xf0f0f0f0) >> 4) | ((value & 0x0f0f0f0f) << 4));
    value = (((value & 0xff00ff00) >> 8) | ((value & 0x00ff00ff) << 8));
    value = ((value >> 16) | (value << 16));

    value >>= (sizeof(value) - bits);
    value &= (2 << bits) - 1;
    return value;
}
