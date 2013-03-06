#!/bin/sh -l

set -e

FIRMWARE=$1
DEVPATH=$2

if echo "${FIRMWARE}" | egrep -q "ath6k" ; then 
	/bin/echo "Refusing to load ${FIRMWARE}."
else
	/bin/echo "Loading ${FIRMWARE}"
	/lib/udev/firmware --firmware=${FIRMWARE} --devpath=${DEVPATH}
fi


