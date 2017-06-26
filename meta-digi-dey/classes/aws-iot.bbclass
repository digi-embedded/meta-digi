# Adds AWS IoT device SDK for embedded C configuration
#

# Customer specific MQTT HOST
AWS_IOT_MQTT_HOST ?= ""

# Default port for MQTT/S
AWS_IOT_MQTT_PORT ?= "8883"

# Thing Name of the Shadow the device is associated with
AWS_IOT_MY_THING_NAME ?= "AWS-IoT-C-SDK"

# Root CA file name
AWS_IOT_ROOT_CA_FILENAME ?= "rootCA.crt"

# Device signed certificate file name
AWS_IOT_CERTIFICATE_FILENAME ?= "cert.pem"

# Device private key filename
AWS_IOT_PRIVATE_KEY_FILENAME ?= "privkey.pem"

# MQTT PubSub

# Any time a message is sent out through the MQTT layer. The message is copied
# into this buffer anytime a publish is done.
# This will also be used in the case of Thing Shadow
AWS_IOT_MQTT_TX_BUF_LEN ?= "512"

# Any message that comes into the device should be less than this buffer size.
# If a received message is bigger than this buffer size the message will be
# dropped
AWS_IOT_MQTT_RX_BUF_LEN ?= "512"

# Maximum number of topic filters the MQTT client can handle at any given time.
# This should be increased appropriately when using Thing Shadow
AWS_IOT_MQTT_NUM_SUBSCRIBE_HANDLERS ?= "5"

# Thing Shadow specific configs

# At any given time we will wait for this many responses.
# This will correlate to the rate at which the shadow actions are requested
MAX_ACKS_TO_COMEIN_AT_ANY_GIVEN_TIME ?= "10"

# We could perform shadow action on any thing Name and this is maximum Thing
# Names we can act on at any given time
MAX_THINGNAME_HANDLED_AT_ANY_GIVEN_TIME ?= "10"

# These are the max tokens that is expected to be in the Shadow JSON document.
# It includes the metadata that gets published
MAX_JSON_TOKEN_EXPECTED ?= "120"

# The Thing Name should not be bigger than this value. Modify this if the Thing
# Name needs to be bigger
MAX_SIZE_OF_THING_NAME ?= "20"

# Auto Reconnect specific config

# Minimum time before the First reconnect attempt is made as part of the
# exponential back-off algorithm (milliseconds)
AWS_IOT_MQTT_MIN_RECONNECT_WAIT_INTERVAL ?= "1000"

# Maximum time interval after which exponential back-off will stop attempting
# to reconnect (milliseconds)
AWS_IOT_MQTT_MAX_RECONNECT_WAIT_INTERVAL ?= "128000"

# Logging level control: error, warn, info, debug, trace.
AWS_IOT_LOGGING_LEVEL ?= "debug"

def get_log_level(d):
    levels = ['error', 'warn', 'info', 'debug', 'trace']
    log_flags = ""

    log_level = d.getVar('AWS_IOT_LOGGING_LEVEL', True)
    if log_level == 'none':
        return ""
    if log_level not in levels:
        log_level = "debug"
        d.setVar('AWS_IOT_LOGGING_LEVEL', log_level)

    log_index = levels.index(log_level)
    for i, val in enumerate(levels):
        log_flags = log_flags + "-DENABLE_IOT_" + val.upper() + " "
        if i == log_index:
            break;

    return log_flags

#######################
# AWS Greengrass Core #
#######################

#
# Verisign root CA server certificate used to authenticate the AWS IoT server.
#
# https://www.symantec.com/content/en/us/enterprise/verisign/roots/VeriSign-Class%203-Public-Primary-Certification-Authority-G5.pem
#
AWS_GGCORE_ROOT_CA ?= "aws-root-ca.pem"

# Greengrass core device certificate
AWS_GGCORE_CERTIFICATE ?= "gg-core.pem"

# Greengrass core device private key
AWS_GGCORE_PRIVATE_KEY ?= "gg-core.key"

# Greengrass core Thing ARN
AWS_GGCORE_THING_ARN ?= ""

# AWS IoT endpoint (check your account)
# CLI: aws iot describe-endpoint
AWS_GGCORE_IOT_HOST ?= ""
