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

#ifndef KGP_MODES_H
#define KGP_MODES_H

#include <kgp/types.h>

void kgp_mode_cbc128(bool (*in) (void *, u8 *, size_t), void *ctx_in,
                     bool (*out)(void *, u8 *, size_t), void *ctx_out,
                     void (*cipher)(b128 *, b128, bool),
                     b128 key, bool invert);

#endif
