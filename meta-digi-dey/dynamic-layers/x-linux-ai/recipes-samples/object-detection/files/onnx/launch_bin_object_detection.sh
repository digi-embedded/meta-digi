#!/bin/sh
weston_user=$(ps aux | grep '/usr/bin/weston '|grep -v 'grep'|awk '{print $1}')
source /usr/local/demo-ai/resources/config_board.sh
cmd="/usr/local/demo-ai/object-detection/onnx/onnx_object_detection -m /usr/local/demo-ai/object-detection/models/coco_ssd_mobilenet/coco_ssd_mobilenet.onnx -l /usr/local/demo-ai/object-detection/models/coco_ssd_mobilenet/labels_coco_ssd_mobilenet_onnx.txt --framerate $DFPS --frame_width $DWIDTH --frame_height $DHEIGHT"
if [ "$weston_user" != "root" ]; then
	echo "user : "$weston_user
	script -qc "su -l $weston_user -c '$cmd'"
else
	$cmd
fi
