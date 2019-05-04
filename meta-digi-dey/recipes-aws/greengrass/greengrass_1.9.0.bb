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

SRC_URI[arm.md5sum] = "57e408134eccbbda40f08dbbf52101c2"
SRC_URI[arm.sha256sum] ="ded5d88a3ec1479d79c842b16fef11f91ee331bd4b79dbba1ca639b3e51922a3"

# For ARCH64 we use another tarball.
SRC_URI[aarch64.md5sum] = "c8e5488e302905583829f95d55d7a912"
SRC_URI[aarch64.sha256sum] ="9cd00902090e8fc34de18bf1ff21dca5e90af12ced886e6ac46e1f6899b059e1"
