# Copyright (C) 2018, 2019, Digi International Inc.

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
    file://ggc/core/THIRD-PARTY-LICENSES;md5=28584ceb716d242782f9a7a7593c9ff2 \
"

SRC_URI[arm.md5sum] = "a7f3667ac9f24e434e7a85908d1db256"
SRC_URI[arm.sha256sum] ="339656dca947f1cff29635fbe7570b5ea04ca7256fd2177cf396711a60a8f26a"

# For ARCH64 we use another tarball.
SRC_URI[aarch64.md5sum] = "abfabf1464b7a1da0322dfd780415e48"
SRC_URI[aarch64.sha256sum] ="411956c8a41857c95dea5af6a41c7c0ab09310d621e054693d9e8ee57b23ed35"
