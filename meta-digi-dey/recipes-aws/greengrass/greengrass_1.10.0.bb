# Copyright (C) 2020, Digi International Inc.

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
    file://ggc/core/THIRD-PARTY-LICENSES;md5=1f0ad815f019455e3a0efe55e888a69a \
"

SRC_URI[arm.md5sum] = "e54bb57929bc278ea89737c4abcd89e8"
SRC_URI[arm.sha256sum] ="91f3d92dca977ea504921c7dbae96a926adce441c8f9ec1896e4c8cf085d6d2e"

# For ARCH64 we use another tarball.
SRC_URI[aarch64.md5sum] = "1bdde4df4c461cd5502f7adbb79b2903"
SRC_URI[aarch64.sha256sum] ="912ecbe10398382894045f9b9dafd16eac7fabce0fc04fc9ee83c8ec8f67ca5a"
