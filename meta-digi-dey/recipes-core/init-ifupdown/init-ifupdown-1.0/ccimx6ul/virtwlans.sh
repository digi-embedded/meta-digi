#!/bin/sh
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

# This will create a second wireless network device
if [ -s "/proc/device-tree/wireless/mac-address1" ] &&
   [ -s "/proc/device-tree/wireless/mac-address2" ] &&
   [ -s "/proc/device-tree/wireless/mac-address3" ]; then
   :
else
	echo "WARNING: Using default MAC addresses for virtual interfaces, please "
	echo "program them referring to the Digi U-Boot Documentation"
fi

# This will create a second wireless network device
iw dev wlan0 interface add wlan1 type managed
