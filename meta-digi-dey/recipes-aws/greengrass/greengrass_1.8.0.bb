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
    file://ggc/core/THIRD-PARTY-LICENSES;md5=70018c9eb1875d260c975eef52c10657 \
"

SRC_URI[arm.md5sum] = "41e862deb244c563d438e4604b8b3ccc"
SRC_URI[arm.sha256sum] ="3658af95e21723f52533e441f8e3a9d9e167a8bc4ada6fc957201b6455438961"

# For ARCH64 we use another tarball.
SRC_URI[aarch64.md5sum] = "4fcd160f685a5131f5aabd6f7cc31b48"
SRC_URI[aarch64.sha256sum] ="8dd7341a51afe03102ea6408a6a529eee9f3a89519fdfcb14e5a9039e711f21b"
