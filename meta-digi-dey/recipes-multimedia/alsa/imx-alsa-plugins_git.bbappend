# Copyright (C) 2019, Digi International Inc.

# ccimx6/ccimx6qp platforms use kernel v4.9, which is incompatible with the
# latest revision of the imx-alsa-plugins code due to UAPI changes. For these
# platforms, use an older revision which is functionally the same, but using
# the v4.9 UAPI.
SRCREV_ccimx6 = "9a63071e7734bd164017f3761b8d1944c017611f"
