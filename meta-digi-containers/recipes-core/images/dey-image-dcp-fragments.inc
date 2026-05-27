#
# Copyright (C) 2026, Digi International Inc.
#
########################
# Rootfs postprocess
########################
ROOTFS_POSTPROCESS_COMMAND += "apply_container_rootfs_overlays; "
apply_container_rootfs_overlays() {
    overlay_dirs="${CONTAINER_ROOTFS_OVERLAY_DIRS}"
    overlay_tarballs="${CONTAINER_ROOTFS_OVERLAY_TARBALLS}"
    default_overlay_dir="${CONTAINER_DEFAULT_ROOTFS_DIR}"

    if [ -z "${overlay_dirs}" ] && [ -d "${default_overlay_dir}" ]; then
        overlay_dirs="${default_overlay_dir}"
        bbnote "Using default profile rootfs overlay: ${default_overlay_dir}"
    fi

    for overlay_dir in ${overlay_dirs}; do
        if [ ! -d "${overlay_dir}" ]; then
            bbfatal "CONTAINER_ROOTFS_OVERLAY_DIRS entry does not exist or is not a directory: ${overlay_dir}"
        fi
        bbnote "Applying rootfs overlay directory: ${overlay_dir}"
        (
            cd "${overlay_dir}" || exit 1
            tar -cf - --exclude='.keep' --exclude='*/.keep' .
        ) | tar -xf - -C "${IMAGE_ROOTFS}"

        # Ensure copied shell scripts are executable inside the container rootfs.
        (
            cd "${overlay_dir}" || exit 1
            find . -type f -name '*.sh'
        ) | while IFS= read -r relpath; do
            relpath="${relpath#./}"
            chmod 0755 "${IMAGE_ROOTFS}/${relpath}"
        done
    done

    for overlay_tarball in ${overlay_tarballs}; do
        if [ ! -f "${overlay_tarball}" ]; then
            bbfatal "CONTAINER_ROOTFS_OVERLAY_TARBALLS entry does not exist: ${overlay_tarball}"
        fi
        bbnote "Extracting rootfs overlay tarball: ${overlay_tarball}"
        tar -xzf "${overlay_tarball}" -C "${IMAGE_ROOTFS}"
    done
}

ROOTFS_POSTPROCESS_COMMAND += "apply_container_rootfs_runtime_defaults; "
apply_container_rootfs_runtime_defaults() {
    placeholder_files="$(grep -rl "@WAYLAND_DISPLAY@" "${IMAGE_ROOTFS}" 2>/dev/null || true)"

    if [ -z "${placeholder_files}" ]; then
        return 0
    fi

    printf '%s\n' "${placeholder_files}" | while IFS= read -r placeholder_file; do
        [ -n "${placeholder_file}" ] || continue
        sed -i \
            -e "s|@WAYLAND_DISPLAY@|${WAYLAND_DISPLAY}|g" \
            "${placeholder_file}"
        bbnote "Applied WAYLAND_DISPLAY='${WAYLAND_DISPLAY}' to ${placeholder_file}"
    done
}
