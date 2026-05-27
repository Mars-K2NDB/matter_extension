#!/usr/bin/env python3
"""Re-apply pintool PORTIO routes after slc generate."""

from setup_light_projects import PRODUCTS, PROJECT_ROOT, patch_ct_dual_pintool, patch_rgbcw_pintool


def main() -> None:
    for name, product in PRODUCTS.items():
        app = PROJECT_ROOT / name / product["app_dir"]
        slcp = app / product["slcp"]
        pintool = app / slcp.name.replace(".slcp", ".pintool")
        if not pintool.is_file():
            continue
        if name == "extended_color_light":
            patch_rgbcw_pintool(pintool)
            print(f"patched rgbcw pintool: {pintool}")
        elif name == "colortemperature_light":
            patch_ct_dual_pintool(pintool)
            print(f"patched ct pintool: {pintool}")


if __name__ == "__main__":
    main()
