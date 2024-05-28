#!/bin/sh
weston_user=$(ps aux | grep '/usr/bin/weston '|grep -v 'grep'|awk '{print $1}')

source /usr/local/demo-ai/resources/config_board.sh
cmd="python3 /usr/local/demo-ai/pose-estimation/tflite/tflite_pose_estimation.py -m /usr/local/demo-ai/pose-estimation/models/movenet/movenet_singlepose_lightning.tflite -i /usr/local/demo-ai/pose-estimation/models/movenet/testdata/ $COMPUTE_ENGINE"

if [ "$weston_user" != "root" ]; then
	echo "user : "$weston_user
	script -qc "su -l $weston_user -c '$cmd'"
else
	$cmd
fi
