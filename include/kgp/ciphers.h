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

#ifndef KGP_CIPHERS_H
#define KGP_CIPHERS_H

#include <kgp/types.h>

void kgp_cipher_lappland(b128 *data, b128 key, bool invert);
void kgp_cipher_misaka(b128 *data, b128 key, bool invert);

#endif
