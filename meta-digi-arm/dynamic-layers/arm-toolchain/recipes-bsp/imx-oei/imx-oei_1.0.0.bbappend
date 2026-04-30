# Copyright (C) 2025,2026 Digi International Inc.

# Select internal or Github imx-oei repo
IMX_OEI_URI_STASH = "${DIGI_MTK_GIT}/emp/imx-oei.git;protocol=ssh"
IMX_OEI_URI_GITHUB = "${DIGI_GITHUB_GIT}/imx-oei.git;protocol=https"
IMX_OEI_SRC:dey = "${@oe.utils.conditional('DIGI_INTERNAL_GIT', '1', '${IMX_OEI_URI_STASH}', '${IMX_OEI_URI_GITHUB}', d)}"

SRCBRANCH:dey = "dey/scarthgap/lf-6.6.52-2.2.2"
# NXP's 'lf-6.6.52_2.2.2' release + patches
SRCREV:dey = "0a1fbc5ce9c4bed08d0add95c911749e56a7d57a"

OEI_DEBUG:dey = "1"
