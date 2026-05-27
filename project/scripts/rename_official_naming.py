#!/usr/bin/env python3
"""Rename project products from lighting_app to Matter official app/solution names."""

from __future__ import annotations

import os
import re
from pathlib import Path

PROJECT_ROOT = Path(__file__).resolve().parents[1]

# product_dir -> (old_app_dir, new_app_dir, old_slug, new_slug, slcp label suffix)
PRODUCTS = {
    "colortemperature_light": (
        "lighting_app",
        "colortemperature_light_app",
        "matter_thread_soc_lighting_app",
        "matter_thread_soc_colortemperature_light_app",
        "Color Temperature Light",
    ),
    "extended_color_light": (
        "lighting_app",
        "extended_color_light_app",
        "matter_thread_soc_lighting_app",
        "matter_thread_soc_extended_color_light_app",
        "Extended Color Light",
    ),
    "extended_color_light_strip": (
        "lighting_app",
        "extended_color_light_strip_app",
        "matter_thread_soc_lighting_app",
        "matter_thread_soc_extended_color_light_strip_app",
        "Extended Color Light Strip",
    ),
}

TEXT_SUFFIXES = {
    ".slcw", ".slpb", ".slcp", ".slps", ".cmake", ".txt", ".json", ".md",
    ".pintool", ".h", ".cpp", ".c", ".ld", ".icf",
}


def replace_in_file(path: Path, old_slug: str, new_slug: str, old_app: str, new_app: str, label: str) -> None:
    if path.suffix not in TEXT_SUFFIXES and path.name not in ("CMakePresets.json",):
        return
    try:
        text = path.read_text(encoding="utf-8")
    except (UnicodeDecodeError, OSError):
        return
    new_text = text.replace(old_slug, new_slug)
    new_text = new_text.replace(old_app, new_app)
    new_text = new_text.replace("Matter Thread - SoC Lighting", f"Matter Thread - SoC {label}")
    new_text = re.sub(
        r"Demonstrates a sample implementation of a Matter over Thread lighting",
        f"Demonstrates a sample implementation of a Matter over Thread {label.lower()}",
        new_text,
    )
    if new_text != text:
        path.write_text(new_text, encoding="utf-8")


def rename_paths(root: Path, old_slug: str, new_slug: str, old_app: str, new_app: str) -> None:
    """Rename files and directories (deepest first)."""
    paths: list[Path] = []
    for dirpath, dirnames, filenames in os.walk(root, topdown=False):
        base = Path(dirpath)
        for name in dirnames + filenames:
            p = base / name
            if old_slug in name or name == old_app:
                new_name = name.replace(old_slug, new_slug)
                if name == old_app:
                    new_name = new_app
                paths.append((p, p.parent / new_name))

    for old_p, new_p in paths:
        if old_p.exists() and old_p != new_p:
            if new_p.exists():
                raise SystemExit(f"target exists: {new_p}")
            old_p.rename(new_p)


def rename_product(product_dir: str, old_app: str, new_app: str, old_slug: str, new_slug: str, label: str) -> None:
    root = PROJECT_ROOT / product_dir
    if not root.is_dir():
        raise SystemExit(f"missing: {root}")

    print(f"[{product_dir}] {old_app} -> {new_app}, {old_slug} -> {new_slug}")

    for path in sorted(root.rglob("*")):
        if path.is_file():
            replace_in_file(path, old_slug, new_slug, old_app, new_app, label)

    app_path = root / old_app
    if app_path.is_dir():
        app_path.rename(root / new_app)

    rename_paths(root, old_slug, new_slug, old_app, new_app)


def main() -> None:
    for product_dir, cfg in PRODUCTS.items():
        rename_product(product_dir, *cfg)
    print("Rename done.")


if __name__ == "__main__":
    main()
