# meta-digi-containers

Yocto layer for Digi container-focused image generation and packaging.

This layer provides the `dey-image-container` image recipe and related logic to produce:

- A base rootfs (`tar.xz`)
- An OCI image output
- A Podman archive (`*.tar`)
- An LXC bundle (`*.tar.xz`)
- Final container artifacts (`*.tar.gz`) with:
  - `manifest.json`
  - `payload/`
  - `checksums/sha256sums.txt`
  - optional `metadata/`

Note: Podman archive generation requires an OCI image output as intermediate input.
The recipe keeps `oci` in `IMAGE_FSTYPES` because `do_image_podman_archive` converts
that OCI artifact into a `docker-archive` tar using `skopeo`.

## Layer Scope

Main recipe:

- `recipes-core/images/dey-image-container.bb`

Recipe includes:

- `dey-image-container-fragments.inc`
- `dey-image-container-lxc.inc`
- `dey-image-container-podman.inc`
- `dey-image-container-artifact.inc`

Container support files:

- `containers/<profile>/configs_lxc/` (profile LXC config fragments)
- `containers/<profile>/rootfs_files/` (profile rootfs overlays)
- `containers/<profile>/artifact/` (optional artifact metadata template)

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
CONTAINER_TYPE = "webkit"         # or: lvgl, base, custom profile
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

- `${CONTAINER_NAME}_artifact_podman_${MACHINE}.tar.gz`
- `${CONTAINER_NAME}_artifact_lxc_${MACHINE}.tar.gz`

Intermediate outputs generated during the build (LXC bundle, Podman archive, OCI/rootfs files)
are removed automatically at the end of artifact creation.

## Profiles

Profile-specific behavior is controlled with:

- `CONTAINER_TYPE`
- `OVERRIDES:append = ":container-${CONTAINER_TYPE}"` (handled in recipe)

Current built-in profile examples:

- `container-lvgl`
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

- `CONTAINER_PACKAGE_ID`
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
