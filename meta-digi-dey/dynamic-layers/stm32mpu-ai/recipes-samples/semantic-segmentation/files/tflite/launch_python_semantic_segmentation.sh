#!/bin/sh
weston_user=$(ps aux | grep '/usr/bin/weston '|grep -v 'grep'|awk '{print $1}')

source /usr/local/demo-ai/resources/config_board.sh
cmd="python3 /usr/local/demo-ai/semantic-segmentation/tflite/tflite_semantic_segmentation.py -m /usr/local/demo-ai/semantic-segmentation/models/deeplabv3/deeplabv3.tflite -l /usr/local/demo-ai/semantic-segmentation/models/deeplabv3/labelmap.txt --framerate $DFPS --frame_width $DWIDTH --frame_height $DHEIGHT $COMPUTE_ENGINE"

if [ "$weston_user" != "root" ]; then
	echo "user : "$weston_user
	script -qc "su -l $weston_user -c '$cmd'"
else
	$cmd
fi
