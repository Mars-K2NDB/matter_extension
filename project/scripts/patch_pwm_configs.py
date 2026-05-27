#!/usr/bin/env python3
"""Re-apply PWM pin_tool blocks after slc generate (keeps OUTPUT_PORT/PIN).

Skips products with empty pwm_pins (e.g. colortemperature_light: Pin Tool owns PWM headers).
"""

from setup_light_projects import PRODUCTS, PROJECT_ROOT, patch_pwm_config


def main() -> None:
    for name, product in PRODUCTS.items():
        if not product["pwm_pins"]:
            continue
        app = PROJECT_ROOT / name / product["app_dir"] / "config"
        for pwm, spec in product["pwm_pins"].items():
            cfg = app / f"sl_pwm_init_{pwm}_config.h"
            if cfg.is_file():
                patch_pwm_config(cfg, spec)
                print(f"patched {cfg}")


if __name__ == "__main__":
    main()
