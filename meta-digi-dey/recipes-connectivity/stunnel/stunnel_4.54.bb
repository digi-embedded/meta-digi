require stunnel.inc

PR = "${DISTRO}.${INC_PR}.0"

SRC_URI = "http://www.stunnel.org/downloads/stunnel-${PV}.tar.gz \
	   file://automake.patch \
	   file://init \
	   file://stunnel.conf"

SRC_URI[md5sum] = "c2b1db99e3ed547214568959a8ed18ac"
SRC_URI[sha256sum] = "b7e1b9e63569574dbdabee8af90b8ab88db3fe13dcb1268d59a1408c56e6de7b"
