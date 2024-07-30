# Copyright (C) 2017, Digi International Inc.

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
    git://github.com/Mbed-TLS/mbedtls.git;protocol=https;branch=master \
    file://0001-mbedtls-library-add-pkg-config-file.patch \
"

# Tag 'mbedtls-2.1.1'
SRCREV = "8cea8ad8b825b0bf5884054af7499f1d5c3ebeb4"

S = "${WORKDIR}/git"

inherit cmake

EXTRA_OECMAKE = " \
    -DENABLE_PROGRAMS:BOOL=OFF \
    -DENABLE_TESTING:BOOL=OFF \
    -DUSE_STATIC_MBEDTLS_LIBRARY:BOOL=ON \
    -DUSE_SHARED_MBEDTLS_LIBRARY:BOOL=ON \
    -DLIB_INSTALL_DIR:STRING=${libdir} \
    -DCMAKE_INSTALL_PREFIX:PATH=${prefix} \
"

ALLOW_EMPTY:${PN} = "1"
