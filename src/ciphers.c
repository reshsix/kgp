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

#include <kgp/types.h>
#include <kgp/structs.h>

#define ROTL(a, b) (((a) << (b)) | ((a) >> ((sizeof(b) * 8)  - (b))))
#define ROTR(a, b) (((a) >> (b)) | ((a) << ((sizeof(b) * 8)  - (b))))

static u64
lappland_round(u64 data, u64 key)
{
    u64 ret = 0;

    for (u8 i = 0; i < sizeof(u64); i++)
    {
        u8 byte = data & 0xFF;

        /* S-box for confusion */
        static u8 s_box[256] =
                          "You're tough, but it's never been about you\n"
                          "You're free, but cement your feet, a statue\n"
                          "Your rules, but you'd rather make up something\n"
                          "You're dead, you were never good for nothing\n"
                          "Double negatives leading me in circles\n"
                          "Twist infinity.\n"
                          "You drive me insane!\n";
        byte ^= s_box[byte];

        /* 8-bit Rotation XOR for diffusion */
        byte = ROTL(byte, (ret + i) % 7);

        ret |= byte;
        ret <<= 8;

        data >>= 8;
    }

    /* 64-bit Rotation XOR for diffusion */
    ret ^= ROTR(ret, ret % 63);

    /* Adding round key */
    ret ^= key;

    return ret;
}

extern void
kgp_cipher_lappland(u64 *data, u64 key[static 2], bool invert)
{
    struct kgp_struct_feistel lappland = {.function = lappland_round,
                                          .rounds = 16};

    /* Expanding and rotating XOR 128bits key into 16x64bits */
    u64 subkeys[16] = {0};
    for (u8 i = 0; i < 16; i++)
    {
        u64 x = key[i % 2];
        x ^= ROTL(x, i);
        subkeys[i] = x;
    }

    kgp_struct_feistel(&lappland, subkeys, data, invert);
}

static u64
misaka_round(u64 data, u64 key)
{
    u64 ret = data;

    /* Capacitor charge (10000V RC=100 + 10000V RC=10) mod 256*/
    static u8 s_box[256] = {0x20, 0xa0, 0x90, 0xfb, 0xee, 0x78, 0xa8, 0x8f,
                            0x40, 0xd2, 0x5b, 0xf4, 0xbc, 0xd1, 0x57, 0x73,
                            0x51, 0x21, 0x15, 0x67, 0x59, 0x2e, 0x37, 0xc8,
                            0x40, 0x09, 0x97, 0x69, 0x0e, 0x22, 0x52, 0x5e,
                            0x19, 0x6c, 0x5a, 0x01, 0x9c, 0x86, 0x43, 0x7a,
                            0x04, 0xe7, 0x63, 0xf3, 0x54, 0x8e, 0xfc, 0x50,
                            0xa1, 0x75, 0xcb, 0x28, 0xa7, 0x05, 0xb8, 0xfc,
                            0xeb, 0x95, 0x18, 0xbf, 0x1d, 0x36, 0xa0, 0xaf,
                            0xa7, 0xec, 0x3c, 0xf1, 0x44, 0x9c, 0xe5, 0xeb,
                            0xc4, 0x46, 0x84, 0x5a, 0x0e, 0xf6, 0x3a, 0xa9,
                            0x9b, 0xf8, 0x50, 0x19, 0x07, 0x8d, 0x84, 0xff,
                            0x55, 0x57, 0xcb, 0x2a, 0x9f, 0x61, 0x5f, 0x55,
                            0x4d, 0x93, 0x3e, 0x39, 0x01, 0x0e, 0x09, 0xd7,
                            0xa5, 0xf3, 0xd6, 0x79, 0x12, 0x65, 0x03, 0x79,
                            0x9b, 0x24, 0xf5, 0x2c, 0x64, 0x7a, 0x29, 0xf3,
                            0xbb, 0x9d, 0x87, 0x33, 0x2b, 0x99, 0xab, 0x83,
                            0x9f, 0xe1, 0x75, 0xd8, 0x97, 0x5c, 0x26, 0xa1,
                            0xe9, 0x25, 0xad, 0xc7, 0x42, 0xa8, 0x04, 0xc5,
                            0xad, 0x43, 0xd8, 0xd1, 0x91, 0x37, 0x2e, 0x8f,
                            0x5a, 0xb1, 0x8b, 0xc6, 0x08, 0x98, 0x45, 0x93,
                            0xaf, 0x51, 0x72, 0x07, 0x4c, 0x17, 0xe3, 0xfd,
                            0x71, 0xfa, 0xc3, 0x8d, 0xc2, 0x70, 0xaf, 0x52,
                            0x81, 0x6c, 0x8f, 0x88, 0xbe, 0x3c, 0x19, 0x27,
                            0x4c, 0x6d, 0x2b, 0x86, 0xc2, 0x78, 0x8b, 0x88,
                            0x5a, 0x5b, 0xf3, 0x0e, 0x55, 0x5d, 0x6e, 0xde,
                            0x54, 0x4f, 0xd8, 0x69, 0x35, 0x12, 0xad, 0x2d,
                            0x42, 0xf7, 0x3e, 0xf5, 0x6c, 0x18, 0x0a, 0xed,
                            0xc6, 0x35, 0x42, 0xc9, 0x8e, 0xc2, 0x2b, 0x7d,
                            0x5f, 0x76, 0xf9, 0xc2, 0x0a, 0x63, 0xb7, 0x61,
                            0x88, 0x9c, 0x1f, 0xe8, 0xd9, 0x7f, 0xd7, 0x32,
                            0x9a, 0xe6, 0xde, 0x8d, 0xbe, 0x7b, 0xa1, 0xd3,
                            0xfa, 0x15, 0x83, 0xeb, 0xa1, 0x61, 0xf4, 0xa6};
    u8 *bytes = (u8*)&ret;
    for (u8 i = 0; i < sizeof(u64); i++)
        bytes[i] = s_box[bytes[i]];

    /* ARX for diffusion */
    ret += ROTL(ret, 7) ^ ROTR(ret, 3);

    /* Adding round key */
    ret ^= key;

    return ret;
}

extern void
kgp_cipher_misaka(u64 *data, u64 key[static 2], bool invert)
{
    struct kgp_struct_feistel misaka = {.function = misaka_round,
                                        .rounds = 16};

    /* Expanding 128bits key into 16x64bits */
    u64 subkeys[16] = {0};
    for (u8 i = 0; i < 16; i++)
        subkeys[i] = ROTL(key[(i % 2)], i) ^ ROTR(key[!(i % 2)], i);

    /* Key whitening */
    data[0] ^= key[0];
    data[1] ^= key[1];

    kgp_struct_feistel(&misaka, subkeys, data, invert);

    /* Key whitening */
    data[0] ^= key[0];
    data[1] ^= key[1];
}
