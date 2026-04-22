#!/usr/bin/env python3
#
# Copyright (C) 2026, Digi International Inc.
#
from __future__ import annotations

import argparse
import hashlib
import json
from datetime import datetime, timezone
import os
from pathlib import Path
import shutil
import sys
import tarfile
import tempfile

BASE36_ALPHABET = "0123456789abcdefghijklmnopqrstuvwxyz"
SUPPORTED_STATS_METRICS = (
    "uptime",
    "cpu",
    "mem",
    "net/rx",
    "net/tx",
    "disk/read",
    "disk/write",
)
SUPPORTED_STATS_METRICS_SET = set(SUPPORTED_STATS_METRICS)


def fail(message: str) -> "NoReturn":
    raise SystemExit(f"error: {message}")


def to_base36(value: int) -> str:
    if value < 0:
        fail("cannot convert negative values to base36")
    if value == 0:
        return "0"
    result: list[str] = []
    while value:
        value, remainder = divmod(value, 36)
        result.append(BASE36_ALPHABET[remainder])
    return "".join(reversed(result))


def format_created_at(timestamp: datetime) -> str:
    return timestamp.astimezone(timezone.utc).isoformat(timespec="milliseconds").replace("+00:00", "Z")


def build_generated_package_id(base_package_id: str, *, created_at_ms: int) -> str:
    return f"{base_package_id}-{to_base36(created_at_ms)}"


def load_manifest(path: Path) -> dict:
    try:
        data = json.loads(path.read_text(encoding="utf-8"))
    except FileNotFoundError:
        fail(f"manifest file not found: {path}")
    except json.JSONDecodeError as exc:
        fail(f"invalid manifest JSON: {exc}")
    except OSError as exc:
        fail(f"cannot read manifest file {path}: {exc}")
    if not isinstance(data, dict):
        fail("manifest must be a JSON object")
    return data


def validate_bool(container: dict, key: str, *, path: str) -> bool:
    value = container.get(key)
    if not isinstance(value, bool):
        fail(f"invalid manifest: {path}.{key} must be boolean")
    return value


def validate_int(container: dict, key: str, *, path: str, minimum: int) -> int:
    value = container.get(key)
    if isinstance(value, bool) or not isinstance(value, int):
        fail(f"invalid manifest: {path}.{key} must be integer")
    if value < minimum:
        fail(f"invalid manifest: {path}.{key} must be >= {minimum}")
    return value


def validate_string(container: dict, key: str, *, path: str, required: bool = True) -> str:
    value = container.get(key)
    if value is None and not required:
        return ""
    if not isinstance(value, str) or not value.strip():
        fail(f"invalid manifest: {path}.{key} must be a non-empty string")
    return value.strip()


def validate_device_types(value: object) -> list[str]:
    if not isinstance(value, list) or not value:
        fail("invalid manifest: device_types must be a non-empty list")
    result: list[str] = []
    for item in value:
        if not isinstance(item, str) or not item.strip():
            fail("invalid manifest: device_types entries must be non-empty strings")
        result.append(item.strip())
    return result


def validate_labels(value: object) -> dict:
    if value is None:
        return {}
    if not isinstance(value, dict):
        fail("invalid manifest: labels must be a JSON object")
    return value


def validate_string_list(value: object, *, path: str, allowed: set[str]) -> list[str]:
    if value is None:
        return []
    if not isinstance(value, list):
        fail(f"invalid manifest: {path} must be an array")
    result: list[str] = []
    seen: set[str] = set()
    for item in value:
        if not isinstance(item, str) or not item.strip():
            fail(f"invalid manifest: {path} entries must be non-empty strings")
        metric = item.strip()
        if metric not in allowed:
            fail(f"invalid manifest: {path} contains unsupported metric {metric}")
        if metric in seen:
            continue
        seen.add(metric)
        result.append(metric)
    if "all" in result and len(result) > 1:
        fail(f"invalid manifest: {path} cannot combine all with specific metrics")
    return result


def normalize_drm_stats_metrics(metrics: list[str]) -> list[str]:
    if "all" in metrics:
        return list(SUPPORTED_STATS_METRICS)
    return metrics


def validate_manifest(data: dict) -> dict:
    package_id = validate_string(data, "package_id", path="manifest")
    version = validate_string(data, "version", path="manifest")
    runtime = validate_string(data, "runtime", path="manifest")
    if runtime not in {"lxc", "podman"}:
        fail("invalid manifest: runtime must be one of: lxc, podman")
    device_types = validate_device_types(data.get("device_types"))

    registration_defaults = data.get("registration_defaults")
    if not isinstance(registration_defaults, dict):
        fail("invalid manifest: registration_defaults must be an object")
    restart = registration_defaults.get("restart")
    if not isinstance(restart, dict):
        fail("invalid manifest: registration_defaults.restart must be an object")
    drm = registration_defaults.get("drm", {})
    if not isinstance(drm, dict):
        fail("invalid manifest: registration_defaults.drm must be an object")

    create_args = data.get("create_args")
    if runtime == "podman":
        if not isinstance(create_args, str) or not create_args.strip():
            fail("invalid manifest: create_args is required for podman")
        create_args = create_args.strip()
    elif create_args is not None:
        fail("invalid manifest: create_args is not allowed for lxc")

    firmware_versions = data.get("firmware_versions", "")
    if firmware_versions is not None and not isinstance(firmware_versions, str):
        fail("invalid manifest: firmware_versions must be a string")

    build_id = data.get("build_id", "")
    if build_id is not None and not isinstance(build_id, str):
        fail("invalid manifest: build_id must be a string")

    description = data.get("description", "")
    if description is not None and not isinstance(description, str):
        fail("invalid manifest: description must be a string")
    name = data.get("name")
    if name is not None:
        if not isinstance(name, str) or not name.strip():
            fail("invalid manifest: name must be a non-empty string")
        name = name.strip()
    friendly_name = data.get("friendly_name")
    if friendly_name is not None:
        if not isinstance(friendly_name, str) or not friendly_name.strip():
            fail("invalid manifest: friendly_name must be a non-empty string")
        friendly_name = friendly_name.strip()

    validated = {
        "package_id": package_id,
        "name": name,
        "friendly_name": friendly_name,
        "version": version,
        "runtime": runtime,
        "create_args": create_args,
        "registration_defaults": {
            "autostart": validate_bool(registration_defaults, "autostart", path="registration_defaults"),
            "monitor": validate_bool(registration_defaults, "monitor", path="registration_defaults"),
            "drm": {
                "enabled": validate_bool(drm, "enabled", path="registration_defaults.drm"),
                "stats_sample_interval_s": validate_int(
                    drm,
                    "stats_sample_interval_s",
                    path="registration_defaults.drm",
                    minimum=1,
                ),
                "stats_list_of_metrics": validate_string_list(
                    drm.get("stats_list_of_metrics"),
                    path="registration_defaults.drm.stats_list_of_metrics",
                    allowed=SUPPORTED_STATS_METRICS_SET | {"all"},
                ),
            },
            "restart": {
                "enabled": validate_bool(restart, "enabled", path="registration_defaults.restart"),
                "max_retries": validate_int(restart, "max_retries", path="registration_defaults.restart", minimum=0),
                "window": validate_int(restart, "window", path="registration_defaults.restart", minimum=1),
                "retry_delay": validate_int(restart, "retry_delay", path="registration_defaults.restart", minimum=0),
            },
        },
        "device_types": device_types,
        "firmware_versions": firmware_versions or "",
        "build_id": build_id or "",
        "description": description or "",
        "labels": validate_labels(data.get("labels")),
    }
    validated["registration_defaults"]["drm"]["stats_list_of_metrics"] = normalize_drm_stats_metrics(
        validated["registration_defaults"]["drm"]["stats_list_of_metrics"]
    )
    return validated


def validate_payload(path: Path, runtime: str) -> None:
    if not path.exists():
        fail(f"payload file not found: {path}")
    if not path.is_file():
        fail(f"payload path is not a file: {path}")
    lowered = path.name.lower()
    if runtime == "podman":
        if not lowered.endswith(".tar"):
            fail("invalid payload: runtime podman requires a .tar file")
        return
    if not lowered.endswith(".tar.gz"):
        fail("invalid payload: runtime lxc requires a .tar.gz file")


def tar_mode_for_path(path: Path, *, writing: bool = False) -> str:
    lowered = path.name.lower()
    if lowered.endswith((".tgz", ".tar.gz")):
        return "w:gz" if writing else "r:gz"
    if lowered.endswith(".tar.xz"):
        return "w:xz" if writing else "r:xz"
    return "w:" if writing else "r:"


def sha256(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as handle:
        for chunk in iter(lambda: handle.read(1024 * 1024), b""):
            digest.update(chunk)
    return digest.hexdigest()


def build_output_name(*, package_id: str, runtime: str, device_type: str) -> str:
    return f"{package_id}_artifact_{runtime}_{device_type}.tar.gz"


def ensure_lxc_layout(payload: Path) -> tuple[str, str]:
    with tempfile.TemporaryDirectory(prefix="dcp-lxc-validate-") as tmpdir:
        root = Path(tmpdir)
        try:
            with tarfile.open(payload, tar_mode_for_path(payload)) as tar:
                tar.extractall(root)
        except (tarfile.TarError, OSError) as exc:
            fail(f"invalid payload: cannot unpack LXC archive {payload}: {exc}")

        if (root / "rootfs").is_dir() and (root / "config").is_file():
            return "", ""

        dirs = [item for item in root.iterdir() if item.is_dir()]
        if len(dirs) == 1 and (dirs[0] / "rootfs").is_dir() and (dirs[0] / "config").is_file():
            return dirs[0].name, dirs[0].name

    fail("invalid payload: LXC archive must contain rootfs/ and config")
def write_default_readme(
    path: Path,
    manifest: dict,
    *,
    package_id: str,
    created_at: str,
    payload_name: str,
) -> None:
    content = (
        f"Package: {package_id}\n"
        f"Version: {manifest['version']}\n"
        f"Runtime: {manifest['runtime']}\n"
        f"Platform: {manifest['device_types'][0]}\n"
        f"Payload: {payload_name}\n"
        f"Generated: {created_at}\n"
    )
    path.write_text(content, encoding="utf-8")


def write_default_changelog(path: Path) -> None:
    path.write_text("Initial package generated with external DCP tool.\n", encoding="utf-8")


def stage_metadata(src: Path | None, dst: Path, default_writer) -> None:
    if src is not None:
        try:
            shutil.copy2(src, dst)
        except OSError as exc:
            fail(f"cannot copy metadata file {src} to {dst}: {exc}")
    else:
        default_writer(dst)


def write_manifest(path: Path, manifest: dict) -> None:
    path.write_text(json.dumps(manifest, indent=2) + "\n", encoding="utf-8")


def build_final_manifest(
    base: dict,
    *,
    package_id: str,
    artifact_type: str,
    digest: str,
    size_bytes: int,
    created_at: str,
) -> dict:
    output = {
        "package_id": package_id,
        "name": base["name"],
        "friendly_name": base["friendly_name"],
        "version": base["version"],
        "runtime": base["runtime"],
        "artifact_type": artifact_type,
        "registration_defaults": base["registration_defaults"],
        "created_at": created_at,
        "digest": f"sha256:{digest}",
        "device_types": base["device_types"],
        "firmware_versions": base["firmware_versions"],
        "size_bytes": size_bytes,
        "build_id": base["build_id"],
        "description": base["description"],
        "labels": base["labels"],
    }
    if base["runtime"] == "podman":
        output["create_args"] = base["create_args"]
    return output


def pack_output(staging_root: Path, output_path: Path) -> None:
    try:
        with tarfile.open(output_path, "w:gz") as tar:
            for item in sorted(staging_root.iterdir()):
                tar.add(item, arcname=item.name)
    except OSError as exc:
        fail(f"cannot create output archive {output_path}: {exc}")


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Generate a Digi container DCP bundle from a manifest and a single payload artifact."
    )
    parser.add_argument("--manifest", required=True, help="Path to input manifest.json")
    parser.add_argument("--payload", required=True, help="Path to input payload file")
    parser.add_argument(
        "--output-dir",
        default=".",
        help="Optional output directory. Defaults to the current working directory.",
    )
    parser.add_argument("--readme", help="Optional README.txt to include under metadata/")
    parser.add_argument("--changelog", help="Optional changelog.txt to include under metadata/")
    return parser.parse_args()


def main() -> int:
    args = parse_args()

    manifest_path = Path(args.manifest)
    payload_path = Path(args.payload)
    output_dir = Path(args.output_dir)
    readme_path = Path(args.readme) if args.readme else None
    changelog_path = Path(args.changelog) if args.changelog else None

    manifest = validate_manifest(load_manifest(manifest_path))
    validate_payload(payload_path, manifest["runtime"])

    try:
        output_dir.mkdir(parents=True, exist_ok=True)
    except OSError as exc:
        fail(f"cannot create output directory {output_dir}: {exc}")

    created_at_dt = datetime.now(timezone.utc)
    created_at_ms = int(created_at_dt.timestamp() * 1000)
    created_at = format_created_at(created_at_dt)
    generated_package_id = build_generated_package_id(
        manifest["package_id"],
        created_at_ms=created_at_ms,
    )
    output_name = build_output_name(
        package_id=generated_package_id,
        runtime=manifest["runtime"],
        device_type=manifest["device_types"][0],
    )
    output_path = output_dir / output_name

    with tempfile.TemporaryDirectory(prefix="dcp-bundle-") as tmpdir:
        staging_root = Path(tmpdir)
        payload_dir = staging_root / "payload"
        checksums_dir = staging_root / "checksums"
        metadata_dir = staging_root / "metadata"
        payload_dir.mkdir(parents=True, exist_ok=True)
        checksums_dir.mkdir(parents=True, exist_ok=True)
        metadata_dir.mkdir(parents=True, exist_ok=True)

        if manifest["runtime"] == "podman":
            payload_name = "image.tar"
            artifact_type = "podman-image-tar"
            staged_payload = payload_dir / payload_name
            shutil.copy2(payload_path, staged_payload)
        else:
            ensure_lxc_layout(payload_path)
            payload_name = payload_path.name
            staged_payload = payload_dir / payload_name
            artifact_type = "lxc-bundle"
            shutil.copy2(payload_path, staged_payload)

        digest = sha256(staged_payload)
        size_bytes = staged_payload.stat().st_size

        final_manifest = build_final_manifest(
            manifest,
            package_id=generated_package_id,
            artifact_type=artifact_type,
            digest=digest,
            size_bytes=size_bytes,
            created_at=created_at,
        )
        write_manifest(staging_root / "manifest.json", final_manifest)

        stage_metadata(
            readme_path,
            metadata_dir / "README.txt",
            lambda dst: write_default_readme(
                dst,
                manifest,
                package_id=generated_package_id,
                created_at=created_at,
                payload_name=payload_name,
            ),
        )
        stage_metadata(changelog_path, metadata_dir / "changelog.txt", write_default_changelog)

        (checksums_dir / "sha256sums.txt").write_text(
            f"{digest}  payload/{payload_name}\n",
            encoding="utf-8",
        )

        pack_output(staging_root, output_path)

    print(output_path.resolve())
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
