# Copyright (C) 2013 Digi International
# Copyright (C) 2012 O.S. Systems Software LTDA.
# Released under the MIT license (see COPYING.MIT for the terms)

DESCRIPTION = "Linux kernel for Digi's imx based platforms"
LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://COPYING;md5=d7810fab7487fb0aad327b76f1be7cd7"

INC_PR = "r0"

inherit kernel

# Put a local version until we have a true SRCREV to point to
LOCALVERSION ?= "+dey"
SCMVERSION ?= "y"

kernel_conf_variable() {
	CONF_SED_SCRIPT="$CONF_SED_SCRIPT /CONFIG_$1[ =]/d;"
	if test "$2" = "n"
	then
		echo "# CONFIG_$1 is not set" >> ${S}/.config
	else
		echo "CONFIG_$1=$2" >> ${S}/.config
	fi
}

do_configure_prepend() {
	echo "" > ${S}/.config
	CONF_SED_SCRIPT=""

	kernel_conf_variable LOCALVERSION "\"${LOCALVERSION}\""
	kernel_conf_variable LOCALVERSION_AUTO y

	sed -e "${CONF_SED_SCRIPT}" < '${WORKDIR}/defconfig' >> '${S}/.config'

	for i in $(echo ${WORKDIR}/*.cfg); do
		[ "${i}" = "${WORKDIR}/*.cfg" ] && continue
		cat ${i} >> '${S}/.config'
	done

	if [ "${SCMVERSION}" = "y" ]; then
		# Add GIT revision to the local version
		head=`git rev-parse --verify --short HEAD 2> /dev/null`
		printf "%s%s" +g $head > ${S}/.scmversion
	fi
}

do_deploy_append() {
	cd ${DEPLOYDIR}
	ln -sf ${KERNEL_IMAGE_BASE_NAME}.bin ${KERNEL_IMAGE_SYMLINK_NAME}
	cd -
}

# install nedded headers for imx-test compilation
do_install_append() {
	# bounds.h may be used by a module and is currently missing
	if [ -d include/generated ]; then
		cp -a include/generated/* $kerneldir/include/generated/
	fi

	# Host architecture object file
	rm -f $kerneldir/scripts/kconfig/kxgettext.o
}

sysroot_stage_all_append() {
	# Copy native binaries need for imx-test build onto sysroot
	mkdir -p ${SYSROOT_DESTDIR}${KERNEL_SRC_PATH}/scripts/basic \
	         ${SYSROOT_DESTDIR}${KERNEL_SRC_PATH}/scripts/mod
	cp ${S}/scripts/basic/fixdep ${SYSROOT_DESTDIR}${KERNEL_SRC_PATH}/scripts/basic
	cp ${S}/scripts/mod/modpost ${SYSROOT_DESTDIR}${KERNEL_SRC_PATH}/scripts/mod
}
