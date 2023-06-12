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

extern void
kgp_struct_feistel128(u64 (*f)(u64, u64), u8 rounds,
                      u64 subkeys[], b128 *data, bool invert)
{
    u64 *l = &(data->u64[0]);
    u64 *r = &(data->u64[1]);

    u64 tmp = 0;
    for (u8 i = 0; i < rounds; i++)
    {
        tmp = *l;
        *l = *r;

        if (!invert)
            *r = f(*r, subkeys[i]);
        else
            *r = f(*r, subkeys[rounds - i - 1]);

        *r ^= tmp;
    }

    tmp = *l;
    *l = *r;
    *r = tmp;
}

extern void
kgp_struct_feistel256(b128 (*f)(b128, b128), u8 rounds,
                      b128 subkeys[], b256 *data, bool invert)
{
    b128 *l = &(data->b128[0]);
    b128 *r = &(data->b128[1]);

    b128 tmp = {0};
    for (u8 i = 0; i < rounds; i++)
    {
        tmp = *l;
        *l = *r;

        if (!invert)
            *r = f(*r, subkeys[i]);
        else
            *r = f(*r, subkeys[rounds - i - 1]);

        r->u64[0] ^= tmp.u64[0];
        r->u64[1] ^= tmp.u64[1];
    }

    tmp = *l;
    *l = *r;
    *r = tmp;
}
