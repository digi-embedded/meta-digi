#
# Copyright (C) 2026, Digi International Inc.
#
########################
# Unified artifact bundles
########################
do_image_container_artifacts() {
    lxc_bundle="${DEPLOY_DIR_IMAGE}/${LXC_OUTPUT_NAME}"
    podman_archive="${DEPLOY_DIR_IMAGE}/${PODMAN_OUTPUT_NAME}"
    template_dir="${CONTAINER_ARTIFACT_TEMPLATE_DIR}"
    default_template_dir="${CONTAINER_DEFAULT_ARTIFACT_TEMPLATE_DIR}"
    image_prefix="${DEPLOY_DIR_IMAGE}/${IMAGE_NAME}"
    workdir="$(mktemp -d)"
    trap 'rm -rf "${workdir}"' EXIT

    if [ -z "${template_dir}" ] && [ -d "${default_template_dir}" ]; then
        template_dir="${default_template_dir}"
        bbnote "Using default artifact template dir: ${default_template_dir}"
    fi

    if [ ! -f "${lxc_bundle}" ]; then
        bbfatal "Expected LXC bundle not found: ${lxc_bundle}"
    fi
    if [ ! -f "${podman_archive}" ]; then
        bbfatal "Expected Podman archive not found: ${podman_archive}"
    fi

    copy_template_files() {
        artifact_dir="$1"

        if [ -z "${template_dir}" ]; then
            return 0
        fi
        if [ ! -d "${template_dir}" ]; then
            bbfatal "CONTAINER_ARTIFACT_TEMPLATE_DIR does not exist: ${template_dir}"
        fi

        if [ -d "${template_dir}/metadata" ]; then
            cp -a "${template_dir}/metadata" "${artifact_dir}/metadata"
        fi
    }

    json_escape() {
        printf '%s' "$1" | sed 's/\\/\\\\/g; s/"/\\"/g'
    }

    compose_podman_create_args() {
        base_args="$1"

        if [ -n "${CONTAINER_INIT_MANAGER}" ] && [ -n "${CONTAINER_INIT_SCRIPT}" ]; then
            entrypoint_json="[\"${CONTAINER_INIT_MANAGER}\",\"${CONTAINER_INIT_SCRIPT}\"]"
            if [ -n "${base_args}" ]; then
                printf "%s --entrypoint '%s'" "${base_args}" "${entrypoint_json}"
            else
                printf "%s" "--entrypoint '${entrypoint_json}'"
            fi
            return 0
        fi

        printf '%s' "${base_args}"
    }

    write_manifest() {
        artifact_dir="$1"
        runtime="$2"
        artifact_type="$3"
        payload_name="$4"
        digest="$5"
        size_bytes="$6"
        created_at="$7"
        create_args="$8"
        build_id="$9"

        package_id_esc="$(json_escape "${CONTAINER_PACKAGE_ID}")"
        version_esc="$(json_escape "${CONTAINER_ARTIFACT_VERSION}")"
        create_args_esc="$(json_escape "${create_args}")"
        fw_versions_esc="$(json_escape "${CONTAINER_FIRMWARE_VERSIONS}")"
        description_esc="$(json_escape "${CONTAINER_ARTIFACT_DESCRIPTION}")"
        build_id_esc="$(json_escape "${build_id}")"
        autostart_value="$(printf '%s' "${CONTAINER_AUTOSTART}" | tr '[:upper:]' '[:lower:]')"
        monitor_value="$(printf '%s' "${CONTAINER_MONITOR}" | tr '[:upper:]' '[:lower:]')"
        restart_enabled_value="$(printf '%s' "${CONTAINER_RESTART_ENABLED}" | tr '[:upper:]' '[:lower:]')"

        printf '%s\n' \
            '{' \
            "  \"package_id\": \"${package_id_esc}\"," \
            "  \"version\": \"${version_esc}\"," \
            "  \"runtime\": \"${runtime}\"," \
            "  \"artifact_type\": \"${artifact_type}\"," \
            > "${artifact_dir}/manifest.json"

        if [ -n "${create_args}" ]; then
            printf '%s\n' "  \"create_args\": \"${create_args_esc}\"," >> "${artifact_dir}/manifest.json"
        fi

        printf '%s\n' \
            '  "registration_defaults": {' \
            "    \"autostart\": ${autostart_value}," \
            "    \"monitor\": ${monitor_value}," \
            '    "restart": {' \
            "      \"enabled\": ${restart_enabled_value}," \
            "      \"max_retries\": ${CONTAINER_RESTART_MAX_RETRIES}," \
            "      \"window\": ${CONTAINER_RESTART_WINDOW}," \
            "      \"retry_delay\": ${CONTAINER_RESTART_RETRY_DELAY}" \
            '    }' \
            '  },' \
            >> "${artifact_dir}/manifest.json"

        printf '%s\n' \
            "  \"created_at\": \"${created_at}\"," \
            "  \"digest\": \"sha256:${digest}\"," \
            "  \"device_types\": ${CONTAINER_DEVICE_TYPES_JSON}," \
            "  \"firmware_versions\": \"${fw_versions_esc}\"," \
            "  \"size_bytes\": ${size_bytes}," \
            "  \"build_id\": \"${build_id_esc}\"," \
            "  \"description\": \"${description_esc}\"," \
            "  \"labels\": ${CONTAINER_ARTIFACT_LABELS_JSON}" \
            '}' \
            >> "${artifact_dir}/manifest.json"
    }

    create_artifact_bundle() {
        runtime="$1"
        src_payload="$2"
        payload_name="$3"
        output_name="$4"
        mode="$5"
        artifact_type="$6"
        create_args="$7"
        bundle_dir="${workdir}/bundle-${runtime}"
        artifact_dir="${bundle_dir}"

        rm -rf "${bundle_dir}"
        mkdir -p "${artifact_dir}/payload" "${artifact_dir}/checksums"

        copy_template_files "${artifact_dir}"

        if [ "${mode}" = "lxc" ]; then
            lxc_unpack="${workdir}/lxc-unpack"
            rm -rf "${lxc_unpack}"
            mkdir -p "${lxc_unpack}"
            tar -xJf "${src_payload}" -C "${lxc_unpack}"

            if [ ! -d "${lxc_unpack}/${CONTAINER_NAME}/rootfs" ] || [ ! -f "${lxc_unpack}/${CONTAINER_NAME}/config" ]; then
                bbfatal "LXC bundle does not contain expected ${CONTAINER_NAME}/rootfs and ${CONTAINER_NAME}/config"
            fi

            tar -C "${lxc_unpack}/${CONTAINER_NAME}" \
                -czf "${artifact_dir}/payload/${payload_name}" \
                rootfs config
        else
            cp "${src_payload}" "${artifact_dir}/payload/${payload_name}"
        fi

        digest="$(sha256sum "${artifact_dir}/payload/${payload_name}" | awk '{print $1}')"
        size_bytes="$(stat -c %s "${artifact_dir}/payload/${payload_name}")"
        created_at="$(date -u +%Y-%m-%dT%H:%M:%SZ)"
        if [ "${runtime}" = "podman" ]; then
            create_args="$(compose_podman_create_args "${create_args}")"
        fi
        build_id="${CONTAINER_ARTIFACT_BUILD_ID}"
        if [ -z "${build_id}" ]; then
            if command -v git >/dev/null 2>&1; then
                build_id="$(git -C "${META_DIGI_REPO_DIR}" rev-parse HEAD 2>/dev/null || true)"
            fi
        fi
        printf '%s  %s\n' "${digest}" "payload/${payload_name}" > "${artifact_dir}/checksums/sha256sums.txt"
        write_manifest "${artifact_dir}" "${runtime}" "${artifact_type}" "${payload_name}" "${digest}" "${size_bytes}" "${created_at}" "${create_args}" "${build_id}"

        tar -C "${bundle_dir}" -czf "${DEPLOY_DIR_IMAGE}/${output_name}" .

        if [ ! -s "${DEPLOY_DIR_IMAGE}/${output_name}" ]; then
            bbfatal "Container artifact bundle not generated correctly: ${DEPLOY_DIR_IMAGE}/${output_name}"
        fi

        bbnote "Container ${runtime} artifact ready: ${DEPLOY_DIR_IMAGE}/${output_name}"
    }

    create_artifact_bundle "lxc" "${lxc_bundle}" "rootfs_and_config.tgz" "${LXC_ARTIFACT_OUTPUT_NAME}" "lxc" "lxc-tgz" ""
    create_artifact_bundle "podman" "${podman_archive}" "image.tar" "${PODMAN_ARTIFACT_OUTPUT_NAME}" "podman" "podman-image-tar" "${CONTAINER_CREATE_ARGS_PODMAN}"

    bbnote "Removing intermediate container artifacts from this build..."
    rm -f "${lxc_bundle}" "${podman_archive}"
    rm -f "${image_prefix}.tar.xz" "${image_prefix}.rootfs-oci.tar" "${image_prefix}.rootfs-oci-dir.tar"
    rm -rf "${image_prefix}.rootfs-oci"
}

addtask image_container_artifacts after do_image_lxc_bundle do_image_podman_archive before do_build
