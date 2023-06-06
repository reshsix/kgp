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

#ifndef KGP_STRUCTS_H
#define KGP_STRUCTS_H

#include <kgp/types.h>

struct kgp_struct_feistel
{
    u8 rounds;
    u64 (*function)(u64, u64);
};

void kgp_struct_feistel(struct kgp_struct_feistel *f,
                        u64 *subkeys, u64 *data, bool invert);

#endif
