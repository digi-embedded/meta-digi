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
# Select profile in local.conf (e.g. CONTAINER_TYPE = "lvgl" or "webkit").
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
PODMAN_ARTIFACT_OUTPUT_NAME ?= "${CONTAINER_NAME}_artifact_podman_${MACHINE}.tar.gz"

########################
# LXC output knobs
########################
CONTAINERS_DIR ?= "${THISDIR}/../../containers"
LXC_FOLDER ?= "/var/lib/lxc"
LXC_OUTPUT_NAME ?= "${CONTAINER_NAME}_lxc_${MACHINE}.tar.xz"
LXC_ARTIFACT_OUTPUT_NAME ?= "${CONTAINER_NAME}_artifact_lxc_${MACHINE}.tar.gz"
LXC_CONFIG_DIR ?= "${CONTAINER_PROFILE_DIR}/configs_lxc"
LXC_CONFIG_FILE ?= "${LXC_CONFIG_DIR}/config_lxc_${MACHINE}"

########################
# Artifact layout knobs
########################
CONTAINER_ARTIFACT_TEMPLATE_DIR ?= ""
CONTAINER_DEFAULT_ARTIFACT_TEMPLATE_DIR ?= "${CONTAINER_PROFILE_DIR}/artifact"
META_DIGI_REPO_DIR ?= "${THISDIR}/../../.."
CONTAINER_PACKAGE_ID ?= "${CONTAINER_NAME}"
CONTAINER_ARTIFACT_VERSION ?= "${PV}"
CONTAINER_CREATE_ARGS ?= ""
CONTAINER_CREATE_ARGS_PODMAN ?= "${CONTAINER_CREATE_ARGS}"
CONTAINER_FIRMWARE_VERSIONS ?= ""
CONTAINER_DEVICE_TYPES_JSON ?= "[\"${MACHINE}\"]"
CONTAINER_ARTIFACT_DESCRIPTION ?= ""
CONTAINER_ARTIFACT_BUILD_ID ?= ""
CONTAINER_ARTIFACT_LABELS_JSON ?= "{}"

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
IMAGE_INSTALL:append:container-lvgl = " lvgl-demo weston"
DISTRO_FEATURES:remove:container-lvgl = " wayland"

########################
# Container type customizations webkit
########################
CONTAINER_INIT_SCRIPT:container-webkit = "/start-webkit-demo.sh"
IMAGE_INSTALL:append:container-webkit = " \
    alsa-utils \
    bluez5 \
    connectcore-demo-example \
    dbus \
    libdrm \
    libgpiod-tools \
    libinput \
    libubootenv-bin \
    mesa \
    networkmanager-nmcli \
    packagegroup-dey-webkit \
    pulseaudio-server \
    python3-dbus \
    wayland \
    wayland-protocols \
    weston \
"
