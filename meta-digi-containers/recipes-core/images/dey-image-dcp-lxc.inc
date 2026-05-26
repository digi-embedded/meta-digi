#
# Copyright (C) 2026, Digi International Inc.
#
########################
# LXC artifact
########################
do_image_lxc_bundle() {
    rootfs_tar="${DEPLOY_DIR_IMAGE}/${IMAGE_LINK_NAME}.tar.xz"
    if [ ! -f "${rootfs_tar}" ]; then
        rootfs_tar="${DEPLOY_DIR_IMAGE}/${IMAGE_NAME}.tar.xz"
    fi
    lxc_tar="${DEPLOY_DIR_IMAGE}/${LXC_OUTPUT_NAME}"
    mkdir -p "${T}"
    export TMPDIR="${T}"
    workdir="$(mktemp -d "${T}/dey-image-dcp-lxc.XXXXXX")"
    cleanup_workdir() {
        rm -rf "${workdir}"
    }
    trap cleanup_workdir EXIT HUP INT TERM

    if [ ! -f "${rootfs_tar}" ]; then
        bbfatal "Expected rootfs tarball not found: ${rootfs_tar}"
    fi

    lxc_dir="${workdir}/lxc-payload"
    mkdir -p "${lxc_dir}/rootfs"

    bbnote "Extracting rootfs for LXC bundle..."
    tar -xf "${rootfs_tar}" -C "${lxc_dir}/rootfs"

    config_in="${LXC_CONFIG_FILE}"
    if [ ! -f "${config_in}" ]; then
        bbfatal "LXC config file not found for CONTAINER_TYPE='${CONTAINER_TYPE}' and MACHINE='${MACHINE}': ${config_in}"
    fi

    sed \
        -e "s|@LXC_ARCH@|${TARGET_ARCH}|g" \
        -e "s|@LXC_FOLDER@|${LXC_FOLDER}|g" \
        -e "s|@CONTAINER_NAME@|${CONTAINER_NAME}|g" \
        -e "s|@CONTAINER_INIT_MANAGER@|${CONTAINER_INIT_MANAGER}|g" \
        -e "s|@CONTAINER_INIT_SCRIPT@|${CONTAINER_INIT_SCRIPT}|g" \
        "${config_in}" > "${lxc_dir}/config"

    tar -C "${lxc_dir}" -czf "${lxc_tar}" config rootfs

    if [ ! -s "${lxc_tar}" ]; then
        bbfatal "LXC bundle was not generated correctly: ${lxc_tar}"
    fi

    bbnote "LXC bundle ready: ${lxc_tar}"
}

addtask image_lxc_bundle after do_image_complete before do_build
