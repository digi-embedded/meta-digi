# meta-digi-containers

Yocto layer for Digi container-focused image generation and packaging.

This layer provides:

- `dey-image-dcp`: generates Digi Container Package (DCP) artifacts
- `dey-image-containers`: generates a minimal root file system with the Container
Manager and everything it requires to run and manage containers

`cc-container-mng` depends on the container runtime packages listed in
`CONTAINERS_BACKEND_TOOLS`, which defaults to `podman lxc`. Override it to select
different runtimes or a single engine, for example:

```bitbake
CONTAINERS_BACKEND_TOOLS = "podman-trimmed lxc-trimmed"
CONTAINERS_BACKEND_TOOLS = "podman-trimmed"
CONTAINERS_BACKEND_TOOLS = "lxc"
```

`dey-image-containers` overrides `CONTAINERS_BACKEND_TOOLS` to install
the trimmed runtime packages to keep the image smaller.

The layer explicitly depends on `meta-virtualization`.
`dey-image-dcp` and`dey-image-containers` requires
`DISTRO_FEATURES:append = " virtualization"` in `local.conf`.

The `dey-image-dcp` workflow produces:

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
- If the input manifest omits `package_id`, the generator derives the final
  `package_id` from `name` and appends a unique suffix using the `created_at`
  timestamp encoded in base36 milliseconds.
- If the input manifest provides `package_id`, that value is kept unchanged.
- The output file name is always derived from the generated package ID and
  manifest fields as:
  - `<generated_package_id>-<runtime>-<device_types[0]>.tar.gz`
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

- `./flutter-demo-<base36_created_at_ms>-podman-ccmp25-dvk.tar.gz`

In those manifests:

- `name` is the stable logical container name stored on the target.
- `friendly_name` is the user-facing label shown by the manager output when available.
- the final `package_id` is generated automatically from `name` unless the
  input manifest provides an explicit `package_id`
- `registration_defaults` is limited to local manager policy such as `autostart`,
  `monitor`, and `restart`; this release does not generate DRM-specific defaults

`dey-image-containers` also overrides the manager persistent base path
through `CC_CONTAINER_PATH`, which defaults to `${ROOT_HOME}/cc-container` in
the Digi platform defaults. This makes the effective target paths live under
`/root/cc-container` in DEY images, while the upstream `cc-container-mng`
project keeps `/opt/cc-container` as its built-in default.

## Layer Scope

Main recipes:

- `recipes-core/images/dey-image-dcp.bb`
- `recipes-core/images/dey-image-containers.bb`

Recipe includes:

- `dey-image-dcp-fragments.inc`
- `dey-image-dcp-lxc.inc`
- `dey-image-dcp-podman.inc`
- `dey-image-dcp-artifact.inc`

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
DCP_NAME = "webkit-demo"          # or: lvgl-demo, chromium-demo, flutter-demo, custom profile
# PODMAN_TAG defaults to "${DCP_NAME}-tag"
```

If `DCP_NAME` is not set, `dey-image-dcp` now defaults to `lvgl-demo`.

Build:

```bash
bitbake dey-image-dcp
```

Outputs are generated in:

- `tmp/deploy/images/${MACHINE}/`

Final outputs:

- `${DCP_NAME}-<base36_created_at_ms>-podman-<device_types[0]>.tar.gz`
- `${DCP_NAME}-<base36_created_at_ms>-lxc-<device_types[0]>.tar.gz`

Notes:

- The generator script derives `package_id` from the input `name` only when the
  manifest does not provide one explicitly. In that default case it appends a
  base36-encoded millisecond timestamp suffix, stores the generated `package_id`
  in the final `manifest.json`, and uses it in the DCP file name.
- In Yocto builds, `dey-image-dcp` keeps that unique-name behavior and
  removes older DCP artifacts with the same `${DCP_NAME}` prefix before
  generating the new one, so the deploy directory does not keep accumulating
  previous builds of the same container/runtime.

Intermediate rootfs and OCI outputs are kept available for incremental rebuilds.
The LXC bundle and Podman archive are generated as temporary payloads while
creating the final DCP artifacts, then removed at the end of artifact creation.

## Profiles

Profile-specific behavior is controlled with:

- `DCP_NAME`
- `OVERRIDES:append = ":container-${DCP_NAME}"` (handled in recipe)

Current built-in profile examples:

- `container-lvgl-demo`
- `container-flutter-demo`
- `container-webkit-demo`
- `container-chromium-demo`

You can add new profiles by appending variables with `:container-<name>` overrides.

For customer-defined profiles, use:

```conf
DCP_NAME = "myprofile"
PODMAN_TAG = "myprofile-tag"

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
  and `containers/${DCP_NAME}/rootfs_files` exists,
  it is used automatically.

## LXC Fragment Configuration

LXC fragments are loaded from:

- `containers/${DCP_NAME}/configs_lxc/`

Config file naming:

- `config_lxc_<machine>`

Example:

- `config_lxc_ccmp25-dvk`

`<machine>` matches the full `MACHINE` value.

Supported placeholders in LXC config fragments:

- `@LXC_ARCH@`
- `@LXC_FOLDER@`
- `@DCP_NAME@`
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

- `DCP_NAME`
- `CONTAINER_FRIENDLY_NAME`
- `CONTAINER_ARTIFACT_VERSION`
- `CONTAINER_CREATE_ARGS_PODMAN`
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

If `CONTAINER_ARTIFACT_TEMPLATE_DIR` is not set and `containers/${DCP_NAME}/artifact` exists,
it is used automatically.

## Container Folder Layout

Each profile is self-contained under `containers/`:

```text
containers/
  lvgl-demo/
    rootfs_files/
    configs_lxc/
    artifact/
  chromium-demo/
    rootfs_files/
    configs_lxc/
    artifact/
  flutter-demo/
    rootfs_files/
    configs_lxc/
    artifact/
  webkit-demo/
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
3. Set `DCP_NAME = "<profile>"` in `local.conf`.
4. Add profile packages with `IMAGE_INSTALL:append:container-<profile> = " ... "`.

## Notes

- Intermediate container artifacts from the current build are removed at the end of the artifact task.
- If `dey-image-dcp` or `dey-image-containers` is not found, verify the layer is present in `BBLAYERS`
and `DISTRO_FEATURES:append = " virtualization"` is defined in `local.conf`.
