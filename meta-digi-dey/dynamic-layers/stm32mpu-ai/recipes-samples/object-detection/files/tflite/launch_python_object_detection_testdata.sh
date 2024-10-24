#!/bin/sh
weston_user=$(ps aux | grep '/usr/bin/weston '|grep -v 'grep'|awk '{print $1}')

source /usr/local/demo-ai/resources/config_board.sh
cmd="python3 /usr/local/demo-ai/object-detection/tflite/tflite_object_detection.py -m /usr/local/demo-ai/object-detection/models/coco_ssd_mobilenet/coco_ssd_mobilenet.tflite -l /usr/local/demo-ai/object-detection/models/coco_ssd_mobilenet/labels_coco_ssd_mobilenet.txt -i /usr/local/demo-ai/object-detection/models/coco_ssd_mobilenet/testdata/ $COMPUTE_ENGINE"

if [ "$weston_user" != "root" ]; then
	echo "user : "$weston_user
	script -qc "su -l $weston_user -c '$cmd'"
else
	$cmd
fi
