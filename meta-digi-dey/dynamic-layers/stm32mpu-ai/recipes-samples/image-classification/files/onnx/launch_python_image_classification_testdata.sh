#!/bin/sh
weston_user=$(ps aux | grep '/usr/bin/weston '|grep -v 'grep'|awk '{print $1}')

source /usr/local/demo-ai/resources/config_board.sh
cmd="python3 /usr/local/demo-ai/image-classification/onnx/onnx_image_classification.py -m /usr/local/demo-ai/image-classification/models/mobilenet/$IMAGE_CLASSIFICATION_MODEL.onnx -l /usr/local/demo-ai/image-classification/models/mobilenet/$IMAGE_CLASSIFICATION_LABEL_onnx.txt -i /usr/local/demo-ai/image-classification/models/mobilenet/testdata/"
if [ "$weston_user" != "root" ]; then
	echo "user : "$weston_user
	script -qc "su -l $weston_user -c '$cmd'"
else
	$cmd
fi
