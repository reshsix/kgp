# This file is part of kgp.

# kgp is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published
# by the Free Software Foundation, version 3.

# kgp is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with kgp. If not, see <https://www.gnu.org/licenses/>.

DESTDIR ?= /usr/local/bin/
CFLAGS += -O2 -march=native -Wall -Wextra -Iinclude

.PHONY: all clean

all: build/kgp
clean:
	rm -rf build

install: build/kgp
	install -c build/kgp $(DESTDIR)
uninstall:
	rm -f $(shell realpath "$(DESTDIR)/kgp")

build:
	mkdir -p build

build/%.o: src/%.c | build
	$(CC) $(CFLAGS) -c $< -o $@

build/kgp: build/structs.o build/ciphers.o \
           build/modes.o build/main.o | build
	$(CC) $(CFLAGS) $^ -o $@
