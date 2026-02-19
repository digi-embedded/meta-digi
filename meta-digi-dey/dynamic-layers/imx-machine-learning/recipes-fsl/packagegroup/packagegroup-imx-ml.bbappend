# Copyright (C) 2023-2025, Digi International Inc.

# For the ccimx93 install only tensorflow-lite engine, as it is
# the only one supported in the NPU. This reduces significantly
# the rootfs image size.
ML_PKGS:ccimx93 = "tensorflow-lite"
