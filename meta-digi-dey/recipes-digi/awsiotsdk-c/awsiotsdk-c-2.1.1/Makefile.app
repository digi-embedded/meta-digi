# ***************************************************************************
# Copyright (c) 2017 Digi International Inc.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this file,
# You can obtain one at http://mozilla.org/MPL/2.0/.
#
# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
# REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
# AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
# INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
# LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
# OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
# PERFORMANCE OF THIS SOFTWARE.
#
# Digi International Inc. 11001 Bren Road East, Minnetonka, MN 55343
#
# ***************************************************************************
# Use GNU C Compiler.
CC ?= gcc

# Generated Executable Name.
EXECUTABLE = $(notdir $(CURDIR))

# Location of Source Code.
SRC = .

#IoT client directory.
IOT_CLIENT_DIR = ../../..

PLATFORM_DIR = $(IOT_CLIENT_DIR)/platform/linux/mbedtls
PLATFORM_COMMON_DIR = $(IOT_CLIENT_DIR)/platform/linux/common

CFLAGS += -I $(SRC)
CFLAGS += -I $(IOT_CLIENT_DIR)/include
CFLAGS += -I $(IOT_CLIENT_DIR)/external_libs/jsmn
CFLAGS += -I $(PLATFORM_COMMON_DIR)
CFLAGS += -I $(PLATFORM_DIR)
CFLAGS += -Wall -g
CFLAGS += $(LOG_FLAGS)

# Libraries to Link
LIBS += $(shell PKG_CONFIG_PATH=../../..:$${PKG_CONFIG_PATH} pkg-config --libs --static awsiotsdk)

# Linking Flags.
LDFLAGS += -L$(IOT_CLIENT_DIR)/src $(DFLAGS)

# Target output to generate.
SRCS = $(wildcard $(SRC)/*.c)

OBJS = $(SRCS:.c=.o)

.PHONY: all
all:  $(EXECUTABLE)

$(EXECUTABLE): $(OBJS)
	$(CC) $(LDFLAGS) $^ $(LIBS) -o $@

.PHONY: install
install: $(EXECUTABLE)
	install -d $(DESTDIR)/usr/bin
	install -m 0755 $< $(DESTDIR)/usr/bin/

.PHONY: clean
clean:
	rm -f $(EXECUTABLE) $(OBJS)

