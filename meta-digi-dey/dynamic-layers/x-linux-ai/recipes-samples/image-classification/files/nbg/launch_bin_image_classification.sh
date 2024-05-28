#!/bin/sh
weston_user=$(ps aux | grep '/usr/bin/weston '|grep -v 'grep'|awk '{print $1}')

source /usr/local/demo-ai/resources/config_board.sh
cmd="/usr/local/demo-ai/image-classification/nbg/nbg_image_classification -m /usr/local/demo-ai/image-classification/models/mobilenet/$IMAGE_CLASSIFICATION_MODEL.nb -l /usr/local/demo-ai/image-classification/models/mobilenet/$IMAGE_CLASSIFICATION_LABEL\_nbg.txt --framerate $DFPS --frame_width $DWIDTH --frame_height $DHEIGHT"

if [ "$weston_user" != "root" ]; then
	echo "user : "$weston_user
	script -qc "su -l $weston_user -c '$cmd'"
else
	$cmd
fi
