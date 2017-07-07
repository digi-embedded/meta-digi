#!/bin/sh
#
# Copyright (c) 2017, Digi International Inc.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, you can obtain one at http://mozilla.org/MPL/2.0/.
#
# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
# WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
# ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
# WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
# ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
# OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
#
# Description: AWS CLI wrapper to be used in DEY
#

SCRIPTNAME="$(basename ${0})"

USAGE="
AWS CLI wrapper to facilitate integration with Digi Embedded Yocto

Usage: ${SCRIPTNAME} [OPTIONS]

    -c, --create-certs <PATH>    Create Root CA and Greengrass Core device certificates under <PATH>
    -t, --thing-name <NAME>      Name of the Greengrass Core Thing. The script will register this Thing in your account if it is not already.
"

error() {
	printf "%s\n" "${1}"
	exit 1
}

do_create_certs() {
	GG_ROOTCA_KEY="${GG_CERTS_DIR}/root-ca.key"
	GG_ROOTCA_PEM="${GG_CERTS_DIR}/root-ca.pem"
	GG_ROOTCA_VERIF_KEY="${GG_CERTS_DIR}/root-ca-verif.key"
	GG_ROOTCA_VERIF_CSR="${GG_CERTS_DIR}/root-ca-verif.csr"
	GG_ROOTCA_VERIF_PEM="${GG_CERTS_DIR}/root-ca-verif.pem"
	GG_CORE_KEY="${GG_CERTS_DIR}/gg-core.key"
	GG_CORE_CSR="${GG_CERTS_DIR}/gg-core.csr"
	GG_CORE_PEM="${GG_CERTS_DIR}/gg-core.pem"

	# Verify that no certificate artifact exists in the certs directory
	if [ -f "${GG_ROOTCA_KEY}" ] || \
	   [ -f "${GG_ROOTCA_PEM}" ] || \
	   [ -f "${GG_ROOTCA_PEM}" ] || \
	   [ -f "${GG_ROOTCA_VERIF_KEY}" ] || \
	   [ -f "${GG_ROOTCA_VERIF_CSR}" ] || \
	   [ -f "${GG_ROOTCA_VERIF_PEM}" ] || \
	   [ -f "${GG_CORE_KEY}" ] || \
	   [ -f "${GG_CORE_CSR}" ] || \
	   [ -f "${GG_CORE_PEM}" ]; then
		error "[ERROR] Certificates directory contains artifacts from previous execution"
	fi

	mkdir -p "${GG_CERTS_DIR}"

	# Get AWS root CA certificate
	printf "[INFO] Downloading AWS root CA certificate.\n"
	AWS_ROOT_CA_URL="https://www.symantec.com/content/en/us/enterprise/verisign/roots/VeriSign-Class%203-Public-Primary-Certification-Authority-G5.pem"
	[ -f "${GG_CERTS_DIR}/aws-root-ca.pem" ] || wget -t 2 -T 30 -q --passive-ftp --no-check-certificate -O "${GG_CERTS_DIR}/aws-root-ca.pem" "${AWS_ROOT_CA_URL}"

	#
	# Create a self-signed CA certificate (if it doesn't exit)
	#
	printf "[INFO] Creating Greengrass Core CA certificate and key.\n"
	GG_ROOTCA_SUBJ="/C=US/ST=Minnesota/L=Minnetonka/O=Digi International Inc./OU=Digi Enginnering/CN=AWS IoT CA Certificate"
	openssl genrsa -out "${GG_ROOTCA_KEY}" 2048 2>/dev/null
	openssl req -x509 -new -nodes -key "${GG_ROOTCA_KEY}" -sha256 -days 1024 -out "${GG_ROOTCA_PEM}" -subj "${GG_ROOTCA_SUBJ}"

	#
	# Create verification certificate (needed to register the CA certificate)
	#
	printf "[INFO] Creating verification certificate.\n"
	if ! REG_CODE="$(aws iot get-registration-code --query registrationCode 2>/dev/null)"; then
		error "[ERROR] Unable to get registration code"
	fi
	GG_ROOTCA_VERIF_SUBJ="/C=US/ST=Minnesota/L=Minnetonka/O=Digi International Inc./OU=Digi Enginnering/CN=${REG_CODE}"
	openssl genrsa -out "${GG_ROOTCA_VERIF_KEY}" 2048 2>/dev/null
	openssl req -new -key "${GG_ROOTCA_VERIF_KEY}" -out "${GG_ROOTCA_VERIF_CSR}" -subj "${GG_ROOTCA_VERIF_SUBJ}"
	openssl x509 -req -in "${GG_ROOTCA_VERIF_CSR}" -CA "${GG_ROOTCA_PEM}" -CAkey "${GG_ROOTCA_KEY}" -CAcreateserial -out "${GG_ROOTCA_VERIF_PEM}" -days 1024 -sha256 2>/dev/null

	#
	# Create Greengrass Core device certificate
	#
	printf "[INFO] Creating Greengrass Core device certificate.\n"
	GG_CORE_SUBJ="/C=US/ST=Minnesota/L=Minnetonka/O=Digi International Inc./OU=Digi Enginnering/CN=AWS IoT Device Certificate"
	openssl genrsa -out "${GG_CORE_KEY}" 2048 2>/dev/null
	openssl req -new -key "${GG_CORE_KEY}" -out "${GG_CORE_CSR}" -subj "${GG_CORE_SUBJ}"
	openssl x509 -req -in "${GG_CORE_CSR}" -CA "${GG_ROOTCA_PEM}" -CAkey "${GG_ROOTCA_KEY}" -CAcreateserial -out "${GG_CORE_PEM}" -days 1024 -sha256 2>/dev/null
}

do_register_thing() {
	printf "[INFO] Registering Greengrass Core Thing.\n"
	if ! AWS_GGCORE_THING_ARN="$(aws iot create-thing --thing-name ${GG_THING_NAME} --query thingArn 2>/dev/null)"; then
		error "[ERROR] Unable to create Greengrass Core Thing"
	fi
}

# Use GNU 'getopt' to parse command line options
SHORT_OPTS="hc:t:"
LONG_OPTS="help,create-certs:,thing-name:"
CMDLINE_OPTS=$(getopt -o ${SHORT_OPTS} -l ${LONG_OPTS} -- "$@") || error "${USAGE}"

eval set -- "${CMDLINE_OPTS}"

while true; do
	case "$1" in
		-h|--help) printf "%s\n" "${USAGE}"; exit 0;;
		-c|--create-certs) GG_CERTS_DIR="${2}"; shift;;
		-t|--thing-name) GG_THING_NAME="${2}"; shift;;
		--) shift; break;;
	esac
	shift
done

# Sanity checks: AWS CLI needs to be installed and configured
if ! aws configure get aws_access_key_id >/dev/null 2>&1; then
	error "[ERROR] AWS CLI needs to be installed and configured"
fi

AWS_GGCORE_IOT_HOST="$(aws iot describe-endpoint 2>/dev/null)"

[ -n "${GG_CERTS_DIR}" ] && do_create_certs
[ -n "${GG_THING_NAME}" ] && do_register_thing

# Print AWS IoT configuration for DEY projects
cat <<-_EOF_

	For Greengrass enabled images, add the following configuration to your project:

	AWS_IOT_CERTS_DIR = "${GG_CERTS_DIR:-</path/to/certificates>}"
	AWS_GGCORE_IOT_HOST = "${AWS_GGCORE_IOT_HOST:-<AWS account unique endpoint>}"
	AWS_GGCORE_THING_ARN = "${AWS_GGCORE_THING_ARN:-<AWS Greengrass Core Thing ARN>}"

	Please verify variables' value is correct.

_EOF_
