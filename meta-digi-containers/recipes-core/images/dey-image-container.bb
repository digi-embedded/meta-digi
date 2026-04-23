# Copyright (C) 2026, Digi International Inc.

SUMMARY = "Minimal LXC container image"
LICENSE = "MIT"

require dey-image-container-artifact.inc
require dey-image-container-fragments.inc
require dey-image-container-lxc.inc
require dey-image-container-podman.inc

inherit core-image image-container image-oci

########################
# Image base settings
########################
IMAGE_FEATURES = ""
IMAGE_FSTYPES = "tar.xz oci"
IMAGE_LINGUAS = "en-us"
NO_RECOMMENDATIONS = "1"

########################
# Container profile
########################
# Select profile in local.conf (e.g. CONTAINER_TYPE = "lvgl", "webkit" or "flutter").
CONTAINER_TYPE ?= "base"
OVERRIDES:append = ":container-${CONTAINER_TYPE}"

########################
# Container runtime knobs
########################
CONTAINER_INIT_MANAGER ?= "/usr/bin/docker-init"
CONTAINER_INIT_SCRIPT ?= "sleep infinity"
CONTAINER_NAME ?= "${CONTAINER_TYPE}-container"
CONTAINER_PROFILE_DIR ?= "${CONTAINERS_DIR}/${CONTAINER_TYPE}"
CONTAINER_ROOTFS_OVERLAY_DIRS ?= ""
CONTAINER_DEFAULT_ROOTFS_DIR ?= "${CONTAINER_PROFILE_DIR}/rootfs_files"
CONTAINER_ROOTFS_OVERLAY_TARBALLS ?= ""

########################
# Podman output knobs
########################
PODMAN_TAG     ?= "${CONTAINER_NAME}-tag"
PODMAN_OUTPUT_NAME ?= "${CONTAINER_NAME}_podman_${MACHINE}.tar"

########################
# LXC output knobs
########################
CONTAINERS_DIR ?= "${THISDIR}/../../containers"
LXC_FOLDER ?= "/var/lib/lxc"
LXC_OUTPUT_NAME ?= "${CONTAINER_NAME}_lxc_${MACHINE}.tar.gz"
LXC_CONFIG_DIR ?= "${CONTAINER_PROFILE_DIR}/configs_lxc"
LXC_CONFIG_FILE ?= "${LXC_CONFIG_DIR}/config_lxc_${MACHINE}"

########################
# Artifact layout knobs
########################
CONTAINER_ARTIFACT_TEMPLATE_DIR ?= ""
CONTAINER_DEFAULT_ARTIFACT_TEMPLATE_DIR ?= "${CONTAINER_PROFILE_DIR}/artifact"
META_DIGI_REPO_DIR ?= "${THISDIR}/../../.."
CONTAINER_FRIENDLY_NAME ?= "${CONTAINER_NAME}"
CONTAINER_ARTIFACT_VERSION ?= "${PV}"
CONTAINER_CREATE_ARGS ?= ""
CONTAINER_CREATE_ARGS_PODMAN ?= "${CONTAINER_CREATE_ARGS}"
CONTAINER_FIRMWARE_VERSIONS ?= ""
CONTAINER_DEVICE_TYPES_JSON ?= "[\"${MACHINE}\"]"
CONTAINER_ARTIFACT_DESCRIPTION ?= ""
CONTAINER_ARTIFACT_BUILD_ID ?= ""
CONTAINER_ARTIFACT_LABELS_JSON ?= "{}"
# monitor and (auto/re)start configuration
CONTAINER_AUTOSTART ?= "true"
CONTAINER_MONITOR ?= "true"
CONTAINER_DRM_ENABLED ?= "true"
CONTAINER_DRM_STATS_SAMPLE_INTERVAL ?= "30"
CONTAINER_DRM_STATS_LIST_OF_METRICS ?= "[\"uptime\", \"cpu\", \"mem\", \"net/rx\", \"net/tx\", \"disk/read\", \"disk/write\"]"
CONTAINER_RESTART_ENABLED ?= "true"
CONTAINER_RESTART_MAX_RETRIES ?= "3"
CONTAINER_RESTART_WINDOW ?= "60"
CONTAINER_RESTART_RETRY_DELAY ?= "3"

########################
# Container image behavior
########################
VIRTUAL-RUNTIME_init_manager = ""
VIRTUAL-RUNTIME_initscripts = ""
IMAGE_CONTAINER_NO_DUMMY = "1"

########################
# OCI metadata mapping
########################
OCI_IMAGE_TAG ?= "${PODMAN_TAG}"
OCI_IMAGE_TAG_SAFE = "${@d.getVar('OCI_IMAGE_TAG').replace(':', '_')}"
OCI_IMAGE_ENTRYPOINT = "${CONTAINER_INIT_MANAGER}"
OCI_IMAGE_ENTRYPOINT_ARGS = "${CONTAINER_INIT_SCRIPT}"

########################
# Base packages
########################
IMAGE_INSTALL = " \
    base-files \
    base-passwd \
    busybox \
    netbase \
    tini \
"

########################
# Container type customizations LVGL
########################
CONTAINER_INIT_SCRIPT:container-lvgl = "/start-lvgl-demo.sh"
CONTAINER_FRIENDLY_NAME:container-lvgl = "LVGL Demo"
CONTAINER_CREATE_ARGS_PODMAN:container-lvgl:ccmp25 = " \
    --privileged \
    --network none \
    --tmpfs /dev/shm:rw,nosuid,nodev,mode=1777 \
    --device /dev/dri \
    --device /dev/input \
    --device /dev/galcore \
    --device /dev/tty \
    --device /dev/tty0 \
    --device /dev/tty1 \
    --device /dev/tty7 \
    --volume /run/udev:/run/udev:ro \
    --tty \
"
IMAGE_INSTALL:append:container-lvgl = " \
    lvgl-demo \
    weston \
    weston-init \
"
DISTRO_FEATURES:remove:container-lvgl = " wayland"

########################
# Container type customizations webkit
########################
CONTAINER_INIT_SCRIPT:container-webkit = "/start-webkit-demo.sh"
CONTAINER_FRIENDLY_NAME:container-webkit = "WebKit Demo"
CONTAINER_CREATE_ARGS_PODMAN:container-webkit:ccmp25 = " \
    --privileged \
    --network host \
    --tmpfs /dev/shm:rw,nosuid,nodev,mode=1777 \
    --device /dev/dri \
    --device /dev/input \
    --device /dev/galcore \
    --device /dev/fb0 \
    --device /dev/snd \
    --device /dev/tty \
    --device /dev/tty0 \
    --device /dev/tty1 \
    --device /dev/tty7 \
    --device /dev/gpiochip5 \
    --device /dev/mmcblk0boot0 \
    --device /dev/mmcblk0boot1 \
    --device /dev/mailbox0 \
    --device /dev/video0 \
    --device /dev/video1 \
    --device /dev/video2 \
    --device /dev/video3 \
    --device /dev/video4 \
    --device /dev/video5 \
    --device /dev/video6 \
    --device /dev/media0 \
    --device /dev/media1 \
    --device /dev/media2 \
    --device /dev/v4l-subdev0 \
    --device /dev/v4l-subdev1 \
    --device /dev/v4l-subdev2 \
    --device /dev/v4l-subdev3 \
    --device /dev/v4l-subdev4 \
    --device /dev/v4l-subdev5 \
    --device /dev/v4l-subdev6 \
    --device /dev/v4l-subdev7 \
    --volume /run/udev:/run/udev:ro \
    --volume /etc/asound.conf:/etc/asound.conf:ro \
    --volume /run/pulse:/run/pulse \
    --volume /run/dbus/system_bus_socket:/run/dbus/system_bus_socket \
    --volume /dev/log:/dev/log \
    --volume /run/systemd/journal:/run/systemd/journal \
    --volume /etc/fw_env.config:/etc/fw_env.config:ro \
    --env PULSE_SERVER=unix:/run/pulse/native \
    --tty \
"
IMAGE_INSTALL:append:container-webkit = " \
    adwaita-icon-theme-symbolic \
    alsa-utils \
    bluez5 \
    connectcore-demo-example \
    dbus \
    fontconfig-utils \
    hicolor-icon-theme \
    libdrm \
    liberation-fonts \
    libgpiod-tools \
    libinput \
    libubootenv-bin \
    networkmanager-nmcli \
    packagegroup-dey-gstreamer \
    packagegroup-dey-webkit \
    packagegroup-core-weston \
    pulseaudio-server \
    python3-dbus \
    ttf-dejavu-sans \
    ttf-dejavu-sans-mono \
    ttf-dejavu-serif \
    wayland \
    wayland-protocols \
    weston \
    weston-init \
"

IMAGE_INSTALL:append:container-webkit:ccmp25 = " \
    gcnano-userland-multi-binary-stm32mp \
    libgles1-gcnano \
    libopenvg-gcnano \
    libvulkan-driver-gcnano \
    packagegroup-dey-x-linux-ai \
"

########################
# Container type customizations flutter
########################
CONTAINER_INIT_SCRIPT:container-flutter = "/start-flutter-demo.sh"
CONTAINER_FRIENDLY_NAME:container-flutter = "Flutter Demo"
CONTAINER_CREATE_ARGS_PODMAN:container-flutter:ccmp25 = " \
    --privileged \
    --network none \
    --tmpfs /dev/shm:rw,nosuid,nodev,mode=1777 \
    --device /dev/dri \
    --device /dev/input \
    --device /dev/galcore \
    --device /dev/tty \
    --device /dev/tty0 \
    --device /dev/tty1 \
    --device /dev/tty7 \
    --volume /run/udev:/run/udev:ro \
    --tty \
"
CONTAINER_CREATE_ARGS_PODMAN:container-flutter:ccimx95 = " \
    --privileged \
    --network none \
    --tmpfs /dev/shm:rw,nosuid,nodev,mode=1777 \
    --device /dev/dri \
    --device /dev/input \
    --device /dev/mali0 \
    --device /dev/tty \
    --device /dev/tty0 \
    --device /dev/tty1 \
    --device /dev/tty7 \
    --volume /run/udev:/run/udev:ro \
    --tty \
"
IMAGE_INSTALL:append:container-flutter = " \
    liberation-fonts \
    packagegroup-dey-flutter \
"
IMAGE_INSTALL:append:container-flutter:ccmp25 = " \
    gcnano-userland-multi-binary-stm32mp \
    libgles2-gcnano \
"
DISTRO_FEATURES:remove:container-flutter = " wayland"
