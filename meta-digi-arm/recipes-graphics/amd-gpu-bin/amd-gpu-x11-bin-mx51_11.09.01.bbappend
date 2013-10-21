# Copyright (C) 2013 Digi International.

PRINC := "${@int(PRINC) + 1}"
PR_append = "+${DISTRO}"

RCONFLICTS_${PN} = "amd-gpu-bin-mx51"

COMPATIBLE_MACHINE = "${@base_contains('DISTRO_FEATURES', 'x11', '(mx5)', 'Invalid!', d)}"
