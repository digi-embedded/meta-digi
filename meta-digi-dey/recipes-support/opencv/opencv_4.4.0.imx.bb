require opencv_4.4.0.bb

LIC_FILES_CHKSUM = "file://LICENSE;md5=19598330421859a6dd353a4318091ac7"

SRCREV_opencv = "e39e6eded2d365a5dc370e1a72717e132166cf07"
SRCREV_contrib = "5fae4082cc493efa5cb7a7486f9e009618a5198b"
SRCREV_extra = "65796edadce27ed013e3deeedb3c081ff527e4ec"
SRC_URI[tinydnn.md5sum] = "adb1c512e09ca2c7a6faef36f9c53e59"
SRC_URI[tinydnn.sha256sum] = "e2c61ce8c5debaa644121179e9dbdcf83f497f39de853f8dd5175846505aa18b"
SRCREV_FORMAT_append = "_extra"

SRC_URI_remove = " \
    git://github.com/opencv/opencv.git;name=opencv \
    file://0002-Make-opencv-ts-create-share-library-intead-of-static.patch \
"
OPENCV_SRC ?= "git://github.com/nxp-imx/opencv-imx.git;protocol=https"
SRCBRANCH = "4.4.0_imx"
SRC_URI =+ "${OPENCV_SRC};branch=${SRCBRANCH};name=opencv"
SRC_URI += " \
    git://github.com/opencv/opencv_extra.git;destsuffix=extra;name=extra \
    https://github.com/tiny-dnn/tiny-dnn/archive/v1.0.0a3.tar.gz;destsuffix=git/3rdparty/tinydnn/tiny-dnn-1.0.0a3;name=tinydnn;unpack=false \
    file://OpenCV_DNN_examples.patch \
    file://0001-Add-smaller-version-of-download_models.py.patch;patchdir=../extra \
"
PV = "4.4.0.imx"

PACKAGECONFIG_remove        = "eigen"
PACKAGECONFIG_append_mx8    = " dnn text"
PACKAGECONFIG_OPENCL        = ""
PACKAGECONFIG_OPENCL_mx8    = "opencl"
PACKAGECONFIG_OPENCL_mx8dxl = ""
PACKAGECONFIG_OPENCL_mx8phantomdxl = ""
PACKAGECONFIG_OPENCL_mx8mm  = ""
PACKAGECONFIG_OPENCL_mx8mnlite  = ""
PACKAGECONFIG_append        = " ${PACKAGECONFIG_OPENCL}"

PACKAGECONFIG[openvx] = "-DWITH_OPENVX=ON -DOPENVX_ROOT=${STAGING_LIBDIR} -DOPENVX_LIB_CANDIDATES='OpenVX;OpenVXU',-DWITH_OPENVX=OFF,virtual/libopenvx,"
PACKAGECONFIG[qt5] = "-DWITH_QT=ON -DOE_QMAKE_PATH_EXTERNAL_HOST_BINS=${STAGING_BINDIR_NATIVE} -DCMAKE_PREFIX_PATH=${STAGING_BINDIR_NATIVE}/cmake,-DWITH_QT=OFF,qtbase qtbase-native,"
PACKAGECONFIG[test] = "-DBUILD_TESTS=ON -DINSTALL_TESTS=ON -DOPENCV_TEST_DATA_PATH=${S}/../extra/testdata, -DBUILD_TESTS=OFF -DINSTALL_TESTS=OFF,"

do_unpack_extra_append() {
    mkdir -p ${S}/3rdparty/tinydnn/
    tar xzf ${WORKDIR}/v1.0.0a3.tar.gz -C ${S}/3rdparty/tinydnn/
}

do_install_append() {
    ln -sf opencv4/opencv2 ${D}${includedir}/opencv2
    install -d ${D}${datadir}/OpenCV/samples/data
    cp -r ${S}/samples/data/* ${D}${datadir}/OpenCV/samples/data
    install -d ${D}${datadir}/OpenCV/samples/bin/
    cp -f bin/example_* ${D}${datadir}/OpenCV/samples/bin/
    if ${@bb.utils.contains('PACKAGECONFIG', 'test', 'true', 'false', d)}; then
        cp -r share/opencv4/testdata/cv/face/* ${D}${datadir}/opencv4/testdata/cv/face/
    fi
}

FILES_${PN}-samples += "${datadir}/OpenCV/samples"
