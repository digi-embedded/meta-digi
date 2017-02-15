#!/bin/sh

# The full CAAM function is exclusive with the Mega/Fast mix off feature in DSM.
# If CAAM is enabled, the Mega/Fast mix off feature needs to be disabled, and
# the user should enable CAAM jr0 as a wakeup source after the kernel boots up,
# and then Mega/Fast mix will keep the power on in DSM.
JR0_WAKEUP="/sys/bus/platform/devices/2100000.aips-bus/2140000.caam/2141000.jr0/power/wakeup"
if [ -f "${JR0_WAKEUP}" ]; then
	echo "enabled" > "${JR0_WAKEUP}"
fi
