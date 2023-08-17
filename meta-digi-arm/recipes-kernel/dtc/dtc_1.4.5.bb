require dtc.inc

FILESEXTRAPATHS:prepend := "${THISDIR}/${BPN}:"

LIC_FILES_CHKSUM = "file://GPL;md5=94d55d512a9ba36caa9b7df079bae19f \
		    file://libfdt/libfdt.h;beginline=3;endline=52;md5=fb360963151f8ec2d6c06b055bcbb68c"

SRCREV = "22a65c5331c22979d416738eb756b9541672e00d"

SRC_URI:append = " \
    file://0001-Remove-redundant-YYLOC-global-declaration.patch \
    file://0001-fdtdump-Fix-gcc11-warning.patch \
"

S = "${WORKDIR}/git"

BBCLASSEXTEND = "native nativesdk"
