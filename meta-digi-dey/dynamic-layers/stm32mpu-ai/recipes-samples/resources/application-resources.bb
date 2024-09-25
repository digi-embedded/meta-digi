# Copyright (C) 2019, STMicroelectronics - All Rights Reserved
SUMMARY = "Create package containing all resources need by out of the box applications"
LICENSE = "Apache-2.0"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/Apache-2.0;md5=89aea4e17d99a7cacdbeed46a0096b10"

SRC_URI = "	file://resources-files "

S = "${WORKDIR}"

do_configure[noexec] = "1"
do_compile[noexec] = "1"

do_install() {
	install -d ${D}${prefix}/local/demo-ai/resources/

	# install all tflite resources
	install -m 0644 ${S}/resources-files/tfl_*.png 					${D}${prefix}/local/demo-ai/resources
	# install all onnx resources
	install -m 0644 ${S}/resources-files/onnx_*.png 				${D}${prefix}/local/demo-ai/resources
	# install all coral resources
	install -m 0644 ${S}/resources-files/coral_*.png 				${D}${prefix}/local/demo-ai/resources
	# install all configuration scripts
	install -m 0755 ${S}/resources-files/config_board.sh 			${D}${prefix}/local/demo-ai/resources
	install -m 0755 ${S}/resources-files/check_camera_preview.sh 	${D}${prefix}/local/demo-ai/resources
	install -m 0755 ${S}/resources-files/setup_camera.sh 			${D}${prefix}/local/demo-ai/resources
	install -m 0644 ${S}/resources-files/Default.css 				${D}${prefix}/local/demo-ai/resources
	# install all common resources
	install -m 0644 ${S}/resources-files/label_*.png 				${D}${prefix}/local/demo-ai/resources
	install -m 0644 ${S}/resources-files/exit_*.png 				${D}${prefix}/local/demo-ai/resources

}

do_install:append:stm32mp25common(){

	# install all nbg resources
	install -m 0644 ${S}/resources-files/nbg_*.png 					${D}${prefix}/local/demo-ai/resources

	# overwrite camera setup script for stm32mp2x
    install -m 0755 ${S}/resources-files/check_camera_preview_main_isp.sh     ${D}${prefix}/local/demo-ai/resources/check_camera_preview.sh
    install -m 0755 ${S}/resources-files/setup_camera_main_isp.sh             ${D}${prefix}/local/demo-ai/resources/setup_camera.sh
}

FILES:${PN} += "${prefix}/local/"

RDEPENDS:${PN} += " bash "
