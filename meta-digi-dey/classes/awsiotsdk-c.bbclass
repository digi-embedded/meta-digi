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

