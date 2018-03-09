# Copyright (C) 2017 Digi International.

SUMMARY = "AWS IoT device SDK Demo"
DESCRIPTION = "Demo application for AWS IoT device SDK"
SECTION = "base"
LICENSE = "ISC"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/ISC;md5=f3b90e78ea0cffb20bf5cca7947a896d"

DEPENDS = "awsiotsdk-c confuse"

AWS_USER_LED ?= ""
AWS_USER_LED_ccimx6ulstarter ?= "75"
AWS_USER_LED_ccimx6ulsbc ?= "488"
AWS_USER_LED_ccimx6sbc ?= "34"
AWS_USER_LED_ccimx6qpsbc ?= "34"

SRCBRANCH = "dey-2.4/maint"
SRCREV = "a2a5d119b94a6113655df026cfc4e4b41f29c008"

CC_STASH = "${DIGI_MTK_GIT}dey/dey-examples.git;protocol=ssh"
CC_GITHUB = "${DIGI_GITHUB_GIT}/dey-examples.git;protocol=git"

CC_GIT_URI ?= "${@base_conditional('DIGI_INTERNAL_GIT', '1' , '${CC_STASH}', '${CC_GITHUB}', d)}"

SRC_URI = "${CC_GIT_URI};nobranch=1"

S = "${WORKDIR}/git/awsiot-sample"

inherit aws-iot pkgconfig

EXTRA_OEMAKE += "'LOG_FLAGS=${@get_log_level(d)}'"

do_configure() {
	# Update the configuration header file.
	sed -i -e "s,\(thing_name = \)\"\",\1\"${AWS_IOT_MY_THING_NAME}\",g" \
	       -e "s,\(host = \)\"\",\1\"${AWS_IOT_MQTT_HOST}\",g" \
	       -e "s,8883,${AWS_IOT_MQTT_PORT},g" \
	       -e "s,\([\"']\)rootCA.crt\([\"']\),\1${AWS_IOT_ROOT_CA_FILENAME}\2,g" \
	       -e "s,\([\"']\)cert.pem\([\"']\),\1${AWS_IOT_CERTIFICATE_FILENAME}\2,g" \
	       -e "s,\([\"']\)privkey.pem\([\"']\),\1${AWS_IOT_PRIVATE_KEY_FILENAME}\2,g" \
	       -e "s,\(user_led = \)-1,\1${AWS_USER_LED},g" \
	       "${S}/cfg_files/awsiotsdk.conf"
}

do_install() {
	oe_runmake DESTDIR=${D} install
}

RRECOMMENDS_${PN} += "awsiotsdk-c-cert"

PACKAGE_ARCH = "${MACHINE_ARCH}"

