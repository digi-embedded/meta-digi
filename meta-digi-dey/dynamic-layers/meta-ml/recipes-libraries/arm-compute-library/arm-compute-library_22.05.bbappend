# Copyright 2023 Digi International Inc.

# The package's SConscript doesn't recognize the MAXLINELENGTH variable
# injected by scons.bbclass, so remove it
EXTRA_OESCONS:remove = "${SCONS_MAXLINELENGTH}"
