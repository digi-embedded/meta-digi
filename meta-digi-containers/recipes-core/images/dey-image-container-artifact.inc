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
    generator_script="${THISDIR}/../../scripts/generate-dcp.py"
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

    write_input_manifest() {
        runtime="$1"
        manifest_path="$2"
        create_args="$3"
        build_id="$4"
        PACKAGE_ID="${CONTAINER_PACKAGE_ID}" \
        VERSION="${CONTAINER_ARTIFACT_VERSION}" \
        RUNTIME="${runtime}" \
        CREATE_ARGS="${create_args}" \
        AUTOSTART="${CONTAINER_AUTOSTART}" \
        MONITOR="${CONTAINER_MONITOR}" \
        STATS_PUBLISH_ENABLED="${CONTAINER_STATS_PUBLISH_ENABLED}" \
        STATS_PUBLISH_SAMPLE_INTERVAL="${CONTAINER_STATS_PUBLISH_SAMPLE_INTERVAL}" \
        RESTART_ENABLED="${CONTAINER_RESTART_ENABLED}" \
        RESTART_MAX_RETRIES="${CONTAINER_RESTART_MAX_RETRIES}" \
        RESTART_WINDOW="${CONTAINER_RESTART_WINDOW}" \
        RESTART_RETRY_DELAY="${CONTAINER_RESTART_RETRY_DELAY}" \
        DEVICE_TYPES_JSON="${CONTAINER_DEVICE_TYPES_JSON}" \
        FIRMWARE_VERSIONS="${CONTAINER_FIRMWARE_VERSIONS}" \
        BUILD_ID="${build_id}" \
        DESCRIPTION="${CONTAINER_ARTIFACT_DESCRIPTION}" \
        LABELS_JSON="${CONTAINER_ARTIFACT_LABELS_JSON}" \
        python3 -c 'import json, os, sys; \
parse_bool = lambda name: os.environ[name].strip().lower() == "true"; \
payload = {"package_id": os.environ["PACKAGE_ID"], "version": os.environ["VERSION"], "runtime": os.environ["RUNTIME"], "registration_defaults": {"autostart": parse_bool("AUTOSTART"), "monitor": parse_bool("MONITOR"), "stats_publish": {"enabled": parse_bool("STATS_PUBLISH_ENABLED"), "sample_interval_s": int(os.environ["STATS_PUBLISH_SAMPLE_INTERVAL"])}, "restart": {"enabled": parse_bool("RESTART_ENABLED"), "max_retries": int(os.environ["RESTART_MAX_RETRIES"]), "window": int(os.environ["RESTART_WINDOW"]), "retry_delay": int(os.environ["RESTART_RETRY_DELAY"]) } }, "device_types": json.loads(os.environ["DEVICE_TYPES_JSON"]), "firmware_versions": os.environ["FIRMWARE_VERSIONS"], "build_id": os.environ["BUILD_ID"], "description": os.environ["DESCRIPTION"], "labels": json.loads(os.environ["LABELS_JSON"])}; \
create_args = os.environ["CREATE_ARGS"].strip(); \
payload.update({"create_args": create_args} if create_args else {}); \
open(sys.argv[1], "w", encoding="utf-8").write(json.dumps(payload, indent=2) + "\n")' \
        "${manifest_path}"
    }

    create_artifact_bundle() {
        runtime="$1"
        src_payload="$2"
        create_args="$3"
        manifest_path="${workdir}/${runtime}-manifest.json"
        readme_arg=""
        changelog_arg=""
        primary_device_type="$(DEVICE_TYPES_JSON="${CONTAINER_DEVICE_TYPES_JSON}" python3 -c 'import json, os; data = json.loads(os.environ["DEVICE_TYPES_JSON"]); print(data[0])' 2>/dev/null)"

        if [ "${runtime}" = "podman" ]; then
            create_args="$(compose_podman_create_args "${create_args}")"
        fi

        if [ -z "${primary_device_type}" ]; then
            bbfatal "Unable to determine primary device type from CONTAINER_DEVICE_TYPES_JSON=${CONTAINER_DEVICE_TYPES_JSON}"
        fi

        build_id="${CONTAINER_ARTIFACT_BUILD_ID}"
        if [ -z "${build_id}" ]; then
            if command -v git >/dev/null 2>&1; then
                build_id="$(git -C "${META_DIGI_REPO_DIR}" rev-parse HEAD 2>/dev/null || true)"
            fi
        fi

        write_input_manifest "${runtime}" "${manifest_path}" "${create_args}" "${build_id}"

        if [ -n "${template_dir}" ] && [ ! -d "${template_dir}" ]; then
            bbfatal "CONTAINER_ARTIFACT_TEMPLATE_DIR does not exist: ${template_dir}"
        fi
        if [ -f "${template_dir}/metadata/README.txt" ]; then
            readme_arg="--readme ${template_dir}/metadata/README.txt"
        fi
        if [ -f "${template_dir}/metadata/changelog.txt" ]; then
            changelog_arg="--changelog ${template_dir}/metadata/changelog.txt"
        fi

        generated_output="$(python3 "${generator_script}" \
            --manifest "${manifest_path}" \
            --payload "${src_payload}" \
            --output-dir "${DEPLOY_DIR_IMAGE}" \
            ${readme_arg} \
            ${changelog_arg})"

        generated_output="$(printf '%s' "${generated_output}" | tail -n 1)"
        if [ ! -s "${generated_output}" ]; then
            bbfatal "Container artifact bundle not generated correctly: ${generated_output}"
        fi

        bbnote "Container ${runtime} artifact ready: ${generated_output}"
    }

    if [ ! -f "${generator_script}" ]; then
        bbfatal "Expected DCP generator script not found: ${generator_script}"
    fi

    create_artifact_bundle "lxc" "${lxc_bundle}" ""
    create_artifact_bundle "podman" "${podman_archive}" "${CONTAINER_CREATE_ARGS_PODMAN}"

    bbnote "Removing intermediate container artifacts from this build..."
    rm -f "${lxc_bundle}" "${podman_archive}"
    rm -f "${image_prefix}.tar.xz" "${image_prefix}.rootfs-oci.tar" "${image_prefix}.rootfs-oci-dir.tar"
    rm -rf "${image_prefix}.rootfs-oci"
}

addtask image_container_artifacts after do_image_lxc_bundle do_image_podman_archive before do_build
