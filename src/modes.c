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

#include <time.h>
#include <stddef.h>
#include <string.h>

#include <kgp/types.h>

extern void
kgp_mode_cbc128(bool (*in) (void *, u8 *, size_t), void *ctx_in,
                bool (*out)(void *, u8 *, size_t), void *ctx_out,
                void (*cipher)(b128 *, b128, bool),
                b128 key, bool invert)
{
    bool ok = true;

    b128 vec = {0};
    if (!invert)
    {
        vec.u64[0] = clock();
        vec.u64[1] = time(0);
        cipher(&vec, key, false);
        ok = out(ctx_out, vec.u8, sizeof(vec));
    }
    else
        ok = in(ctx_in, vec.u8, sizeof(vec));

    while (ok)
    {
        b128 buf = {0};

        ok = in(ctx_in, buf.u8, sizeof(buf));
        if (!ok)
            break;

        if (!invert)
        {
            buf.u64[0] ^= vec.u64[0];
            buf.u64[1] ^= vec.u64[1];

            cipher(&buf, key, false);

            vec.u64[0] = buf.u64[0];
            vec.u64[1] = buf.u64[1];
        }
        else
        {
            b128 buf2 = buf;

            cipher(&buf, key, true);

            buf.u64[0] ^= vec.u64[0];
            buf.u64[1] ^= vec.u64[1];

            vec.u64[0] = buf2.u64[0];
            vec.u64[1] = buf2.u64[1];
        }

        ok = out(ctx_out, buf.u8, sizeof(buf));
    }
}
