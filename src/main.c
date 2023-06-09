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

static bool eflag = false;
static bool
error(const char *s)
{
    eflag = true;
    fprintf(stderr, "kgp: %s\n", s);
    return false;
}

static void
usage_cipher(const char *name, const char *status,
             const char *confusion, const char *diffusion,
             const char *structure, const char *size, const char *bias,
             const char *avalanche)
{
    fprintf(stderr, "%s\n", name);
    fprintf(stderr, "\tStatus: %s\n", status);
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
    fprintf(stderr, "\n[ Block ciphers ]\n\n");
    usage_cipher("LAPPLAND", "\033[31mDeprecated\033[0m",
                             "S-box made of Signore dei Lupi lyrics",
                             "RX operations", "16 rounds Feistel",
                             "Key 128, Block 128",
                             "Linear 0.5, Differential 0.148",
                             "Minimum 0.41, Average 0.50");
    usage_cipher("MISAKA",   "\033[33mPotential issues\033[0m",
                             "S-box made of the capacitor charge formula",
                             "ARX operations", "16 rounds Feistel",
                             "Key 128, Block 128",
                             "Linear 0.125, Differential 0.039",
                             "Minimum 0.35, Average 0.50");
    return false;
}

static bool
parse(int argc, char *argv[], bool *invert,
      void (**cipher128)(b128 *, b128, bool),
      void (**cipher256)(b256 *, b256, bool),
      FILE **a, FILE **b, b128 *key128, b256 *key256)
{
    bool ret = true;

    (void)(cipher256);
    (void)(key256);
    if (argc != 5)
        ret = usage();

    if (ret)
    {
        if (strcmp(argv[1], "decrypt") == 0)
            *invert = true;
        else if (strcmp(argv[1], "encrypt") != 0)
            ret = usage();
    }

    if (ret)
    {
        if (strcmp(argv[2], "LAPPLAND") == 0)
            *cipher128 = kgp_cipher_lappland;
        else if (strcmp(argv[2], "MISAKA") == 0)
            *cipher128 = kgp_cipher_misaka;
        else
            ret = usage();
    }

    if (ret)
    {
        *a = fopen(argv[3], "rb");
        if (!(*a))
            ret = error(strerror(errno));
    }

    if (ret)
    {
        *b = fopen(argv[4], "wb");
        if (!(*b))
            ret = error(strerror(errno));
    }

    char *str = NULL;
    if (ret)
    {
        str = getenv("KGPKEY");
        if (!str || strlen(str) != 32)
            ret = error(strerror(EINVAL));
    }

    if (ret)
    {
        for (u8 i = 0; i < 32; i ++)
        {
            u64 *out = (i < 16) ? &(key128->u64[1]) : &(key128->u64[0]);
            u64 c = str[i];
            if (c >= '0' && c <= '9')
                *out |= (c - '0') << (i * 4);
            else if (c >= 'a' && c <= 'f')
                *out |= (c - 'a' + 10) << (i * 4);
            else if (c >= 'A' && c <= 'F')
                *out |= (c - 'A' + 10) << (i * 4);
            else
            {
                ret = error(strerror(EINVAL));
                break;
            }
        }
    }

    return ret;
}

struct context
{
    FILE *src, *dest;
    bool invert;
};

static bool
mode_in(void *ctx, u8 *buf, size_t size)
{
    bool ret = true;

    struct context *c = ctx;

    size_t read = fread(buf, 1, size, c->src);
    if (read == 0)
        ret = (ferror(c->src)) ? error(strerror(errno)) : false;

    if (ret && read != size)
    {
        if (!(c->invert))
        {
            size_t pad = size - read;
            memset(&(buf[read]), pad, pad);
        }
        else
            ret = error(strerror(EIO));
    }

    return ret;
}

static bool
mode_out(void *ctx, u8 *buf, size_t size)
{
    bool ret = true;

    struct context *c = ctx;

    u8 pad = 0;
    if (c->invert)
    {
        int ch = fgetc(c->src);
        if (ch == EOF)
        {
            pad = buf[size - 1];
            pad = (pad < size) ? pad : 0;
            for (u8 i = 1; i < pad; i++)
                if (buf[size - 1 - i] != pad)
                    pad = 0;
        }
        else
            ungetc(ch, c->src);
    }

    if (fwrite(buf, size - pad, 1, c->dest) < 1)
        ret = error(strerror(errno));

    return ret;
}

extern int
main(int argc, char *argv[])
{
    bool ret = true;

    setlocale(LC_ALL, "");

    bool invert = false;
    void (*cipher128)(b128 *, b128, bool) = NULL;
    void (*cipher256)(b256 *, b256, bool) = NULL;
    FILE *a = NULL, *b = NULL;
    b128 key128 = {0};
    b256 key256 = {0};
    ret = parse(argc, argv, &invert, &cipher128, &cipher256,
                &a, &b, &key128, &key256);

    (void)(cipher256);
    (void)(key256);

    struct context ctx = {.src = a, .dest = b, .invert = invert};
    if (ret)
        kgp_mode_cbc128(mode_in, &ctx, mode_out, &ctx,
                        cipher128, key128, invert);

    if (a) fclose(a);
    if (b) fclose(b);

    return (ret && !eflag) ? EXIT_SUCCESS : EXIT_FAILURE;
}
