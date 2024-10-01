DESCRIPTION = "TIM-VX is a software integration module provided by VeriSilicon to facilitate \
deployment of Neural-Networks on OpenVX enabled ML accelerators. It serves as the backend \
binding for runtime frameworks such as Android NN, Tensorflow-Lite, MLIR, TVM and more."
SUMMARY = "Tensor Interface Module for OpenVX"
HOMEPAGE = "https://github.com/VeriSilicon/TIM-VX"
LICENSE = "MIT"

LIC_FILES_CHKSUM = "file://LICENSE;md5=d72cd187d764d96d91db827cb65b48a7"

SRCBRANCH_tim_vx = "main"
SRCREV_tim_vx = "33f3a4f176ff9c407479eaf6be78c52bb3c7a939"
SRC_URI ="git://github.com/VeriSilicon/TIM-VX.git;branch=${SRCBRANCH_tim_vx};name=tim_vx;destsuffix=tim_vx_git/;protocol=https"
SRC_URI += " file://0001-tim-vx-tests-disable-AVG_ANDROID-tests.patch"


SRCBRANCH_googletest = "main"
SRCREV_googletest = "eab0e7e289db13eabfc246809b0284dac02a369d"
SRC_URI +="git://github.com/google/googletest;branch=${SRCBRANCH_googletest};name=googletest;destsuffix=googletest/;protocol=https "


PV = "1.1.57+git${SRCREV_tim_vx}"
PV_googletest = "1.14.0"

S = "${WORKDIR}/tim_vx_git"

# Only compatible with stm32mp25
COMPATIBLE_MACHINE = "stm32mp25common"

python () {
    #Get major of the PV variable
    version = d.getVar('PV')
    version = version.split("+")
    version_base = version[0]
    version = version_base.split(".")
    major = version[0]
    d.setVar('MAJOR', major)
    d.setVar('PVB', version_base)
}

inherit cmake
DEPENDS += " patchelf-native \
	     gcnano-driver-stm32mp \
	     gcnano-userland \
             gtest \
             googletest \
	"

EXTRA_OECMAKE =  " \
    -DCONFIG=YOCTO \
    -DCMAKE_SYSROOT=${RECIPE_SYSROOT} \
    -DTIM_VX_ENABLE_TEST=ON \
    -DCMAKE_SKIP_RPATH=TRUE \
    -DFETCHCONTENT_FULLY_DISCONNECTED=OFF \
    -DTIM_VX_USE_EXTERNAL_OVXLIB=ON \
    -DTIM_VX_DBG_ENABLE_TENSOR_HNDL=OFF \
    -DOVXLIB_INC=${S}/src/tim/vx/internal/include/ \
    -DOVXLIB_LIB=${STAGING_LIBDIR}/libovxlib.so \
    -DFETCHCONTENT_SOURCE_DIR_GOOGLETEST=${WORKDIR}/googletest \
"
do_configure[network] = "1"

do_install() {
    # Install libtim-vx.so into libdir
    install -d ${D}${libdir}
    install -d ${D}/usr/local/bin/${PN}-${PVB}
    install -d ${D}/home/weston

    install -m 0755 ${WORKDIR}/build/src/tim/libtim-vx.so ${D}${libdir}/libtim-vx.so.${PVB}
    patchelf --set-soname libtim-vx.so ${D}${libdir}/libtim-vx.so.${PVB}

    ln -sf libtim-vx.so.${PVB} ${D}${libdir}/libtim-vx.so.${MAJOR}
    ln -sf libtim-vx.so.${PVB} ${D}${libdir}/libtim-vx.so

    # Install other libraries for benchmark
    install -m 0755 ${WORKDIR}/build/lib/libgtest_main.so ${D}${libdir}/libgtest_main.so.${PV_googletest}
    install -m 0755 ${WORKDIR}/build/lib/libgtest.so      ${D}${libdir}/libgtest.so.${PV_googletest}
    install -m 0755 ${WORKDIR}/build/lib/libgmock_main.so ${D}${libdir}/libgmock_main.so
    install -m 0755 ${WORKDIR}/build/lib/libgmock.so      ${D}${libdir}/libgmock.so
    install -m 0755 ${WORKDIR}/build/src/tim/unit_test    ${D}/usr/local/bin/${PN}-${PVB}/TIM-VX_test

    # Include
    install -d ${D}${includedir}
    cp -r ${S}/include/tim ${D}${includedir}
    cp -r ${STAGING_INCDIR}/CL/cl_viv_vx_ext.h ${D}/usr/local/bin/${PN}-${PVB}/cl_viv_vx_ext.h
    cp -r ${STAGING_INCDIR}/CL/cl_viv_vx_ext.h ${D}/home/weston/cl_viv_vx_ext.h
}

PACKAGES =+ "${PN}-tools"
FILES_SOLIBSDEV = ""

FILES:${PN}-tools = "   /usr/local/bin/${PN}-${PVB}/TIM-VX_test \
			/usr/local/bin/${PN}-${PVB}/cl_viv_vx_ext.h \
			/home/weston/cl_viv_vx_ext.h \
			${libdir}/libgtest_main.so.${PV_googletest} \
			${libdir}/libgtest.so.${PV_googletest} \
			${libdir}/libgmock_main.so \
			${libdir}/libgmock.so \
"

FILES:${PN} += " ${libdir}/libtim-vx.so.${MAJOR} \
                 ${libdir}/libtim-vx.so.${PVB}   \
                 ${libdir}/libtim-vx.so \
"

INSANE_SKIP:${PN} += " dev-so "