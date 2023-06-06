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

#include <errno.h>
#include <stdio.h>
#include <locale.h>
#include <stdlib.h>
#include <string.h>

#include <kgp/types.h>
#include <kgp/modes.h>
#include <kgp/structs.h>
#include <kgp/ciphers.h>

static bool
error(const char *s)
{
    fprintf(stderr, "kgp: %s\n", s);
    return false;
}

static bool
usage(void)
{
    fprintf(stderr, "usage: kgp encrypt/decrypt CIPHER INPUT OUTPUT\n");
    fprintf(stderr, "Encrypts/decrypts files using experimental ciphers\n");
    fprintf(stderr, "\nKey is read as hex from KGPKEY envvar\n");
    fprintf(stderr, "Ciphers:\n");
    fprintf(stderr, "\tLAPPLAND\t128bits key and block, 16 rounds Feistel\n");
    fprintf(stderr, "\t\t\tUses S-box made of Signore dei Lupi lyrics\n");
    fprintf(stderr, "\t\t\tand rotation XOR\n");
    fprintf(stderr, "\t\t\tInitializes CBC with TEXAS DEADBEEF\n");
    return false;
}

extern int
main(int argc, char *argv[])
{
    bool ret = true;

    setlocale(LC_ALL, "");
    if (argc != 5)
        ret = usage();

    bool invert = false;
    if (ret)
    {
        if (strcmp(argv[1], "decrypt") == 0)
            invert = true;
        else if (strcmp(argv[1], "encrypt") != 0)
            ret = usage();
    }

    void (*run)(u64 *, u64 [static 2], bool) = NULL;
    if (ret)
    {
        if (strcmp(argv[2], "LAPPLAND") == 0)
            run = kgp_cipher_lappland;
        else
            ret = usage();
    }

    FILE *a = NULL, *b = NULL;
    if (ret)
    {
        a = fopen(argv[3], "rb");
        if (!a)
            ret = error(strerror(errno));
    }

    if (ret)
    {
        b = fopen(argv[4], "wb");
        if (!b)
            ret = error(strerror(errno));
    }

    char *str = NULL;
    if (ret)
    {
        str = getenv("KGPKEY");
        if (!str || strlen(str) != 32)
            ret = error(strerror(EINVAL));
    }

    u64 key[2] = {0, 0};
    if (ret)
    {
        for (u8 i = 0; i < 32; i ++)
        {
            u64 *out = (i < 16) ? &(key[0]) : &(key[1]);
            char c = str[i];
            if (c >= '0' && c <= '9')
                *out += (c - '0') << (i * 4);
            else if (c >= 'a' && c <= 'f')
                *out += (c - 'a' + 10) << (i * 4);
            else if (c >= 'A' && c <= 'F')
                *out += (c - 'A' + 10) << (i * 4);
            else
            {
                ret = error(strerror(EINVAL));
                break;
            }
        }
    }

    if (ret)
    {
        u64 iv[2] = {0x7E8A5, 0xDEADBEEF};
        ret = kgp_mode_cbc(a, b, run, iv, key, invert);
    }

    if (a)
        fclose(a);
    if (b)
        fclose(b);

    return (ret) ? EXIT_SUCCESS : EXIT_FAILURE;
}
