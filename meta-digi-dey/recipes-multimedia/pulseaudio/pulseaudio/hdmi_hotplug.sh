#!/bin/sh
#
# Called from udev on HDMI plug/unplug event

# Find HDMI card number
for card in /sys/class/sound/card*; do
	if readlink ${card}/device | grep -qs hdmi; then
		HDMI_CARD="${card##/sys/class/sound/card}"
	fi
done

# On HDMI plugin event, if the sink has not been loaded yet, load the
# HDMI audio sink from ALSA
if [ "${EVENT}" = "plugin" ]; then
	if ! pactl list sinks | grep -qs "imx-hdmi-soc"; then
		pactl load-module module-alsa-sink device=hw:${HDMI_CARD}
	fi
fi
