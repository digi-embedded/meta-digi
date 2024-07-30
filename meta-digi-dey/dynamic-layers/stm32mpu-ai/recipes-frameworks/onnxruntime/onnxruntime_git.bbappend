# Copyright (C) 2023, Digi International Inc.

do_install() {

	# Install onnxruntime dynamic library
	install -d ${D}${libdir}
	install -d ${D}${prefix}/local/bin/${PN}-${PVB}/tools
	install -m 0644 ${B}/libonnxruntime.so				 ${D}${libdir}/libonnxruntime.so.${PVB}

	# This shared lib is used by onnxruntime_shared_lib_test and onnxruntime_test_python.py
	install -m 644 ${B}/libcustom_op_library.so			 ${D}${libdir}
	# And this one only by onnxruntime_test_python.py
	install -m 644 ${B}/libtest_execution_provider.so		 ${D}${libdir}
	install -m 644 ${B}/libonnxruntime_providers_shared.so		 ${D}${libdir}/libonnxruntime_providers_shared.so
	install -m 644 ${B}/onnxruntime_pybind11_state.so		 ${D}${libdir}/onnxruntime_pybind11_state.so

	# Install the symlinks.
	ln -sf libonnxruntime.so.${PVB} ${D}${libdir}/libonnxruntime.so.${MAJOR}
	ln -sf libonnxruntime.so.${PVB} ${D}${libdir}/libonnxruntime.so

	# Digi: copy instead of moving to avoid QA errors
	# Copy the onnx_test_runner executable that was installed in /usr instead of /usr/local.
	cp ${B}/onnx_test_runner ${D}${prefix}/local/bin/${PN}-${PVB}/tools

	# These are not included in the base installation, so we install them manually.
	install -m 755 ${B}/onnxruntime_perf_test			${D}${prefix}/local/bin/${PN}-${PVB}/tools
	install -m 755 ${B}/onnxruntime_test_all			${D}${prefix}/local/bin/${PN}-${PVB}/tools
	install -m 755 ${B}/onnxruntime_shared_lib_test			${D}${prefix}/local/bin/${PN}-${PVB}/tools
	install -m 755 ${B}/onnxruntime_api_tests_without_env		${D}${prefix}/local/bin/${PN}-${PVB}/tools
	install -m 755 ${B}/onnxruntime_global_thread_pools_test	${D}${prefix}/local/bin/${PN}-${PVB}/tools
	install -m 755 ${B}/onnxruntime_test_python.py			${D}${prefix}/local/bin/${PN}-${PVB}/tools
	install -m 755 ${B}/helper.py					${D}${prefix}/local/bin/${PN}-${PVB}/tools
	cp -r ${B}/testdata						${D}${prefix}/local/bin/${PN}-${PVB}/tools

	# We have to change some of the RPATH as well.
	chrpath -r '$ORIGIN' ${D}${prefix}/local/bin/${PN}-${PVB}/tools/onnxruntime_perf_test
	chrpath -r '$ORIGIN' ${D}${prefix}/local/bin/${PN}-${PVB}/tools/onnxruntime_shared_lib_test
	chrpath -r '$ORIGIN' ${D}${prefix}/local/bin/${PN}-${PVB}/tools/onnxruntime_api_tests_without_env
	chrpath -r '$ORIGIN' ${D}${prefix}/local/bin/${PN}-${PVB}/tools/onnxruntime_global_thread_pools_test
	chrpath -r '$ORIGIN' ${D}${libdir}/libtest_execution_provider.so

	# Install the Python package.
	mkdir -p ${D}${PYTHON_SITEPACKAGES_DIR}/onnxruntime
	cp -r    ${B}/onnxruntime ${D}${PYTHON_SITEPACKAGES_DIR}
}
