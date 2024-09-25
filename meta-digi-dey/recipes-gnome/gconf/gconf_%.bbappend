# Copyright (C) 2021, Digi International Inc.

# We don't use python3 and they take up over
# 8 MiB in the filesystem, so remove them.
RDEPENDS:${PN}:remove:ccimx6ul = "python3-xml"
