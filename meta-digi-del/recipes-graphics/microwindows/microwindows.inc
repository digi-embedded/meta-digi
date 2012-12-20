DESCRIPTION = "Microwindows Graphical Engine"
SECTION = "x11/wm"
PRIORITY = "optional"
DEPENDS = "libpng jpeg zlib"
LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://src/LICENSE;md5=537b9004889eb701c48fc1fe78d9c30e"

PR="r2"

PARALLEL_MAKE = ""

EXTRA_OEMAKE = " \
  -C ${S}/src \
  ARMTOOLSPREFIX=${TARGET_PREFIX}	\
  INCJPEG=${STAGING_INCDIR} \
  LIBJPEG=${STAGING_LIBDIR}/libjpeg.so \
  INCPNG=${STAGING_INCDIR} \
  LIBPNG=${STAGING_LIBDIR}/libpng.so \
  LIBZ=${STAGING_LIBDIR}/libz.so \
  INSTALL_PREFIX=${D}${prefix} \
  INSTALL_OWNER1= \
  INSTALL_OWNER2= \
 "

do_compile() {
  cp ${WORKDIR}/defconfig ${S}/src/config
  oe_runmake
}

do_install() {
  oe_runmake install all

# the next 3 lines are necessary for snapshot build since it has CVS cruft in the tarball
  rm -rf ${S}/src/lib/CVS
  rm -rf ${S}/src/bin/CVS
  rm -f ${S}/src/bin/fonts

# convbdf is a host tool - don't install it  
  mkdir ${S}/src/hostbin
  mv ${S}/src/bin/convbdf ${S}/src/hostbin

  install -d ${D}${bindir} ${D}$bindir}/fonts ${D}${libdir} 
  install -m 0755 ${S}/src/lib/*   ${D}${libdir}/
  install -d ${D}/${includedir}/microwin
  install -m 0644 ${S}/src/include/* ${D}/${includedir}/microwin/

# No binaries compiled - change the config file if demoes and apps are needed
#  install -m 0755 ${S}/src/bin/*   ${D}${bindir}/
}

FILES_${PN}="${bindir}/* ${libdir}/*"
