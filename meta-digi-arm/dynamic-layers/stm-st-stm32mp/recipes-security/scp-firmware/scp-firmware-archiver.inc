#
# Archiver Configuration
#
SRC_URI:append = " file://README.HOW_TO.txt "
COPYLEFT_LICENSE_INCLUDE:append = " BSD-3* "

inherit archiver
ARCHIVER_MODE[src] = "original"

inherit archiver_stm32mp_clean
