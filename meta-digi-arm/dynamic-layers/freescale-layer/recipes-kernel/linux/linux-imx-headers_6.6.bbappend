# Copyright (C) 2025, Digi International Inc.

# In terms of the i.MX linux headers, the ones in our fork are identical to the
# ones in the i.MX fork, so there's no reason to download the entire i.MX Linux
# repo to get headers that are already available in ours.
SRC_URI = "${LINUX_GIT_URI};nobranch=1"
