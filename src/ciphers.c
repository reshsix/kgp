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
