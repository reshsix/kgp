/*
This file is part of kgp.

kgp is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License as published
by the Free Software Foundation, version 3.

kgp is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with kgp. If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef KGP_TYPES_H
#define KGP_TYPES_H

#include <stdint.h>
#include <stdbool.h>

typedef int8_t   s8;
typedef int16_t  s16;
typedef int32_t  s32;
typedef int64_t  s64;
typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef union __attribute__((packed, aligned(8)))
{
    u8  u8  [16];
    u16 u16 [8];
    u32 u32 [4];
    u64 u64 [2];
} b128;

typedef union __attribute__((packed, aligned(8)))
{
    u8   u8   [32];
    u16  u16  [16];
    u32  u32  [8];
    u64  u64  [4];
    b128 b128 [2];
} b256;

typedef union __attribute__((packed, aligned(8)))
{
    u8   u8   [64];
    u16  u16  [32];
    u32  u32  [16];
    u64  u64  [8];
    b128 b128 [4];
    b256 b256 [2];
} b512;

#endif
