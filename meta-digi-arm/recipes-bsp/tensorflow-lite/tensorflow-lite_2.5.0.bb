SUMMARY = "TensorFlow Lite Python Library"
DESCRIPTION = "TensorFlow Lite is the official solution for running machine learning models on mobile and embedded devices."
SECTION = "devel/python"
HOMEPAGE = "https://www.tensorflow.org/lite/"
LICENSE = "Apache-2.0"
LIC_FILES_CHKSUM = "file://${S}/tflite_runtime-2.5.0.dist-info/METADATA;md5=8c4b9e107a64b0121980a8705094014b"

PYTHON_PACKAGE = "tflite_runtime-2.5.0-cp38-cp38-linux_aarch64.whl"

SRC_URI = "https://github.com/google-coral/pycoral/releases/download/v1.0.1/${PYTHON_PACKAGE};downloadfilename=${PYTHON_PACKAGE};subdir=${BP}"
SRC_URI[md5sum] = "9c47617e1fa0bdca673a78b8b6688d64"
SRC_URI[sha256sum] = "b87a4c152be05d3585521a1d5418f7645a4fb82965772489b983e93aae6bd9ac"

DEPENDS = "python3 python3-pip-native python3-wheel-native"

RDEPENDS_${PN} = "${PYTHON_PN} \
                  ${PYTHON_PN}-numpy \
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
        ${WORKDIR}/${BP}/tflite_runtime-*.whl
}

FILES_${PN} += "\
    ${libdir}/${PYTHON_DIR}/site-packages/* \
"

INSANE_SKIP_${PN} += "already-stripped"

COMPATIBLE_MACHINE = "(-)"
COMPATIBLE_MACHINE_aarch64 = "(.*)"
