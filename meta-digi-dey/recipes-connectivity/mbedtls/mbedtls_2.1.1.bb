# Copyright (C) 2017 Digi International.

SUMMARY = "An open source, portable, easy to use, readable and flexible SSL \
library"
DESCRIPTION = "mbedtls is a lean open source crypto library          \
for providing SSL and TLS support in your programs. It offers        \
an intuitive API and documented header files, so you can actually    \
understand what the code does. It features:                          \
                                                                     \
 - Symmetric algorithms, like AES, Blowfish, Triple-DES, DES, ARC4,  \
   Camellia and XTEA                                                 \
 - Hash algorithms, like SHA-1, SHA-2, RIPEMD-160 and MD5            \
 - Entropy pool and random generators, like CTR-DRBG and HMAC-DRBG   \
 - Public key algorithms, like RSA, Elliptic Curves, Diffie-Hellman, \
   ECDSA and ECDH                                                    \
 - SSL v3 and TLS 1.0, 1.1 and 1.2                                   \
 - Abstraction layers for ciphers, hashes, public key operations,    \
   platform abstraction and threading                                \
"

HOMEPAGE = "https://tls.mbed.org/"
SECTION = "libdevel"
BUGTRACKER = "https://github.com/ARMmbed/mbedtls/issues"
LICENSE = "Apache-2.0"
LIC_FILES_CHKSUM = "file://LICENSE;md5=302d50a6369f5f22efdb674db908167a"

SRC_URI = " \
    https://github.com/ARMmbed/mbedtls/archive/${PN}-${PV}.tar.gz \
    file://0001-mbedtls-library-add-pkg-config-file.patch \
"
SRC_URI[md5sum] = "6f5d3e7154ce4e04bcb9b299f614775f"
SRC_URI[sha256sum] = "ae458a4987f36819bdf1d39519212f4063780fe448d4155878fccf4e782a715f"

S = "${WORKDIR}/${PN}-${PN}-${PV}"

inherit cmake

EXTRA_OECMAKE = " \
    -DENABLE_PROGRAMS:BOOL=OFF \
    -DENABLE_TESTING:BOOL=OFF \
    -DUSE_STATIC_MBEDTLS_LIBRARY:BOOL=ON \
    -DUSE_SHARED_MBEDTLS_LIBRARY:BOOL=ON \
    -DLIB_INSTALL_DIR:STRING=${libdir} \
    -DCMAKE_INSTALL_PREFIX:PATH=${prefix} \
"

ALLOW_EMPTY_${PN} = "1"
