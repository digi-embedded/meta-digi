#
# Copyright (C) 2026, Digi International Inc.
#
########################
# Podman artifact
########################
do_image_podman_archive[depends] += "skopeo-native:do_populate_sysroot"
do_image_podman_archive() {
    src_tar="${DEPLOY_DIR_IMAGE}/${IMAGE_BASENAME}-${OCI_IMAGE_TAG_SAFE}-oci.tar"
    if [ ! -f "${src_tar}" ]; then
        src_tar="${DEPLOY_DIR_IMAGE}/${IMAGE_NAME}.rootfs-oci.tar"
    fi
    dst_tar="${DEPLOY_DIR_IMAGE}/${PODMAN_OUTPUT_NAME}"
    image_ref="${CONTAINER_NAME}:${PODMAN_TAG}"

    if [ ! -f "${src_tar}" ]; then
        bbfatal "Expected OCI tarball not found: ${src_tar}"
    fi

    # skopeo docker-archive destination must not already exist.
    rm -f "${dst_tar}"

    bbnote "Converting OCI archive to docker-archive (${image_ref})..."
    PATH="${STAGING_BINDIR_NATIVE}:${PATH}" \
    skopeo --insecure-policy copy \
        "oci-archive:${src_tar}" \
        "docker-archive:${dst_tar}:${image_ref}"

    if [ ! -s "${dst_tar}" ]; then
        bbfatal "Podman archive not generated correctly: ${dst_tar}"
    fi

    bbnote "Podman archive ready: ${dst_tar} (${image_ref})"
}

addtask image_podman_archive after do_image_complete before do_build
