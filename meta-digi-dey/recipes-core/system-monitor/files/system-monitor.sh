#!/bin/sh
#===============================================================================
#
#  system-monitor.sh
#
#  Copyright (C) 2019 by Digi International Inc.
#  All rights reserved.
#
#  This program is free software; you can redistribute it and/or modify it
#  under the terms of the GNU General Public License version 2 as published by
#  the Free Software Foundation.
#
#  !Description: Checks the system status and takes actions if the system is
#                not in the desired state.
#
#  The system-monitor systemd service calls this script periodically.
#
#===============================================================================

# Watchdog time in specified in system-monitor.service
# The script will monitor the system and kick the software watchdod
# every 1/4 of the time watchdog time configured.
WDOG_TIME_SEC=$((WATCHDOG_USEC / 1000000 / 4))

log() {
	if type "systemd-cat" >/dev/null 2>/dev/null; then
		systemd-cat -p "${1}" -t system-monitor printf "%s" "${2}"
	else
		logger -p "${1}" -t system-monitor "${2}"
	fi
}

checks_failed=0

monitor()
{
	# run system check actions
	if run-parts -a "${checks_failed}" "/etc/system-monitor/check.d"; then
		log info "system check correct"
		checks_failed=0
	else
		# system check failed, run recover actions
		checks_failed=$((checks_failed + 1))
		log warning "system check failed (${checks_failed} consecutive checks failed so far)"
		run-parts -a "${checks_failed}" "/etc/system-monitor/recover-action.d"
	fi
}

log info "started"
/bin/systemd-notify --ready

# system-monitor main loop:
#   * Monitor system status
#   * Kick software watchdog
#   * Wait for remaining time (if any) until next check
while true; do
	loop_start=$(date +%s)

	monitor
	/bin/systemd-notify WATCHDOG=1

	time_to_wait=$((WDOG_TIME_SEC - ($(date +%s) - loop_start)))
	[ ${time_to_wait} -gt 0 ] && sleep ${time_to_wait}
done
