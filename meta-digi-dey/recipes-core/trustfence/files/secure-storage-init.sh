#!/bin/sh

# Get the Key storage location from tee-supplicant config file
KEYDIR="$(cat /etc/default/tee-supplicant | tr -d \" | awk -F =/ '{ print $2 }')"

log() { echo "[secure-storage] $*"; }

# Configure encryption for the EXT4 filesystem if detected
enable_ext4_encrypt() {
	set -- $(df -T -P "${SECURE_DIR}" 2>/dev/null | awk 'NR==2 { print $1, $2 }')
	[ "${2:-}" = "ext4" ] || return 0
	if [ "${1#/dev/}" != "${1}" ]; then
		tune2fs -O encrypt "${1}" >/dev/null 2>&1
		tune2fs -l "${1}" 2>/dev/null | grep -qs 'Filesystem features:.*encrypt' || \
			{ log "Cannot enable file system encryption on ${1}"; exit 1; }
	fi
}

secure_dir_is_empty() {
	[ -z "$(find "${SECURE_DIR}" -mindepth 1 -print -quit 2>/dev/null)" ]
}

# Ensure prerequisites
command -v trustfence-fscrypt >/dev/null 2>&1 || { log "trustfence-fscrypt tool not found"; exit 1; }

start () {
	log "create $SECURE_DIR"
	# Ensure secure directory exists
	mkdir -p "$SECURE_DIR"
	log "verifiy if we are on EXT4"
	# verify if we are on EXT4 and enable encryption
	enable_ext4_encrypt

	log "Check if $KEYDIR exists"
	# check if we already have a KEYDIR
	if [ ! -d "$KEYDIR" ]; then
		log "Generating master key directory at $KEYDIR"
		install -d -m770 -o root -g tee $KEYDIR
	fi

	log "check if we already have a key"
	# check if we already have a key
	if ! trustfence-fscrypt --start-session=$SECURE_DIR >/dev/null 2>&1; then
		# check if directory is empty
		if secure_dir_is_empty; then
			log "Generating new random key"
			# start fscrypt session with random key
			trustfence-fscrypt --new-key --start-session=$SECURE_DIR >/dev/null 2>&1
		else
			log "ERROR: ${SECURE_DIR} not empty, but must be empty for initial policy setup"
			exit 1
		fi
	fi

	log "Secure storage ready at $SECURE_DIR"
}

stop() {
	log "Remove session key and lock secure storage"
	trustfence-fscrypt --end-session=$SECURE_DIR >/dev/null 2>&1
}

case "$1" in
	start)  start ;;
	stop)   stop ;;
	*) ;;
esac
