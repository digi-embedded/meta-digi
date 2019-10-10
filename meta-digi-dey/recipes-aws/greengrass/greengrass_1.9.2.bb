# Copyright (C) 2019, Digi International Inc.

require greengrass.inc

#
# The Amazon Greengrass Core Product includes the following third-party software/licensing:
# github.com/aws/aws-sdk-go/; version 1.15.65 -- https://github.com/aws/aws-sdk-go/
# github.com/coreos/go-systemd/; version 10 -- https://github.com/coreos/go-systemd/
# github.com/docker/docker; version 1.12.0-rc4 -- https://github.com/docker/docker
# github.com/docker/go-units; version 0.3.1 -- https://github.com/docker/go-units
# github.com/go-ini/ini; version 1.32.0 -- https://github.com/go-ini/ini
# github.com/jmespath/go-jmespath; version 0.2.2 -- https://github.com/jmespath/go-jmespath
# github.com/mwitkow/go-http-dialer; version 0.1 -- https://github.com/mwitkow/go-http-dialer
# github.com/opencontainers/runc; version 1.0.0-rc3 -- https://github.com/opencontainers/runc
# github.com/opencontainers/runtime-spec; version 1.0.0-rc5 -- https://github.com/opencontainers/runtime-spec
# github.com/pquerna/ffjson; version 1.0 -- https://github.com/pquerna/ffjson
# github.com/vishvananda/netlink; version 0.1 -- https://github.com/vishvananda/netlink
#
# And the following Licenses:
LIC_FILES_CHKSUM = " \
    file://ggc/core/THIRD-PARTY-LICENSES;md5=53b6a4caa097863bc3971d5e0ac6d1db \
"

SRC_URI[arm.md5sum] = "63a1f6aae22260be19f34f278f7e7833"
SRC_URI[arm.sha256sum] ="4bc0bc8a938cdb3d846df92e502155c6ec8cbaf1b63dfa9f3cc3a51372d95af5"

# For ARCH64 we use another tarball.
SRC_URI[aarch64.md5sum] = "967cd25ac77e733b0a1b42d83dc96ed1"
SRC_URI[aarch64.sha256sum] ="4cbaf91e5354fe49ded160415394413f068426c2bba13089e6b8a28775990a9c"
