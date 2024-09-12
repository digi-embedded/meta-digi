# Copyright (C) 2024, Digi International Inc. 

# Specify global CFLAGS for target to support 64 bit time
GLIBC_64BIT_TIME_FLAGS = " -D_TIME_BITS=64 -D_FILE_OFFSET_BITS=64"

# Only needed for some 32-bit architectures
TARGET_CC_ARCH:append:arm = "${GLIBC_64BIT_TIME_FLAGS}"

# Do not apply the 64Bit compile flag for the following packages
# to avoid build errors
GLIBC_64BIT_TIME_FLAGS:pn-glibc = ""
GLIBC_64BIT_TIME_FLAGS:pn-kbd = ""
GLIBC_64BIT_TIME_FLAGS:pn-v4l-utils = ""
GLIBC_64BIT_TIME_FLAGS:pn-pulseaudio = ""
GLIBC_64BIT_TIME_FLAGS:pn-imx-gst1.0-plugin = ""
GLIBC_64BIT_TIME_FLAGS:pn-zlib = ""
