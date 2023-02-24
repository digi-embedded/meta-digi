SUMMARY = "wolfSSL Lightweight Embedded SSL/TLS Library"
DESCRIPTION = "wolfSSL is a lightweight SSL/TLS library written in C and \
               optimized for embedded and RTOS environments. It can be up \
               to 20 times smaller than OpenSSL while still supporting \
               a full TLS client and server, up to TLS 1.3"
HOMEPAGE = "https://www.wolfssl.com/products/wolfssl"
BUGTRACKER = "https://github.com/wolfssl/wolfssl/issues"
SECTION = "libs"
LICENSE = "WolfSSL-Commercial"
LICENSE_FLAGS = "commercial"
LIC_FILES_CHKSUM = "file://WolfSSL_LicenseAgmt_JAN-2022.pdf;md5=be28609dc681e98236c52428fadf04dd"
NO_GENERIC_LICENSE[WolfSSL-Commercial] = "WolfSSL_LicenseAgmt_JAN-2022.pdf"

PROVIDES += "cyassl"
RPROVIDES:${PN} = "cyassl"
PROVIDES += "wolfssl"
RPROVIDES:${PN} = "wolfssl"

# To be configured in project's config file
WOLFSSL_FIPS_PKG_NAME ?= "wolfssl-5.4.0-commercial-fips-linuxv5"
WOLFSSL_FIPS_PKG_PASSWORD ?= ""
WOLFSSL_FIPS_PKG_PATH ?= ""

python() {
    # The package is not publicly available, so provide a PREMIRROR to a local directory
    # that can be configured in the project's local.conf file using WOLFSSL_FIPS_PKG_PATH
    # variable.
    wolfssl_fips_local_path = d.getVar('WOLFSSL_FIPS_PKG_PATH')
    if wolfssl_fips_local_path:
        premirrors = d.getVar('PREMIRRORS')
        d.setVar('PREMIRRORS', "http:///not/exist/${WOLFSSL_FIPS_PKG_NAME}.7z file://%s \\n %s" % (wolfssl_fips_local_path, premirrors))

    # Yocto does not support unpacking password protected packages, so configure the
    # SRC_URI as unpack=false in that case.
    d.setVar('WOLFSSL_FIPS_PKG_UNPACK', str(not d.getVar('WOLFSSL_FIPS_PKG_PASSWORD')))

    # Aux variable to prevent running 7za archiver on a not-7z package
    d.setVar('WOLFSSL_FIPS_PKG_IS_7Z', str(d.getVar('WOLFSSL_FIPS_PKG_PATH').endswith('.7z')))

    # FIPS core integrity hash needs to be added back to build process
    wolfssl_fips_core_hash = d.getVar('WOLFSSL_FIPS_CORE_HASH')
    if wolfssl_fips_core_hash:
        d.setVar('CFLAGS:append', " -DWOLFCRYPT_FIPS_CORE_HASH_VALUE=%s" % wolfssl_fips_core_hash)
}

SRC_URI = "http:///not/exist/${WOLFSSL_FIPS_PKG_NAME}.7z;unpack=${WOLFSSL_FIPS_PKG_UNPACK}"
SRC_URI[sha256sum] = "0743e481e9e3ec2b7ba531c5821c44d55b313c0af04ded148caf4db7e0baa582"

S = "${WORKDIR}/${WOLFSSL_FIPS_PKG_NAME}"

inherit autotools

do_unpack[depends] += "p7zip-native:do_populate_sysroot"
do_unpack[postfuncs] += "${@oe.utils.vartrue('WOLFSSL_FIPS_PKG_UNPACK', '', 'unpack_7z_password_pkg', d)}"
unpack_7z_password_pkg() {
	if [ "${WOLFSSL_FIPS_PKG_IS_7Z}" = "True" ]; then
		7za x -o${WORKDIR} -p${WOLFSSL_FIPS_PKG_PASSWORD} -y ${WORKDIR}/${WOLFSSL_FIPS_PKG_NAME}.7z 1>/dev/null
	fi
}

# Enable FIPS support, the compatibility layer and some other useful options
EXTRA_OECONF += " \
    --enable-fips=v5 \
    --enable-opensslextra \
    --enable-postauth \
    --enable-sha3 \
    --enable-tls13 \
    --enable-tlsx \
"

BBCLASSEXTEND += "native nativesdk"

DEFAULT_PREFERENCE = "-1"
