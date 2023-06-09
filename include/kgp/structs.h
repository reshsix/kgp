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

void kgp_struct_feistel128(u64 (*f)(u64, u64), u8 rounds,
                           u64 subkeys[], b128 *data, bool invert);

void kgp_struct_feistel256(b128 (*f)(b128, b128), u8 rounds,
                           b128 subkeys[], b256 *data, bool invert);

#endif
