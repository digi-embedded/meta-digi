# Copyright (C) 2023-2024, Digi International Inc.

FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

PV = "1.14.0+git${SRCPV}"

SRCREV = "6ccaeddefa65ccac402a47fa4d9cad8229794bb2"
SRC_URI = "gitsm://github.com/microsoft/onnxruntime.git;branch=rel-1.14.0;protocol=https"
SRC_URI += " file://0001-onnxruntime-test-remove-AVX-specific-micro-benchmark.patch "
SRC_URI += " file://0002-onnxruntime-add-SONAME-with-MAJOR-version.patch "
SRC_URI += " file://0003-onnxruntime-test-libcustom-library-remove-relative.patch "
SRC_URI += " file://0004-onnxruntime-fix-imcompatibility-with-compiler-GCC12.patch "
SRC_URI += " file://0005-onnxruntime-avoid-using-unsupported-Eigen-headers.patch "
SRC_URI += " file://0007-onnxruntime-cmake-change-visibility-compilation-opti.patch "
SRC_URI:append:stm32mp2common = " file://0006-onnxruntime-xnnpack-Fix-mcpu-compiler-build-failure.patch "

PROTOC_VERSION = "3.20.2"
SRC_URI += "https://github.com/protocolbuffers/protobuf/releases/download/v${PROTOC_VERSION}/protoc-${PROTOC_VERSION}-linux-x86_64.zip;name=protoc;subdir=protoc-${PROTOC_VERSION}/"
SRC_URI[protoc.sha256sum] = "d97227fd8bc840dcb1cf7332c8339a2d8f0fc381a98b028006e5c9a911d07c2a"

S = "${WORKDIR}/git"

inherit python3-dir cmake

DEPENDS:append = "\
	${PYTHON_PN}-numpy \
"

EXTRA_OECMAKE += " -DPython_NumPy_INCLUDE_DIR="${STAGING_LIBDIR}/${PYTHON_DIR}/site-packages/numpy/core/include" \
		      -DBENCHMARK_ENABLE_GTEST_TESTS=OFF \
		      -Donnxruntime_USE_XNNPACK=ON \
		      -Donnxruntime_BUILD_UNIT_TESTS=ON \
"

ONNX_TARGET_ARCH:aarch64="${@bb.utils.contains('TUNE_FEATURES', 'cortexa35', 'aarch64', '', d)}"

do_install() {

	# Install onnxruntime dynamic library
	install -d ${D}${libdir}
	install -d ${D}${prefix}/local/bin/${PN}-${PVB}/tools
	install -d ${D}${prefix}/local/bin/${PN}-${PVB}/unit-tests

	install -m 0644 ${B}/libonnxruntime.so				 ${D}${libdir}/libonnxruntime.so.${PVB}

	# This shared lib is used by onnxruntime_shared_lib_test and onnxruntime_test_python.py
	install -m 644 ${B}/libcustom_op_library.so			 ${D}${libdir}

	# And this one only by onnxruntime_test_python.py
	install -m 644 ${B}/libtest_execution_provider.so		${D}${libdir}
	install -m 644 ${B}/libonnxruntime_providers_shared.so	${D}${libdir}/libonnxruntime_providers_shared.so
	install -m 644 ${B}/libcustom_op_invalid_library.so		${D}${libdir}/libcustom_op_invalid_library.so
	install -m 644 ${B}/onnxruntime_pybind11_state.so		${D}${libdir}/onnxruntime_pybind11_state.so

	# Install the symlinks.
	ln -sf libonnxruntime.so.${PVB} ${D}${libdir}/libonnxruntime.so.${MAJOR}
	ln -sf libonnxruntime.so.${PVB} ${D}${libdir}/libonnxruntime.so

	# These are not included in the base installation, so we install them manually.
	install -m 755 ${B}/onnx_test_runner						${D}${prefix}/local/bin/${PN}-${PVB}/unit-tests
	install -m 755 ${B}/onnxruntime_perf_test					${D}${prefix}/local/bin/${PN}-${PVB}/tools
	install -m 755 ${B}/onnxruntime_test_all					${D}${prefix}/local/bin/${PN}-${PVB}/unit-tests
	install -m 755 ${B}/onnxruntime_shared_lib_test				${D}${prefix}/local/bin/${PN}-${PVB}/unit-tests
	install -m 755 ${B}/onnxruntime_api_tests_without_env		${D}${prefix}/local/bin/${PN}-${PVB}/unit-tests
	install -m 755 ${B}/onnxruntime_global_thread_pools_test	${D}${prefix}/local/bin/${PN}-${PVB}/unit-tests
	install -m 755 ${B}/onnxruntime_test_python.py				${D}${prefix}/local/bin/${PN}-${PVB}/unit-tests
	install -m 755 ${B}/helper.py								${D}${prefix}/local/bin/${PN}-${PVB}/unit-tests
	cp -r ${B}/testdata											${D}${prefix}/local/bin/${PN}-${PVB}/unit-tests

	# We have to change some of the RPATH as well.
	chrpath -r '$ORIGIN' ${D}${prefix}/local/bin/${PN}-${PVB}/tools/onnxruntime_perf_test
	chrpath -r '$ORIGIN' ${D}${prefix}/local/bin/${PN}-${PVB}/unit-tests/onnxruntime_shared_lib_test
	chrpath -r '$ORIGIN' ${D}${prefix}/local/bin/${PN}-${PVB}/unit-tests/onnxruntime_api_tests_without_env
	chrpath -r '$ORIGIN' ${D}${prefix}/local/bin/${PN}-${PVB}/unit-tests/onnxruntime_global_thread_pools_test
	chrpath -r '$ORIGIN' ${D}${libdir}/libtest_execution_provider.so

	# Install the Python package.
	mkdir -p ${D}${PYTHON_SITEPACKAGES_DIR}/onnxruntime
	cp -r    ${B}/onnxruntime ${D}${PYTHON_SITEPACKAGES_DIR}

	# Install header files
	install -d ${D}${includedir}/onnxruntime
	cd ${S}/onnxruntime
	cp --parents $(find . -name "*.h*" -not -path "*cmake_build/*") 	${D}${includedir}/onnxruntime
	cp  ${S}/include/onnxruntime/core/session/onnxruntime_cxx_api.h  	${D}${includedir}/onnxruntime
	cp  ${S}/include/onnxruntime/core/session/onnxruntime_c_api.h  		${D}${includedir}/onnxruntime
	cp  ${S}/include/onnxruntime/core/session/onnxruntime_cxx_inline.h  ${D}${includedir}/onnxruntime
}

# The package_qa() task does not like the fact that this library is present in both onnxruntime-tools
# and python3-onnxruntime packages (the normal /usr/lib version and a copy placed inside the Python package).
# So we simply mark the lib as a "private lib", to prevent the task from outputting an error.
PRIVATE_LIBS = "libonnxruntime_providers_shared.so"

PACKAGES += "${PN}-unit-tests"
PROVIDES += "${PN}-unit-tests"

FILES:${PN}-tools = "${prefix}/local/bin/${PN}-${PVB}/tools/onnxruntime_perf_test"
FILES:${PN}-unit-tests = "${prefix}/local/bin/${PN}-${PVB}/unit-tests/* ${libdir}/libcustom_op_invalid_library.so ${libdir}/libtest_execution_provider.so ${libdir}/libcustom_op_library.so"

# onnxruntime_test_python.py unitary test requires python3-numpy and python3-onnxruntime packages
RDEPENDS:${PN}-unit-tests += "${PYTHON_PN}-${PN}"
RDEPENDS:${PYTHON_PN}-${PN} += "${PYTHON_PN}-numpy"
