# Copyright (C) 2025, Digi International Inc.

# Remove addons packagegroup to reduce SDK size and remove spurious
# dependencies
RDEPENDS:${PN}:remove:dey = "nativesdk-packagegroup-qt6-toolchain-host-addons"
