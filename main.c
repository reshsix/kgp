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
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

#define ROTL(a, b) (((a) << (b)) | ((a) >> ((sizeof(b) * 8)  - (b))))
#define ROTR(a, b) (((a) >> (b)) | ((a) << ((sizeof(b) * 8)  - (b))))

static bool
error(const char *s)
{
    fprintf(stderr, "kgp: %s\n", s);
    return false;
}

/* Feistel network implementation */

struct feistel
{
    u64 iv[2];
    u8 rounds;
    u64 (*function)(u64, u64);
};

static void
feistel(struct feistel *c, u64 *keys, u64 *data, bool invert)
{
    u64 *l = &(data[0]);
    u64 *r = &(data[1]);

    u64 tmp = 0;
    for (u8 i = 0; i < c->rounds; i++)
    {
        tmp = *l;
        *l = *r;

        if (!invert)
            *r = c->function(*r, keys[i]);
        else
            *r = c->function(*r, keys[c->rounds - i - 1]);

        *r ^= tmp;
    }

    tmp = *l;
    *l = *r;
    *r = tmp;
}

/* Encryption modes implementation */

static bool
cbc_encrypt(FILE *src, FILE *dest, struct feistel *c, u64 *keys)
{
    bool ret = true;

    u64 vec[2] = {c->iv[0], c->iv[1]};
    while (ret)
    {
        u64 buf[2] = {0};

        size_t read = fread(&buf, 1, sizeof(buf), src);
        if (read == 0)
        {
            if (ferror(src))
                ret = error(strerror(errno));
            break;
        }

        if (read != sizeof(buf))
        {
            size_t pad = sizeof(buf) - read;
            memset(&(((u8*)buf)[read]), pad, pad);
        }

        buf[0] ^= vec[0];
        buf[1] ^= vec[1];

        feistel(c, keys, buf, false);

        vec[0] = buf[0];
        vec[1] = buf[1];

        if (fwrite(buf, sizeof(buf), 1, dest) < 1)
            ret = error(strerror(errno));
    }

    return ret;
}

static bool
cbc_decrypt(FILE *src, FILE *dest, struct feistel *c, u64 *keys)
{
    bool ret = true;

    u64 vec[2] = {c->iv[0], c->iv[1]};
    while (true)
    {
        u64 buf[2] = {0};

        size_t read = fread(buf, 1, sizeof(buf), src);
        if (read == 0)
        {
            if (ferror(src))
                ret = error(strerror(errno));
            break;
        }

        u64 buf2[2] = {buf[0], buf[1]};

        feistel(c, keys, buf, true);

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

    return ret;
}

/* Custom ciphers implementation */

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

static bool
lappland_run(FILE *src, FILE *dest, u64 key[static 2], bool invert)
{
    bool ret = false;

    struct feistel lappland = {.iv = {0x7E8A5, 0xDEADBEEF}, .rounds = 16,
                               .function = lappland_round};

    /* Expanding and rotating XOR 128bits key into 16x64bits */
    u64 keys[16] = {0};
    for (u8 i = 0; i < 16; i++)
    {
        u64 x = key[i % 2];
        x ^= ROTL(x, i);
        keys[i] = x;
    }

    ret = ((!invert) ? cbc_encrypt : cbc_decrypt)(src, dest, &lappland, keys);

    return ret;
}

/* Main functions */

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

    bool (*run)(FILE *, FILE *, u64 [static 2], bool) = NULL;
    if (ret)
    {
        if (strcmp(argv[2], "LAPPLAND") == 0)
            run = lappland_run;
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
        ret = run(a, b, key, invert);

    if (a)
        fclose(a);
    if (b)
        fclose(b);

    return (ret) ? EXIT_SUCCESS : EXIT_FAILURE;
}
