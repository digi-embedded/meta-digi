# Copyright (C) 2017, Digi International Inc.

SUMMARY = "SDK for connecting to AWS IoT from a device using embedded C"
HOMEPAGE = "https://github.com/aws/aws-iot-device-sdk-embedded-C"
SECTION = "base"
LICENSE = "Apache-2.0"
LIC_FILES_CHKSUM = "file://LICENSE.txt;md5=acc7a1bf87c055789657b148939e4b40"

DEPENDS = "mbedtls"

SRC_URI = " \
    git://github.com/aws/aws-iot-device-sdk-embedded-C.git;protocol=https;branch=main \
    file://aws_iot_config.h.template \
    file://awsiotsdk.pc \
    file://Makefile \
    file://Makefile.lib \
"

# Tag 'v2.1.1'
SRCREV = "70071112bd5e1c5b9f150894fafe199637b4f63a"

S = "${WORKDIR}/git"

inherit aws-iot pkgconfig

EXTRA_OEMAKE += "'LOG_FLAGS=${@get_log_level(d)}'"

do_configure() {
	cp -f ${WORKDIR}/awsiotsdk.pc ${S}

	# Copy and update the configuration header file.
	cp -f ${WORKDIR}/aws_iot_config.h.template ${S}/include/aws_iot_config.h
	sed -i -e "s,##AWS_IOT_MQTT_HOST##,${AWS_IOT_MQTT_HOST},g" "${S}/include/aws_iot_config.h"
	sed -i -e "s,##AWS_IOT_MQTT_PORT##,${AWS_IOT_MQTT_PORT},g" "${S}/include/aws_iot_config.h"
	sed -i -e "s,##AWS_IOT_MY_THING_NAME##,${AWS_IOT_MY_THING_NAME},g" "${S}/include/aws_iot_config.h"
	sed -i -e "s,##AWS_IOT_ROOT_CA_FILENAME##,${AWS_IOT_ROOT_CA_FILENAME},g" "${S}/include/aws_iot_config.h"
	sed -i -e "s,##AWS_IOT_CERTIFICATE_FILENAME##,${AWS_IOT_CERTIFICATE_FILENAME},g" "${S}/include/aws_iot_config.h"
	sed -i -e "s,##AWS_IOT_PRIVATE_KEY_FILENAME##,${AWS_IOT_PRIVATE_KEY_FILENAME},g" "${S}/include/aws_iot_config.h"
	sed -i -e "s,##AWS_IOT_MQTT_TX_BUF_LEN##,${AWS_IOT_MQTT_TX_BUF_LEN},g" "${S}/include/aws_iot_config.h"
	sed -i -e "s,##AWS_IOT_MQTT_RX_BUF_LEN##,${AWS_IOT_MQTT_RX_BUF_LEN},g" "${S}/include/aws_iot_config.h"
	sed -i -e "s,##AWS_IOT_MQTT_NUM_SUBSCRIBE_HANDLERS##,${AWS_IOT_MQTT_NUM_SUBSCRIBE_HANDLERS},g" "${S}/include/aws_iot_config.h"
	sed -i -e "s,##MAX_ACKS_TO_COMEIN_AT_ANY_GIVEN_TIME##,${MAX_ACKS_TO_COMEIN_AT_ANY_GIVEN_TIME},g" "${S}/include/aws_iot_config.h"
	sed -i -e "s,##MAX_THINGNAME_HANDLED_AT_ANY_GIVEN_TIME##,${MAX_THINGNAME_HANDLED_AT_ANY_GIVEN_TIME},g" "${S}/include/aws_iot_config.h"
	sed -i -e "s,##MAX_JSON_TOKEN_EXPECTED##,${MAX_JSON_TOKEN_EXPECTED},g" "${S}/include/aws_iot_config.h"
	sed -i -e "s,##MAX_SIZE_OF_THING_NAME##,${MAX_SIZE_OF_THING_NAME},g" "${S}/include/aws_iot_config.h"
	sed -i -e "s,##AWS_IOT_MQTT_MIN_RECONNECT_WAIT_INTERVAL##,${AWS_IOT_MQTT_MIN_RECONNECT_WAIT_INTERVAL},g" "${S}/include/aws_iot_config.h"
	sed -i -e "s,##AWS_IOT_MQTT_MAX_RECONNECT_WAIT_INTERVAL##,${AWS_IOT_MQTT_MAX_RECONNECT_WAIT_INTERVAL},g" "${S}/include/aws_iot_config.h"

	# Copy the Makefiles.
	cp -f ${WORKDIR}/Makefile ${S}
	cp -f ${WORKDIR}/Makefile.lib ${S}/src/Makefile
}

do_install() {
	oe_runmake DESTDIR=${D} install

	# Install certificates only if they exist.
	if [ -f "${AWS_IOT_CERTS_DIR}/${AWS_IOT_ROOT_CA_FILENAME}" ] && \
	   [ -f "${AWS_IOT_CERTS_DIR}/${AWS_IOT_CERTIFICATE_FILENAME}" ] && \
	   [ -f "${AWS_IOT_CERTS_DIR}/${AWS_IOT_PRIVATE_KEY_FILENAME}" ]; then
		install -d ${D}${sysconfdir}/ssl/certs
		install -m 0644 "${AWS_IOT_CERTS_DIR}/${AWS_IOT_ROOT_CA_FILENAME}" ${D}${sysconfdir}/ssl/certs/
		install -m 0644 "${AWS_IOT_CERTS_DIR}/${AWS_IOT_CERTIFICATE_FILENAME}" ${D}${sysconfdir}/ssl/certs/
		install -m 0644 "${AWS_IOT_CERTS_DIR}/${AWS_IOT_PRIVATE_KEY_FILENAME}" ${D}${sysconfdir}/ssl/certs/
	fi
}

PACKAGES =+ "${PN}-cert"

FILES:${PN}-cert = "${sysconfdir}/ssl/certs/"

ALLOW_EMPTY:${PN} = "1"

