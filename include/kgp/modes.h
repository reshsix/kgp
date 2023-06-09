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

bool kgp_mode_cbc128(FILE *src, FILE *dest,
                     void (*cipher)(b128 *, b128, bool),
                     b128 key, bool invert);

#endif
