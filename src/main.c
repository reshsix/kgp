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

static void
usage_cipher(const char *name, const char *confusion, const char *diffusion,
             const char *structure, const char *size, const char *bias,
             const char *avalanche)
{
    fprintf(stderr, "%s\n", name);
    fprintf(stderr, "\tConfusion: %s\n", confusion);
    fprintf(stderr, "\tDiffusion: %s\n", diffusion);
    fprintf(stderr, "\tStructure: %s\n", structure);
    fprintf(stderr, "\tSize: %s\n", size);
    fprintf(stderr, "\tBias: %s\n", bias);
    fprintf(stderr, "\tAvalanche: %s\n", avalanche);
    fprintf(stderr, "\t\n");
}

static bool
usage(void)
{
    fprintf(stderr, "usage: kgp encrypt/decrypt CIPHER INPUT OUTPUT\n");
    fprintf(stderr, "Encrypts/decrypts files using experimental ciphers\n");
    fprintf(stderr, "Key is read as hex from KGPKEY envvar\n");
    fprintf(stderr, "\n[Ciphers]\n\n");
    usage_cipher("LAPPLAND", "S-box made of Signore dei Lupi lyrics",
                             "RX operations", "16 rounds Feistel",
                             "Key 128, Block 128",
                             "Linear 0.5, Differential 0.148",
                             "Minimum 0.37, Average 0.49");
    usage_cipher("MISAKA",   "S-box made of the capacitor charge formula",
                             "ARX operations", "16 rounds Feistel",
                             "Key 128, Block 128",
                             "Linear 0.125, Differential 0.039",
                             "Minimum 0.44, Average 0.51");
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

    void (*run)(b128 *, b128, bool) = NULL;
    if (ret)
    {
        if (strcmp(argv[2], "LAPPLAND") == 0)
            run = kgp_cipher_lappland;
        else if (strcmp(argv[2], "MISAKA") == 0)
            run = kgp_cipher_misaka;
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

    b128 key = {0};
    if (ret)
    {
        for (u8 i = 0; i < 32; i ++)
        {
            u64 *out = (i < 16) ? &(key.u64[0]) : &(key.u64[1]);
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
        ret = kgp_mode_cbc128(a, b, run, key, invert);

    if (a)
        fclose(a);
    if (b)
        fclose(b);

    return (ret) ? EXIT_SUCCESS : EXIT_FAILURE;
}
