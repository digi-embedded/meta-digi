require stunnel.inc

PR = "${DISTRO}.${INC_PR}.0"

SRC_URI = "http://www.stunnel.org/downloads/stunnel-${PV}.tar.gz \
	   file://automake.patch \
	   file://init \
	   file://stunnel.conf"

SRC_URI[md5sum] = "ac4c4a30bd7a55b6687cbd62d864054c"
SRC_URI[sha256sum] = "9cae2cfbe26d87443398ce50d7d5db54e5ea363889d5d2ec8d2778a01c871293"
