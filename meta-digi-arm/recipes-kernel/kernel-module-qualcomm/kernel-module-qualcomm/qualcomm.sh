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

# Do nothing if the module is already loaded
grep -qws 'wlan' /proc/modules && exit 0

FIRMWARE_DIR="/lib/firmware"
MACFILE="${FIRMWARE_DIR}/wlan/wlan_mac.bin"
TMP_MACFILE="$(mktemp -t wlan_mac.XXXXXX)"

# Read the MACs from DeviceTree. We can have up to four wireless interfaces
# The only required one is wlan0 that is mapped with device tree mac address
# without suffix.
for index in $(seq 0 3); do
	MAC_ADDR="$(hexdump -ve '1/1 "%02X"' /proc/device-tree/wireless/mac-address${index%0} 2>/dev/null)"
	if [ "${index}" = "0" ] && { [ -z "${MAC_ADDR}" ] || [ "${MAC_ADDR}" = "00:00:00:00:00:00" ]; }; then
		# Set a default MAC for wlan0
		MAC_ADDR="0004F3FFFFFB"
	fi

	# Add the MAC address to the firmware file with the expected format
	echo "Intf${index}MacAddress=${MAC_ADDR}" >> ${TMP_MACFILE}
done

# Override the MAC firmware file only if the MAC file has changed.
if ! cmp -s ${TMP_MACFILE} ${MACFILE}; then
	cp ${TMP_MACFILE} ${MACFILE}
fi
rm -f "${TMP_MACFILE}"

OTP_REGION_CODE="$(cat /proc/device-tree/digi,hwid,cert 2>/dev/null)"
DTB_REGION_CODE="$(cat /proc/device-tree/wireless/regulatory-domain 2>/dev/null)"
US_CODE="0x0"
WW_CODE="0x1"
JP_CODE="0x2"
# Check if the DTB_REGION_CODE is in the list of valid codes,
# if not use the OTP programmed value.
case "${DTB_REGION_CODE}" in
	${US_CODE} | ${WW_CODE} | ${JP_CODE})
		REGULATORY_DOMAIN="${DTB_REGION_CODE}";;
	*)
		if [ -n "${DTB_REGION_CODE}" ]; then
			logger -t qca6564 "[WARN] Invalid region code in device tree, using OTP value"
		fi
		REGULATORY_DOMAIN="${OTP_REGION_CODE}";;
esac


# Create symbolic links to the proper FW files depending on the country region
# Use a sub-shell here to change to firmware directory
(
	cd "${FIRMWARE_DIR}"

	BDATA_SOURCE="bdwlan30_US.bin"
	case "${REGULATORY_DOMAIN}" in
		${US_CODE})
			logger -t qca6564 "Setting US wireless region";;
		${WW_CODE}|${JP_CODE})
			if [ -f "bdwlan30_World.bin" ]; then
				logger -t qca6564 "Setting WW (world wide) wireless region"
				BDATA_SOURCE="bdwlan30_World.bin"
			else
				logger -t qca6564 "[WARN] No WW (worldwide) board data file, using US"
			fi
			;;
		"")
			logger -t qca6564 "[WARN] region code not found, using US";;
		*)
			logger -t qca6564 "[WARN] Invalid region code, using US";;
	esac

	# We don't want to rewrite NAND every time we boot so only
	# change the links if they are wrong.
	BDATA_LINK="bdwlan30.bin"
	UTFBDATA_LINK="utfbd30.bin"
	if [ ! -e "${BDATA_LINK}" ] || ! cmp -s "${BDATA_LINK}" "${BDATA_SOURCE}"; then
		ln -sf "${BDATA_SOURCE}" "${BDATA_LINK}"
		ln -sf "${BDATA_SOURCE}" "${UTFBDATA_LINK}"
	fi
)

# Load the wireless module with the params defined in modprobe.d/qualcomm.conf
modprobe wlan

# Verify the interface is present
if [ -d "/sys/class/net/wlan0" ]; then
	# Create 'wlan1' virtual interface
	virtwlans.sh
else
	logger -t qca6564 "[ERROR] Loading qca6564 module"
fi
