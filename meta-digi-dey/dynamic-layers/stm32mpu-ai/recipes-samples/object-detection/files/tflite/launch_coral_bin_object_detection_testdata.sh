#!/bin/sh
weston_user=$(ps aux | grep '/usr/bin/weston '|grep -v 'grep'|awk '{print $1}')

cmd="/usr/local/demo-ai/object-detection/coral/coral_object_detection -m /usr/local/demo-ai/object-detection/models/coco_ssd_mobilenet/coco_ssd_mobilenet_edgetpu.tflite -l /usr/local/demo-ai/object-detection/models/coco_ssd_mobilenet/labels_coco_ssd_mobilenet.txt -i /usr/local/demo-ai/object-detection/models/coco_ssd_mobilenet/testdata/ --edgetpu"
if [ "$weston_user" != "root" ]; then
	echo "user : "$weston_user
	script -qc "su -l $weston_user -c '$cmd'"
else
	$cmd
fi
