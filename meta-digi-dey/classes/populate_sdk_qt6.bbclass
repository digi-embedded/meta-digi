# Copyright (c) 2023, Digi International Inc.

inherit populate_sdk_qt6_base

TOOLCHAIN_HOST_TASK:append = " nativesdk-packagegroup-qt6-toolchain-host"
TOOLCHAIN_TARGET_TASK:append = " packagegroup-qt6-modules"
