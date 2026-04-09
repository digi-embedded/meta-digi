# Copyright (C) 2025, 2026, Digi International Inc.

# Select internal or Github imx-system-manager repo
IMX_SYSTEM_MANAGER_URI_STASH = "${DIGI_MTK_GIT}/emp/imx-sm.git;protocol=ssh"
IMX_SYSTEM_MANAGER_URI_GITHUB = "${DIGI_GITHUB_GIT}/imx-sm.git;protocol=https"
IMX_SYSTEM_MANAGER_SRC:dey = "${@oe.utils.conditional('DIGI_INTERNAL_GIT', '1', '${IMX_SYSTEM_MANAGER_URI_STASH}', '${IMX_SYSTEM_MANAGER_URI_GITHUB}', d)}"

SRCBRANCH:dey = "dey/scarthgap/lf-6.6.52-2.2.2"
# NXP's 'lf-6.6.52_2.2.2' release + patches
SRCREV:dey = "ecd89d0bc35687c7e1e19b47cf6bcdefc3a3fe68"

# Disable debug monitor by default
PACKAGECONFIG ??= "m0"
