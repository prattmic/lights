##
## This file is part of the libopencm3 project.
##
## Copyright (C) 2009 Uwe Hermann <uwe@hermann-uwe.de>
##
## This library is free software: you can redistribute it and/or modify
## it under the terms of the GNU Lesser General Public License as published by
## the Free Software Foundation, either version 3 of the License, or
## (at your option) any later version.
##
## This library is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU Lesser General Public License for more details.
##
## You should have received a copy of the GNU Lesser General Public License
## along with this library.  If not, see <http://www.gnu.org/licenses/>.
##

PREFIX		?= arm-none-eabi

# Be silent per default, but 'make V=1' will show all compiler calls.
ifneq ($(V),1)
Q := @
# Do not print "Entering directory ...".
MAKEFLAGS += --no-print-directory
endif

OPENCM3_DIR := $(realpath libopencm3)

all: bin

bin: RULES += bin
hex: RULES += hex
srec: RULES += srec
list: RULES += list
images: RULES += images

bin: build
hex: build
srec: build
list: build
images: build

build: lib lights

lib:
	$(Q)if [ ! "`ls -A libopencm3`" ] ; then \
		printf "######## ERROR ########\n"; \
		printf "\tlibopencm3 is not initialized.\n"; \
		printf "\tPlease run:\n"; \
		printf "\t$$ git submodule init\n"; \
		printf "\t$$ git submodule update\n"; \
		printf "\tbefore running make.\n"; \
		printf "######## ERROR ########\n"; \
		exit 1; \
		fi
	$(Q)$(MAKE) -C libopencm3

lights: lib
	@printf "  BUILD   $@\n";
	$(Q)$(MAKE) -C $@ OPENCM3_DIR=$(OPENCM3_DIR) $(RULES)

flash: lights
	@printf "  FLASH   $^\n";
	$(Q)$(MAKE) -C lights OPENCM3_DIR=$(OPENCM3_DIR) stlink-flash

clean: libopencm3.clean lights.clean

%.clean:
	$(Q)if [ -d $* ]; then \
		printf "  CLEAN   $*\n"; \
		$(MAKE) -C $* clean OPENCM3_DIR=$(OPENCM3_DIR) || exit $?; \
	fi;

.PHONY: build lib clean bin hex srec list images flash
