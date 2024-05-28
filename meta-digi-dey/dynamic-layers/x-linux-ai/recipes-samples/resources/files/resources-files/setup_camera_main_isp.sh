#!/bin/bash
WIDTH=$1
HEIGHT=$2
FPS=$3
DEVICE=$4
FMT=RGB16
CAMERA_WIDTH=640
CAMERA_HEIGHT=480
displaybuscode=RGB565_2X8_LE

function cmd() {
	cmd=$1
	eval $cmd > /dev/null 2>&1
}

function is_dcmipp_present() {
	DCMIPP_SENSOR="NOTFOUND"
	# on disco board ov5640 camera can be present on csi connector
	for video in $(find /sys/class/video4linux -name "video*" -type l);
	do
		if [ "$(cat $video/name)" = "dcmipp_main_capture" ]; then
			cd $video/device/
			mediadev=/dev/$(ls -d media*)
			cd -
			for sub in $(find /sys/class/video4linux -name "v4l-subdev*" -type l);
			do
				subdev_name=$(tr -d '\0' < $sub/name | awk '{print $1}')
				if [ "$subdev_name" = "ov5640" ] || [ "$subdev_name" = "imx335" ]; then
					DCMIPP_SENSOR=$subdev_name
					V4L_DEVICE="$(basename $video)"
					sensorsubdev="$(tr -d '\0' < $sub/name)"
					sensordev=$(media-ctl -d $mediadev -p -e "$sensorsubdev" | grep "node name" | awk -F\name '{print $2}')
					#interface is connected to input of isp (":1 [ENABLED" with media-ctl -p)
					interfacesubdev=$(media-ctl -d $mediadev -p -e "dcmipp_main_isp" | grep ":1 \[ENABLED" | awk -F\" '{print $2}')
					return
				fi
			done
		fi
	done
}

get_webcam_device() {
    found="NOTFOUND"
    for video in $(find /sys/class/video4linux -name "video*" -type l | sort);
    do
        if [ "$(cat $video/name)" = "dcmipp_main_capture" ] || [ "$(cat $video/name)" = "st,stm32mp25-vdec-dec" ] || [ "$(cat $video/name)" = "st,stm32mp25-venc-enc" ] || [ "$(cat $video/name)" = "dcmipp_dump_capture" ] || [ "$(cat $video/name)" = "dcmipp_aux_capture" ] || [ "$(cat $video/name)" = "dcmipp_main_isp_stat_capture" ] ; then
            found="FOUND"
        else
            V4L_DEVICE="$(basename $video)"
            break;
        fi
    done
}

# ------------------------------
#         main
# ------------------------------

#if a video device is specified in the launch script use it if not search for
#dcmipp camera of a webcam
if [ "$DEVICE" != "" ]; then
	echo "A video device has been specified"
	DCMIPP_SENSOR="NOTFOUND"
	if [ "$(cat /sys/class/video4linux/$DEVICE/name)" = "dcmipp_dump_capture" ] || [ "$(cat /sys/class/video4linux/$DEVICE/name)" = "dcmipp_main_capture" ] ; then
		cd /sys/class/video4linux/$DEVICE/device/
		mediadev=/dev/$(ls -d media*)
		cd -
		for sub in $(find /sys/class/video4linux -name "v4l-subdev*" -type l);
		do
			subdev_name=$(tr -d '\0' < $sub/name | awk '{print $1}')
			if [ "$subdev_name" = "imx335" ] || [ "$subdev_name" = "ov5640" ]; then
				DCMIPP_SENSOR=$subdev_name
				echo "DCMIPP_SENSOR="$DCMIPP_SENSOR
				V4L_DEVICE="$(basename $DEVICE)"
				sensorsubdev="$(tr -d '\0' < $sub/name)"
				sensordev=$(media-ctl -d $mediadev -p -e "$sensorsubdev" | grep "node name" | awk -F\name '{print $2}')
				#interface is connected to input of isp (":1 [ENABLED" with media-ctl -p)
				interfacesubdev=$(media-ctl -d $mediadev -p -e "dcmipp_main_isp" | grep ":1 \[ENABLED" | awk -F\" '{print $2}')
			fi
		done
	else
		if [ "$(cat /sys/class/video4linux/$DEVICE/name)" != "dcmipp_dump_capture" ] || [ "$(cat /sys/class/video4linux/$DEVICE/name)" != "dcmipp_main_capture" ] ; then
			V4L_DEVICE="$(basename $DEVICE)"
		else
			echo "camera specified not valid ... try to find another camera"
			is_dcmipp_present
		fi
	fi
	echo "DCMIPP_SENSOR="$DCMIPP_SENSOR
else
	is_dcmipp_present
	echo "DCMIPP_SENSOR="$DCMIPP_SENSOR
fi

if [ "$DCMIPP_SENSOR" != "NOTFOUND" ]; then
	#Use sensor in raw-bayer format
	sensorbuscode=`v4l2-ctl --list-subdev-mbus-codes -d $sensordev | grep SRGGB | awk -FMEDIA_BUS_FMT_ '{print $2}'`

	if [ "$DCMIPP_SENSOR" = "ov5640" ]; then
		#OV5640 only support 720p with raw-bayer format
		CAMERA_WIDTH=1280
		CAMERA_HEIGHT=720
		#OV5640 claims to support all raw bayer combinations but always output SBGGR8_1X8...
		sensorbuscode=SBGGR8_1X8
	elif [ "$DCMIPP_SENSOR" = "imx335" ]; then
		v4l2-ctl -d $sensordev -c exposure=4490
		#Do exposure correction continuously in background
#        while : ; do /home/weston/isp -w > /dev/null ; done &
	fi

	#Let sensor return its prefered resolution & format
	media-ctl -d $mediadev --set-v4l2 "'$sensorsubdev':0[fmt:$sensorbuscode/${SENSORWIDTH}x${SENSORHEIGHT}@1/${FPS} field:none]" > /dev/null 2>&1
	sensorfmt=`media-ctl -d $mediadev --get-v4l2 "'$sensorsubdev':0" | awk -F"fmt:" '{print $2}' | awk -F" " '{print $1}'`
	SENSORWIDTH=`echo $sensorfmt | awk -F"/" '{print $2}' | awk -F"x" '{print $1}'`
	SENSORHEIGHT=`echo $sensorfmt | awk -F"/" '{print $2}' | awk -F"x" '{print $2}' | awk -F" " '{print $1}' | awk -F"@" '{print $1}'`

    #Use main pipe for debayering, scaling and color conversion
	echo "Mediacontroller graph:"
	cmd "  media-ctl -d $mediadev --set-v4l2 \"'$sensorsubdev':0[fmt:$sensorbuscode/${SENSORWIDTH}x${SENSORHEIGHT}]\""
	cmd "  media-ctl -d $mediadev --set-v4l2 \"'$interfacesubdev':1[fmt:$sensorbuscode/${SENSORWIDTH}x${SENSORHEIGHT}]\""
	cmd "  media-ctl -d $mediadev --set-v4l2 \"'dcmipp_main_isp':1[fmt:RGB888_1X24/${SENSORWIDTH}x${SENSORHEIGHT} field:none]\""
	cmd "  media-ctl -d $mediadev --set-v4l2 \"'dcmipp_main_postproc':0[compose:(0,0)/${WIDTH}x${HEIGHT}]\""
	cmd "  media-ctl -d $mediadev --set-v4l2 \"'dcmipp_main_postproc':1[fmt:$displaybuscode/${WIDTH}x${HEIGHT}]\""
	echo ""

	#v4l2-ctl -d /dev/v4l-subdev6 --set-ctrl=horizontal_flip=1
	V4L2_CAPS="video/x-raw, format=$FMT, width=$WIDTH, height=$HEIGHT"
	V4L_OPT=""
else
	if [ "$DEVICE" = "" ];then
		get_webcam_device
	fi
	# suppose we have a webcam
	V4L2_CAPS="video/x-raw, width=$WIDTH, height=$HEIGHT"
	V4L_OPT="io-mode=4"
	v4l2-ctl --set-parm=20
fi

echo "V4L_DEVICE="$V4L_DEVICE
echo "V4L2_CAPS="$V4L2_CAPS
echo "DCMIPP_SENSOR="$DCMIPP_SENSOR
