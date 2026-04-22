# meta-digi-containers

Yocto layer for Digi container-focused image generation and packaging.

This layer provides:

- `dey-image-container` to generate container artifacts
- `dey-image-container-manager` to run and manage Podman/LXC containers on target

`dey-image-container-manager` installs dedicated `lxc-trimmed` and `podman-trimmed`
recipes, so it does not require `DISTROOVERRIDES` changes in `local.conf`
and does not affect other DEY images built in the same environment.

The layer explicitly depends on `meta-virtualization`, and
`dey-image-container-manager`
requires `DISTRO_FEATURES:append = " virtualization"` in `local.conf`.

The `dey-image-container` workflow produces:

- A base rootfs (`tar.xz`)
- An OCI image output
- A Podman archive (`*.tar`)
- An LXC bundle (`*.tar.gz`)
- Final container artifacts (`*.tar.gz`) with:
  - `manifest.json`
  - `payload/`
  - `checksums/sha256sums.txt`
  - optional `metadata/`

Note: Podman archive generation requires an OCI image output as intermediate input.
The recipe keeps `oci` in `IMAGE_FSTYPES` because `do_image_podman_archive` converts
that OCI artifact into a `docker-archive` tar using `skopeo`.

## External Digi Container Package (DCP) generation

Use the following Python script to generate a DCP out of Yocto workspace:

- `meta-digi-containers/scripts/generate-dcp.py`

This script requires:

- `manifest.json`
- payload artifact, which may be one of:
  - Podman: `image.tar`
  - LXC: a Yocto-style LXC bundle (`.tar.gz`) containing `rootfs/` and `config`

Usage:

```bash
python3 meta-digi-containers/scripts/generate-dcp.py \
  --manifest /path/to/manifest.json \
  --payload /path/to/payload \
  [--output-dir /path/to/outdir] \
  [--readme /path/to/README.txt] \
  [--changelog /path/to/changelog.txt]
```

and generates a final DCP bundle with the same layout used today by the Yocto recipe:

- `manifest.json`
- `payload/`
- `checksums/sha256sums.txt`
- `metadata/README.txt`
- `metadata/changelog.txt`

The script generates exactly one runtime artifact per execution.

Notes:

- `--output-dir` is optional. If omitted, the script writes the DCP to the current directory.
- The generator appends a unique suffix to the input `package_id` using the
  `created_at` timestamp encoded in base36 milliseconds.
- The output file name is always derived from the generated package ID and
  manifest fields as:
  - `<generated_package_id>_artifact_<runtime>_<device_types[0]>.tar.gz`
- For Podman payloads, the final internal payload name is always `payload/image.tar`
- For LXC payloads, the generator requires a Yocto-style `.tar.gz` bundle, validates its layout,
  and stores it inside the DCP using the original file name and format without re-packaging it.

Example manifests are provided at:

- `meta-digi-containers/examples/manifest-lxc.json`
- `meta-digi-containers/examples/manifest-podman.json`

Example:

```bash
python3 meta-digi-containers/scripts/generate-dcp.py \
  --manifest meta-digi-containers/examples/manifest-podman.json \
  --payload /tmp/image.tar
```

This generates a bundle named like:

- `./flutter-demo-<base36_created_at_ms>_artifact_podman_ccmp25-dvk.tar.gz`

In those manifests:

- `package_id` is the base identifier used to derive the final unique DCP package ID.
- `name` is the stable logical container name stored on the target.
- `friendly_name` is the user-facing label shown by DRM and the manager output when available.

## Digi Remote Manager metrics support

`dey-image-container-manager` includes `cc-container-mng` that has the capability to
publish container statistics through the local CCCS Python API.
For generated DCPs, DRM behavior is controlled through `registration_defaults.drm`
in the artifact manifest.
Those manifest defaults establish the initial runtime policy on the target.
After installation, mutable policy such as `autostart`, `monitor`, `restart`, and
`drm` can be inspected or updated through the container manager `config`
`get` and `set` operations without regenerating the DCP.
The image recipe generates the DCP automatically from the following variables:

- `CONTAINER_DRM_ENABLED`
- `CONTAINER_DRM_STATS_SAMPLE_INTERVAL`
- `CONTAINER_DRM_STATS_LIST_OF_METRICS`

Example:

```conf
CONTAINER_DRM_ENABLED = "true"
CONTAINER_DRM_STATS_SAMPLE_INTERVAL = "30"
CONTAINER_DRM_STATS_LIST_OF_METRICS = "[\"cpu\", \"mem\"]"
```

`CONTAINER_DRM_STATS_LIST_OF_METRICS` follows the same semantics as the target
manager:

- `["all"]` is accepted as a shorthand input for all periodic metrics.
- `[]` publishes no periodic metrics.
- any other list publishes only the selected metrics.

Generated manifests and target-side effective configuration use the explicit
metric list.

`dey-image-container-manager` also overrides the manager persistent base path
through `CC_CONTAINER_PATH`, which defaults to `${ROOT_HOME}/cc-container` in
the Digi platform defaults. This makes the effective target paths live under
`/root/cc-container` in DEY images, while the upstream `cc-container-mng`
project keeps `/opt/cc-container` as its built-in default.

## Layer Scope

Main recipes:

- `recipes-core/images/dey-image-container.bb`
- `recipes-core/images/dey-image-container-manager.bb`

Recipe includes:

- `dey-image-container-fragments.inc`
- `dey-image-container-lxc.inc`
- `dey-image-container-podman.inc`
- `dey-image-container-artifact.inc`

Container support files:

- `containers/<profile>/configs_lxc/` (profile LXC config fragments)
- `containers/<profile>/rootfs_files/` (profile rootfs overlays)
- `containers/<profile>/artifact/` (optional artifact metadata template)

Container runtime recipes:

- `recipes-containers/lxc/lxc-trimmed_git.bb`
- `recipes-containers/podman/podman-trimmed_git.bb`

## Add The Layer

In your build environment:

```bash
bitbake-layers add-layer /path/to/sources/meta-digi/meta-digi-containers
bitbake-layers add-layer /path/to/sources/meta-virtualization
bitbake-layers add-layer /path/to/sources/meta-openembedded/meta-filesystems
```

Or add it manually to `conf/bblayers.conf`.

## Basic Usage

Set profile and naming in `conf/local.conf`:

```conf
DISTRO_FEATURES:append = " virtualization"
CONTAINER_TYPE = "webkit"         # or: lvgl, flutter, base, custom profile
CONTAINER_NAME = "webkit-example"
# PODMAN_TAG defaults to "${CONTAINER_NAME}-tag"
```

Build:

```bash
bitbake dey-image-container
```

Outputs are generated in:

- `tmp/deploy/images/${MACHINE}/`

Final outputs:

- `${CONTAINER_PACKAGE_ID}-<base36_created_at_ms>_artifact_podman_<device_types[0]>.tar.gz`
- `${CONTAINER_PACKAGE_ID}-<base36_created_at_ms>_artifact_lxc_<device_types[0]>.tar.gz`

Notes:

- The generator script appends a base36-encoded millisecond timestamp suffix to
  the input `package_id`, stores that generated value in the final
  `manifest.json`, and uses it in the DCP file name.
- In Yocto builds, `CONTAINER_PACKAGE_ID` defaults to `${CONTAINER_NAME}` before
  the generator adds the unique suffix.

Intermediate outputs generated during the build (LXC bundle, Podman archive, OCI/rootfs files)
are removed automatically at the end of artifact creation.

## Profiles

Profile-specific behavior is controlled with:

- `CONTAINER_TYPE`
- `OVERRIDES:append = ":container-${CONTAINER_TYPE}"` (handled in recipe)

Current built-in profile examples:

- `container-lvgl`
- `container-flutter`
- `container-webkit`

You can add new profiles by appending variables with `:container-<name>` overrides.

For customer-defined profiles, use:

```conf
CONTAINER_TYPE = "myprofile"
CONTAINER_NAME = "myprofile-demo"
PODMAN_TAG = "myprofile-demo-tag"

IMAGE_INSTALL:append:container-myprofile = " package-a package-b"
CONTAINER_INIT_MANAGER:container-myprofile = "/usr/bin/docker-init"
CONTAINER_INIT_SCRIPT:container-myprofile = "/usr/bin/my-app --foreground"

# Manifest-related overrides can also be profile-specific:
CONTAINER_ARTIFACT_VERSION:container-myprofile = "1.2.0"
CONTAINER_FIRMWARE_VERSIONS:container-myprofile = ">=25.01"
CONTAINER_DEVICE_TYPES_JSON:container-myprofile = "[\"ccmp25-dvk\"]"
CONTAINER_ARTIFACT_DESCRIPTION:container-myprofile = "My profile demo container"
CONTAINER_ARTIFACT_LABELS_JSON:container-myprofile = "{\"vendor\":\"digi\",\"component\":\"demo\"}"

```

## Rootfs Overlay

Use the following variables to inject directories into the container rootfs:

```conf
CONTAINER_ROOTFS_OVERLAY_DIRS = "/absolute/path/to/overlay"
CONTAINER_ROOTFS_OVERLAY_TARBALLS = "/absolute/path/to/overlay.tar.gz"
```

Notes:

- Multiple entries are supported (space-separated).
- `*.sh` files copied from overlay directories are marked executable automatically.
- If `CONTAINER_ROOTFS_OVERLAY_DIRS` is not set
  and `containers/${CONTAINER_TYPE}/rootfs_files` exists,
  it is used automatically.

## LXC Fragment Configuration

LXC fragments are loaded from:

- `containers/${CONTAINER_TYPE}/configs_lxc/`

Config file naming:

- `config_lxc_<machine>`

Example:

- `config_lxc_ccmp25-dvk`

`<machine>` matches the full `MACHINE` value.

Supported placeholders in LXC config fragments:

- `@LXC_ARCH@`
- `@LXC_FOLDER@`
- `@CONTAINER_NAME@`
- `@CONTAINER_INIT_MANAGER@`
- `@CONTAINER_INIT_SCRIPT@`

## Artifact Manifest Fields

The artifact manifest is generated automatically and includes:

- `package_id`
- `name` [stable logical container name]
- `friendly_name` [optional user-facing display name]
- `version`
- `runtime`
- `artifact_type`
- `create_args` [optional] for Podman artifacts (only when non-empty)
- `created_at`
- `digest`
- `device_types`
- `firmware_versions`
- `size_bytes`
- `build_id`
- `description`
- `labels`

`build_id` behavior:

- Uses `CONTAINER_ARTIFACT_BUILD_ID` if explicitly set.
- Otherwise uses the current `meta-digi` git commit SHA (`git rev-parse HEAD`).

Relevant variables:

- `CONTAINER_NAME`
- `CONTAINER_PACKAGE_ID`
- `CONTAINER_FRIENDLY_NAME`
- `CONTAINER_ARTIFACT_VERSION`
- `CONTAINER_CREATE_ARGS_PODMAN`
- `CONTAINER_DRM_ENABLED`
- `CONTAINER_DRM_STATS_SAMPLE_INTERVAL`
- `CONTAINER_DRM_STATS_LIST_OF_METRICS`
- `CONTAINER_FIRMWARE_VERSIONS`
- `CONTAINER_DEVICE_TYPES_JSON`
- `CONTAINER_ARTIFACT_DESCRIPTION`
- `CONTAINER_ARTIFACT_BUILD_ID`
- `CONTAINER_ARTIFACT_LABELS_JSON`

## Optional Artifact Metadata Template

If you set:

```conf
CONTAINER_ARTIFACT_TEMPLATE_DIR = "/path/to/artifact-template"
```

and the template contains `metadata/`, it is copied into final artifact bundles.

`manifest.json`, `payload/*`, and `checksums/sha256sums.txt` are always generated during
build time and should not be pre-created in the template.

If `CONTAINER_ARTIFACT_TEMPLATE_DIR` is not set and `containers/${CONTAINER_TYPE}/artifact` exists,
it is used automatically.

## Container Folder Layout

Each profile is self-contained under `containers/`:

```text
containers/
  lvgl/
    rootfs_files/
    configs_lxc/
    artifact/
  flutter/
    rootfs_files/
    configs_lxc/
    artifact/
  webkit/
    rootfs_files/
    configs_lxc/
    artifact/
  custom/
    rootfs_files/
    configs_lxc/
    artifact/
```

To create a new profile:

1. Create `containers/<profile>/configs_lxc/config_lxc_<machine>`.
2. Optionally add `containers/<profile>/rootfs_files/` for rootfs content.
3. Set `CONTAINER_TYPE = "<profile>"` in `local.conf`.
4. Add profile packages with `IMAGE_INSTALL:append:container-<profile> = " ... "`.

## Notes

- Intermediate container artifacts from the current build are removed at the end of the artifact task.
- If `dey-image-container` is not found, verify the layer is present in `BBLAYERS`.
