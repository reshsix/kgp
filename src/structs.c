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

extern void
kgp_struct_feistel(struct kgp_struct_feistel *f,
                   u64 *subkeys, u64 *data, bool invert)
{
    u64 *l = &(data[0]);
    u64 *r = &(data[1]);

    u64 tmp = 0;
    for (u8 i = 0; i < f->rounds; i++)
    {
        tmp = *l;
        *l = *r;

        if (!invert)
            *r = f->function(*r, subkeys[i]);
        else
            *r = f->function(*r, subkeys[f->rounds - i - 1]);

        *r ^= tmp;
    }

    tmp = *l;
    *l = *r;
    *r = tmp;
}
