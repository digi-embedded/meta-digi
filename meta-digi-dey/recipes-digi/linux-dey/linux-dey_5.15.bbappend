# Append to the existing kernel recipe for linux-dey
FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

# Ensure we're building the correct kernel version
PV = "5.15"

# Include additional kernel configuration for USB-to-UART support
SRC_URI += "file://usb-serial.cfg"

# Apply configuration fragment
do_configure:append() {
  # Apply the configuration fragment
    cat ${WORKDIR}/usb-serial.cfg >> ${B}/.config
    yes '' | make oldconfig
}

# Ensure the kernel rebuilds with our changes
do_compile[depends] += "virtual/kernel:do_configure"
