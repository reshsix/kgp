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
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <kgp/types.h>

static bool
error(const char *s)
{
    fprintf(stderr, "kgp: %s\n", s);
    return false;
}

extern bool
kgp_mode_cbc(FILE *src, FILE *dest, void (*cipher)(u64 *, u64[static 2], bool),
             u64 key[static 2], bool invert)
{
    bool ret = true;

    u64 vec[2] = {0};
    if (!invert)
    {
        vec[0] = clock();
        vec[1] = time(0);
        cipher(vec, key, false);

        if (fwrite(vec, sizeof(vec), 1, dest) < 1)
            ret = error(strerror(errno));
    }
    else
    {
        if (fread(vec, 1, sizeof(vec), src) == 0)
            if (ferror(src))
                ret = error(strerror(errno));
    }

    while (ret)
    {
        u64 buf[2] = {0};

        size_t read = fread(buf, 1, sizeof(buf), src);
        if (read == 0)
        {
            if (ferror(src))
                ret = error(strerror(errno));
            break;
        }

        if (!invert)
        {
            if (read != sizeof(buf))
            {
                size_t pad = sizeof(buf) - read;
                memset(&(((u8*)buf)[read]), pad, pad);
            }

            buf[0] ^= vec[0];
            buf[1] ^= vec[1];

            cipher(buf, key, false);

            vec[0] = buf[0];
            vec[1] = buf[1];

            if (fwrite(buf, sizeof(buf), 1, dest) < 1)
                ret = error(strerror(errno));
        }
        else
        {
            u64 buf2[2] = {buf[0], buf[1]};

            cipher(buf, key, true);

            buf[0] ^= vec[0];
            buf[1] ^= vec[1];

            vec[0] = buf2[0];
            vec[1] = buf2[1];

            u8 pad = 0;
            int c = getc(src);
            if (c == EOF)
            {
                u8 *buf8 = (u8*)buf;
                pad = buf8[15];
                pad = (pad < 16) ? pad : 0;
                for (u8 i = 1; i < pad; i++)
                {
                    if (buf8[15 - i] != pad)
                        pad = 0;
                }
            }
            else
                ungetc(c, src);

            if (fwrite(buf, sizeof(buf) - pad, 1, dest) < 1)
                ret = error(strerror(errno));
        }
    }

    return ret;
}
