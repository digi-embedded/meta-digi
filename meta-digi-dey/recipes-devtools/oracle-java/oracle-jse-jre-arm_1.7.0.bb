# Copyright (C) 2013 Digi International.

require oracle-jse.inc
JDK_JRE = "ejre"

# Embedded JRE does not contain a LICENSE file
LIC_FILES_CHKSUM = "\
        file://${WORKDIR}/${JDK_JRE}${PV}_${PV_UPDATE}/COPYRIGHT;md5=3a11238025bf13b87f04753183ffeb90 \
        file://${WORKDIR}/${JDK_JRE}${PV}_${PV_UPDATE}/THIRDPARTYLICENSEREADME.txt;md5=c339b34e3da6673d2c5950d0f8808f8c \
        "

PR = "r0"
PV_UPDATE = "21"

ORACLE_URL = "http://download.oracle.com/otn/java/ejre/7u21-b11/"

JRE_ARMV5_headless = "${ORACLE_URL}/ejre-7u21-fcs-b11-linux-arm-sflt-headless-04_apr_2013.tar.gz"
JRE_ARMv7_headless = "${ORACLE_URL}/ejre-7u21-fcs-b11-linux-arm-vfp-server_headless-04_apr_2013.tar.gz"
JRE_ARMv7_headfull = "${ORACLE_URL}/ejre-7u21-fcs-b11-linux-arm-vfp-client_headful-04_apr_2013.tar.gz"

SRC_URI_armv7a = "${@base_contains('DISTRO_FEATURES', 'x11', '${JRE_ARMv7_headfull};name=armv7_headfull', '${JRE_ARMv7_headless};name=armv7_headless', d)}"

SRC_URI[armv7_headless.md5sum] = "edd6661debdcccd9e5e8af85d6bd30f1"
SRC_URI[armv7_headless.sha256sum] = "4e7e5d5eb8a192d67cd56875d31c7f1513b12193328bd41e11be347d89271d64"
SRC_URI[armv7_headfull.md5sum] = "eac89bfdfb5ecf3ee804cf0e9c6bd7e7"
SRC_URI[armv7_headfull.sha256sum] = "544e9a3189b9b420af7c92da1976d40fe7f90ed5b9c4195b59988b9982c8733f"

SRC_URI_armv5 = "${JRE_ARMV5_headless};name=armv5"
SRC_URI[armv5.md5sum] = "edd6661debdcccd9e5e8af85d6bd30f1"
SRC_URI[armv5.sha256sum] = "4e7e5d5eb8a192d67cd56875d31c7f1513b12193328bd41e11be347d89271d64"
