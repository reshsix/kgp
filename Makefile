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

DESTDIR ?= /usr/local/
DESTDIR := $(shell realpath $(DESTDIR))

CFLAGS += -O2 -march=native -Wall -Wextra -Iinclude

.PHONY: all clean

all: build/kgp
clean:
	rm -rf build

install: build/kgp
	@printf '- Installing libkgp.a into %s/lib\n' $(DESTDIR)
	@install -c build/kgp $(DESTDIR)/lib
	@printf '- Installing kgp into %s/bin\n' $(DESTDIR)
	@install -c build/kgp $(DESTDIR)/bin
uninstall:
	@printf '- Uninstalling libkgp.a from %s/lib\n' $(DESTDIR)
	@rm -f "$(DESTDIR)/lib/libkgp.a"
	@printf '- Uninstalling kgp from %s/bin\n' $(DESTDIR)
	@rm -f "$(DESTDIR)/kgp"

build:
	@mkdir -p build

build/%.o: src/%.c | build
	@printf '- Compiling %s\n' $(notdir $@)
	@$(CC) $(CFLAGS) -c $< -o $@

build/libkgp.a: build/structs.o build/ciphers.o build/modes.o
	@printf '- Archiving %s\n' $(notdir $@)
	@ar cr $@ $^
	@ranlib $@

build/kgp: build/main.o build/libkgp.a | build
	@printf '- Building %s\n' $(notdir $@)
	@$(CC) $(CFLAGS) $^ -o $@ -Lbuild -lkgp
