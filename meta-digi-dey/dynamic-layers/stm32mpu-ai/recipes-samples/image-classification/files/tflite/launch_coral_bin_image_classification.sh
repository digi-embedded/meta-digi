#!/bin/sh
weston_user=$(ps aux | grep '/usr/bin/weston '|grep -v 'grep'|awk '{print $1}')

source /usr/local/demo-ai/resources/config_board.sh
cmd="/usr/local/demo-ai/image-classification/coral/coral_image_classification -m /usr/local/demo-ai/image-classification/models/mobilenet/mobilenet_v1_1.0_224_quant_edgetpu.tflite -l /usr/local/demo-ai/image-classification/models/mobilenet/labels.txt --framerate $DFPS --frame_width $DWIDTH --frame_height $DHEIGHT --edgetpu"

if [ "$weston_user" != "root" ]; then
	echo "user : "$weston_user
	script -qc "su -l $weston_user -c '$cmd'"
else
	$cmd
fi
