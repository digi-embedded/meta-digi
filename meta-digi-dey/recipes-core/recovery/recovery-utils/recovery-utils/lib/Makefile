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

LIBRECOVERY_AR = librecovery.a

CFLAGS += -Wall

all: $(LIBRECOVERY_AR)

$(LIBRECOVERY_AR): recovery.o
	$(AR) -rcs $@ $^

.PHONY: install
install: $(LIBRECOVERY_AR)
	install -d $(DESTDIR)/usr/lib/pkgconfig $(DESTDIR)/usr/include/librecovery
	install -m 0644 $(LIBRECOVERY_AR) $(DESTDIR)/usr/lib/
	install -m 0644 recovery.pc $(DESTDIR)/usr/lib/pkgconfig/
	install -m 0644 ../include/recovery.h $(DESTDIR)/usr/include/librecovery/

.PHONY: clean
clean:
	-rm -f *.o $(LIBRECOVERY_AR)
