SUMMARY = "Python Library for Coral devices"
DESCRIPTION = "Python Library to run inferences and perform on-device transfer learning with TensorFlow Lite models on Coral devices"
SECTION = "devel/python"
HOMEPAGE = "https://coral.ai/software/#pycoral-api"
LICENSE = "Apache-2.0"
LIC_FILES_CHKSUM = "file://${S}/pycoral-1.0.1.dist-info/LICENSE;md5=d8927f3331d2b3e321b7dd1925166d25"

PYTHON_PACKAGE = "pycoral-1.0.1-cp38-cp38-linux:aarch64.whl"

SRC_URI = "git://github.com/google-coral/pycoral.git;protocol=https"
SRCREV = "d4b9f572fa3baef9d854483a171e02a6b3f9dbd0"

SRC_URI += "https://github.com/google-coral/pycoral/releases/download/v1.0.1/${PYTHON_PACKAGE};downloadfilename=${PYTHON_PACKAGE};subdir=${BP};name=python-wheel"
SRC_URI[python-wheel.md5sum] = "ea89677a47d7d81d2558b8dbbae44d95"
SRC_URI[python-wheel.sha256sum] = "894468447192fbcf946157db0f3b6424ece6d70bcec1243892d27cd7b521f176"

DEPENDS = "python3 python3-pip-native python3-wheel-native curl-native ca-certificates-native"

RDEPENDS:${PN} = "${PYTHON_PN} \
                  ${PYTHON_PN}-numpy \
                  ${PYTHON_PN}-pycairo \
                  ${PYTHON_PN}-pygobject \
                  ${PYTHON_PN}-pillow \
                  libedgetpu \
                  tensorflow-lite-coral \
"

inherit python3native

do_unpack[depends] += "unzip-native:do_populate_sysroot"

do_unpack_extra(){
    [ -d ${S} ] || mkdir -p ${S}
    cd ${S}
    unzip -q -o ${S}/${PYTHON_PACKAGE} -d ${S}
}
addtask unpack_extra after do_unpack before do_patch


do_configure() {
    export CURL_CA_BUNDLE=${STAGING_DIR_NATIVE}/etc/ssl/certs/ca-certificates.crt

    cd ${WORKDIR}/git
    bash examples/install_requirements.sh classify_image.py
}

do_install() {
    # Install examples
    install -d ${D}/opt/pycoral
    install -m 0555 ${WORKDIR}/git/test_data/* ${D}/opt/pycoral
    install -m 0555 ${WORKDIR}/git/examples/classify_image.py ${D}/opt/pycoral

    # Install pip package
    install -d ${D}/${PYTHON_SITEPACKAGES_DIR}
    ${STAGING_BINDIR_NATIVE}/pip3 install --disable-pip-version-check -v \
        -t ${D}/${PYTHON_SITEPACKAGES_DIR} --no-cache-dir --no-deps \
        ${WORKDIR}/${BP}/pycoral-*.whl
}

FILES:${PN} += "\
    ${libdir}/${PYTHON_DIR}/site-packages/* \
    /opt/pycoral/* \
"

INSANE_SKIP:${PN} += "already-stripped"

COMPATIBLE_MACHINE = "(-)"
COMPATIBLE_MACHINE:aarch64 = "(.*)"
