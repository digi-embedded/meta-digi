#!/bin/sh
weston_user=$(ps aux | grep '/usr/bin/weston '|grep -v 'grep'|awk '{print $1}')

source /usr/local/demo-ai/resources/config_board.sh
cmd="/usr/local/demo-ai/image-classification/tflite/tflite_image_classification -m /usr/local/demo-ai/image-classification/models/mobilenet/$IMAGE_CLASSIFICATION_MODEL.tflite -l /usr/local/demo-ai/image-classification/models/mobilenet/$IMAGE_CLASSIFICATION_LABEL.txt -i /usr/local/demo-ai/image-classification/models/mobilenet/testdata/ $COMPUTE_ENGINE"

if [ "$weston_user" != "root" ]; then
	echo "user : "$weston_user
	script -qc "su -l $weston_user -c '$cmd'"
else
	$cmd
fi
