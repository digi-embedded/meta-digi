#!/bin/sh

REGDOMAIN_FILE="/var/run/regdomain"

previous=$(cat "${REGDOMAIN_FILE}" 2>/dev/null)

# Extract the 'global' regulatory domain
current=$(iw reg get | grep -m1 '^country' | cut -d' ' -f2 | tr -d ':')

if [ -n "${current}" ] && [ "${current}" != "${previous}" ]; then
	echo "Global regulatory domain changed to '${current}'"
	echo "${current}" > "${REGDOMAIN_FILE}"
	iw reg set ${current}
fi
