#!/bin/sh
#
# Copyright (c) 2017-2019, Digi International Inc.
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

if basename "${DEVPATH}" | grep -qs "gpiochip0$"; then
	# Use 'gpiochip0' event to set group and mode for 'export/unexport' files
	chown root:digiapix /sys/class/gpio/export /sys/class/gpio/unexport
	chmod g+w /sys/class/gpio/export /sys/class/gpio/unexport
elif basename "${DEVPATH}" | grep -qs "pwmchip[0-9]\+$" && [ "${ACTION}" = "add" ] ; then
	# Set group and mode for pwmchip's 'export/unexport' files
	chown root:digiapix /sys${DEVPATH}/export /sys${DEVPATH}/unexport
	chmod g+w /sys${DEVPATH}/export /sys${DEVPATH}/unexport
elif basename "${DEVPATH}" | grep -qs "pwmchip[0-9]\+$" && [ "${ACTION}" = "change" ] ; then
	# Set group and mode for 'pwmX' channel and all files inside it...
	chown root:digiapix /sys${DEVPATH}/${EXPORT} /sys${DEVPATH}/${EXPORT}/*
	chmod g+w /sys${DEVPATH}/${EXPORT}/*
else
	# Change group and mode of the sysfs files
	chown -h root:digiapix /sys${DEVPATH}/*
	chmod g+w /sys${DEVPATH}/*
fi
