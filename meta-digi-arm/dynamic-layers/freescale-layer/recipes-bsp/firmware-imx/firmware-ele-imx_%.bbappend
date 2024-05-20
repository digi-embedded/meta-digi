# Copyright 2024 Digi International Inc.
FILESEXTRAPATHS:prepend := "${THISDIR}/${BPN}:"

# Forward-port the i.MX93 A0 fw from v0.1.0
SRC_URI:append:ccimx93 = " \
    file://mx93a0-ahab-container.img \
"
do_install:prepend:ccimx93() {
	# Copy our A0 firmware file to where the other firmware files are
	cp ${WORKDIR}/${SECO_FIRMWARE_NAME} ${S}
}
