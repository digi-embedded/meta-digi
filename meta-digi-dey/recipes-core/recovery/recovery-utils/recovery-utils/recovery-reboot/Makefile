#
# Copyright (c) 2017, Digi International Inc.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, you can obtain one at http://mozilla.org/MPL/2.0/.
#
# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
# WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
# ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
# WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
# ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
# OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
#

PROGRAM := recovery-reboot

GIT_REVISION := $(shell git rev-parse --verify --short=7 HEAD 2>/dev/null)

CFLAGS += -Wall -DGIT_REVISION=\"$(if $(GIT_REVISION),-g$(GIT_REVISION))\"

#
# In general, when a library is installed in the toolchain the correct way to
# set the flags would be:
#
# CFLAGS += $(shell pkg-config --cflags recovery)
# LDLIBS += $(shell pkg-config --libs --static recovery)
#
# But in this case as we want to use the library not installed and from the
# local build directory, we adjust manually the flags.
#
CFLAGS  += -I../include
LDFLAGS += -L../lib
LDLIBS  += -lrecovery -lubootenv

all: $(PROGRAM)

.PHONY: install
install: $(PROGRAM)
	install -d $(DESTDIR)/usr/bin
	install -m 0755 $(PROGRAM) $(DESTDIR)/usr/bin/
	ln -sf $(PROGRAM) $(DESTDIR)/usr/bin/update-firmware

.PHONY: clean
clean:
	-rm -f *.o $(PROGRAM)
