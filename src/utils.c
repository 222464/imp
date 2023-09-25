#include "utils.h"

void imp_hash(u32* hash, const void* data, s32 size) {
    const u8* p = (u8*)data;

    while (size--) {
        *hash ^= *p++ & 0x8F;
        *hash *= 16777619;
    }
}

