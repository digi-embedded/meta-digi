DESCRIPTION = "Open-source software for mathematics, science, and engineering. It includes modules for statistics, optimization, integration, linear algebra, Fourier transforms, signal and image processing, ODE solvers, and more."
SECTION = "devel/python"
HOMEPAGE = "https://www.scipy.org"
LICENSE = "BSD-3-Clause"
LIC_FILES_CHKSUM = "file://${S}/scipy-1.7.0.dist-info/LICENSE.txt;md5=caecddab96f03de0092b62333ea77f91"

PYTHON_PACKAGE = "scipy-1.7.0-cp38-cp38-manylinux_2_17_aarch64.manylinux2014_aarch64.whl"

SRC_URI = "https://files.pythonhosted.org/packages/d0/8d/3dbb59d78218b6a76f1ddb55db60ea5459fa7968655acb21252a59a10bc3/${PYTHON_PACKAGE};subdir=${BP}"
SRC_URI[md5sum] = "e2e369078c6b7ca29c952cb9971bc154"
SRC_URI[sha256sum] = "bd4399d4388ca0239a4825e312b3e61b60f743dd6daf49e5870837716502a92a"

DEPENDS = "python3 python3-pip-native python3-wheel-native"

RDEPENDS:${PN} = "${PYTHON_PN} \
                  ${PYTHON_PN}-numpy \
"
RPROVIDES:${PN} += "\
    libgfortran-daac5196.so.5.0.0(GFORTRAN_8)(64bit) \
"

inherit python3native

do_unpack[depends] += "unzip-native:do_populate_sysroot"

do_unpack_extra(){
    [ -d ${S} ] || mkdir -p ${S}
    cd ${S}
    unzip -q -o ${S}/${PYTHON_PACKAGE} -d ${S}
}
addtask unpack_extra after do_unpack before do_patch

do_install() {
    # Install pip package
    install -d ${D}/${PYTHON_SITEPACKAGES_DIR}
    ${STAGING_BINDIR_NATIVE}/pip3 install --disable-pip-version-check -v \
        -t ${D}/${PYTHON_SITEPACKAGES_DIR} --no-cache-dir --no-deps \
        ${WORKDIR}/${BP}/scipy-*.whl
}

FILES:${PN} += "\
    ${libdir}/${PYTHON_DIR}/site-packages/* \
"

INSANE_SKIP:${PN} += "already-stripped"

COMPATIBLE_MACHINE = "(-)"
COMPATIBLE_MACHINE:aarch64 = "(.*)"
