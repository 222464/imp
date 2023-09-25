#pragma once

#include <stdbool.h>

typedef unsigned long u64;
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;
typedef long s64;
typedef int s32;
typedef short s16;
typedef double f64;
typedef float f32;
typedef int b32;

#define IMP_MAX(a,b) (((a) > (b)) ? (a) : (b))
#define IMP_ASSERT(c) do { if (!(c)) { (*(int*)0=0); }} while (0);
#define IMP_PCAST(type, p) (*((type*)&p))

#define HASH_INITIAL 2166136261

void imp_hash(u32* hash, const void* data, s32 size);
