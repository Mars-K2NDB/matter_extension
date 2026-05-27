#!/usr/bin/env python3
"""Migrate lighting_mot config/src into project/* and apply per-product PWM/ZAP."""

from __future__ import annotations

import re
import shutil
from pathlib import Path

REPO = Path(__file__).resolve().parents[2]
REF_APP = REPO / "lighting_mot" / "lighting_app"
REF_BOOT = REPO / "lighting_mot" / "matter_bootloader"
PROJECT_ROOT = Path(__file__).resolve().parents[1]
SCRIPT_DIR = Path(__file__).resolve().parent
OVERLAYS = SCRIPT_DIR / "light_products"
CHEF_ZAP = REPO / "third_party" / "matter_sdk" / "examples" / "chef" / "devices"

PORT_C = "SL_GPIO_PORT_C"
PORT_B = "SL_GPIO_PORT_B"

PRODUCTS = {
    "dimmable_light": {
        "label": "Matter Thread Dimmable Light (PB4)",
        "app_dir": "dimmable_light_app",
        "slcp": "matter_thread_soc_dimmable_light_app_freertos.slcp",
        "zap_src": CHEF_ZAP / "rootnode_dimmablelight_bCwGYSDpoe.zap",
        "zap_name": "dimmable-thread-app.zap",
        "pwm_instances": ["pwm0"],
        "pwm_pins": {
            "pwm0": (PORT_B, 4, "TIMER4", 4, 0),
        },
    },
    "colortemperature_light": {
        "label": "Matter Thread Color Temperature Light (TIMER0/PB4冷, TIMER1/PB5暖)",
        "app_dir": "colortemperature_light_app",
        "slcp": "matter_thread_soc_colortemperature_light_app_freertos.slcp",
        "zap_src": CHEF_ZAP / "rootnode_colortemperaturelight_hbUnzYVeyn.zap",
        "zap_name": "colortemperature-thread-app.zap",
        "pwm_instances": ["pwm0", "pwm1"],
        # Pin Tool owns sl_pwm_init_pwm*_config.h; do not patch OUTPUT_PORT/PIN here.
        "pwm_pins": {},
    },
    "extended_color_light": {
        "label": "Matter Thread Extended Color Light (PC0-2/PB4-5)",
        "app_dir": "extended_color_light_app",
        "slcp": "matter_thread_soc_extended_color_light_app_freertos.slcp",
        "zap_src": CHEF_ZAP / "rootnode_extendedcolorlight_8lcaaYJVAa.zap",
        "zap_name": "extended-color-thread-app.zap",
        "pwm_instances": ["pwm0", "pwm1", "pwm2", "pwm3", "pwm4"],
        "pwm_pins": {
            "pwm0": (PORT_C, 0, "TIMER0", 0, 0),
            "pwm1": (PORT_C, 1, "TIMER1", 1, 0),
            "pwm2": (PORT_C, 2, "TIMER2", 2, 0),
            "pwm3": (PORT_B, 4, "TIMER4", 4, 0),
            "pwm4": (PORT_B, 5, "TIMER4", 4, 1),
        },
    },
    "extended_color_light_strip": {
        "label": "Matter Thread Extended Color Light Strip (SPI)",
        "app_dir": "extended_color_light_strip_app",
        "slcp": "matter_thread_soc_extended_color_light_strip_app_freertos.slcp",
        "zap_src": CHEF_ZAP / "rootnode_extendedcolorlight_8lcaaYJVAa.zap",
        "zap_name": "extended-color-thread-app.zap",
        "pwm_instances": [],
        "pwm_pins": {},
    },
}


def copy_tree(src: Path, dst: Path) -> None:
    if dst.exists():
        shutil.rmtree(dst)
    shutil.copytree(src, dst)


def fix_zap_paths(zap_path: Path) -> None:
    text = zap_path.read_text(encoding="utf-8")
    replacements = [
        (
            '"path": "../../../src/app/zap-templates/zcl/zcl.json"',
            '"path": "../../../../../../third_party/matter_sdk/src/app/zap-templates/zcl/zcl.json"',
        ),
        (
            '"path": "../../../src/app/zap-templates/app-templates.json"',
            '"path": "../../../../../../third_party/matter_sdk/src/app/zap-templates/app-templates.json"',
        ),
        (
            '"path": "..\\..\\..\\..\\src\\app\\zap-templates\\zcl\\zcl.json"',
            '"path": "..\\..\\..\\..\\..\\..\\..\\third_party\\matter_sdk\\src\\app\\zap-templates\\zcl\\zcl.json"',
        ),
        (
            '"path": "..\\..\\..\\..\\src\\app\\zap-templates\\app-templates.json"',
            '"path": "..\\..\\..\\..\\..\\..\\..\\third_party\\matter_sdk\\src\\app\\zap-templates\\app-templates.json"',
        ),
    ]
    for old, new in replacements:
        text = text.replace(old, new)
    zap_path.write_text(text, encoding="utf-8")


def patch_rgbcw_pintool(pintool: Path) -> None:
    """TIMER0/1/2 on PC0-2; TIMER4 CC0/CC1 on PB4/PB5 for RGBCW PWM."""
    text = pintool.read_text(encoding="utf-8")
    if "portio.timer0.enable.cc0" not in text:
        block = (
            '    <property object="PORTIO" propertyId="portio.timer0.enable.cc0" value="Enabled"/>\n'
            '    <property object="PORTIO" propertyId="portio.timer0.location.cc0" value="32"/>\n'
            '    <property object="PORTIO" propertyId="portio.timer1.enable.cc0" value="Enabled"/>\n'
            '    <property object="PORTIO" propertyId="portio.timer1.location.cc0" value="33"/>\n'
            '    <property object="PORTIO" propertyId="portio.timer2.enable.cc0" value="Enabled"/>\n'
            '    <property object="PORTIO" propertyId="portio.timer2.location.cc0" value="34"/>\n'
        )
        text = text.replace(
            '    <property object="PORTIO" propertyId="portio.pti.location.dout" value="38"/>\n',
            '    <property object="PORTIO" propertyId="portio.pti.location.dout" value="38"/>\n' + block,
            1,
        )
    if "portio.timer2.location.cc0" not in text:
        text = text.replace(
            '    <property object="PORTIO" propertyId="portio.timer2.enable.cc0" value="Enabled"/>\n',
            '    <property object="PORTIO" propertyId="portio.timer2.enable.cc0" value="Enabled"/>\n'
            '    <property object="PORTIO" propertyId="portio.timer2.location.cc0" value="34"/>\n',
            1,
        )
    old = '<property object="TIMER4" propertyId="channel.1.name" value=""/>'
    new = '<property object="TIMER4" propertyId="channel.1.name" value="OUTPUT"/>'
    if old in text:
        text = text.replace(old, new, 1)
    pintool.write_text(text, encoding="utf-8")


def patch_color_temp_mireds_range(zap_path: Path) -> None:
    """ColorTempPhysicalMin/MaxMireds = 111..500 (warm..cool) for CT-capable lights."""
    text = zap_path.read_text(encoding="utf-8")
    text = re.sub(
        r'("name": "ColorTempPhysicalMinMireds",[\s\S]*?"defaultValue": )"(?:0x009A|154)"',
        r'\1"111"',
        text,
        count=1,
    )
    text = re.sub(
        r'("name": "ColorTempPhysicalMaxMireds",[\s\S]*?"defaultValue": )"(?:0x01C6|454)"',
        r'\1"500"',
        text,
        count=1,
    )
    zap_path.write_text(text, encoding="utf-8")


def patch_ct_dual_pintool(pintool: Path) -> None:
    """TIMER0 CC0 on PB4 (cool) and TIMER1 CC0 on PB5 (warm), linked to SL_PWM_PWM0/1."""
    text = pintool.read_text(encoding="utf-8")
    if "portio.timer0.enable.cc0" not in text:
        block = (
            '    <property object="PORTIO" propertyId="portio.timer0.enable.cc0" value="Enabled"/>\n'
            '    <property object="PORTIO" propertyId="portio.timer0.location.cc0" value="20"/>\n'
            '    <property object="PORTIO" propertyId="portio.timer1.enable.cc0" value="Enabled"/>\n'
            '    <property object="PORTIO" propertyId="portio.timer1.location.cc0" value="21"/>\n'
        )
        text = text.replace(
            '    <property object="PORTIO" propertyId="portio.pti.location.dout" value="38"/>\n',
            '    <property object="PORTIO" propertyId="portio.pti.location.dout" value="38"/>\n' + block,
            1,
        )
    pintool.write_text(text, encoding="utf-8")


def patch_pwm_config(cfg_path: Path, spec: tuple) -> None:
    port, pin, timer, timer_no, channel = spec
    tag_m = re.search(r"sl_pwm_init_(pwm\d+)_config", cfg_path.name)
    if not tag_m:
        return
    tag = tag_m.group(1).upper()
    port_letter = "C" if "PORT_C" in port else "B" if "PORT_B" in port else "A"
    pin_label = f"P{port_letter}{pin:02d}"
    pin_tool = f"""// <<< sl:start pin_tool >>>

// <timer channel=OUTPUT> SL_PWM_{tag}
// $[TIMER_SL_PWM_{tag}]
#ifndef SL_PWM_{tag}_PERIPHERAL
#define SL_PWM_{tag}_PERIPHERAL                   {timer}
#endif
#ifndef SL_PWM_{tag}_PERIPHERAL_NO
#define SL_PWM_{tag}_PERIPHERAL_NO                {timer_no}
#endif

#ifndef SL_PWM_{tag}_OUTPUT_CHANNEL
#define SL_PWM_{tag}_OUTPUT_CHANNEL               {channel}
#endif
// {timer} CC{channel} on {pin_label}
#ifndef SL_PWM_{tag}_OUTPUT_PORT
#define SL_PWM_{tag}_OUTPUT_PORT                  {port}
#endif
#ifndef SL_PWM_{tag}_OUTPUT_PIN
#define SL_PWM_{tag}_OUTPUT_PIN                   {pin}
#endif
// [TIMER_SL_PWM_{tag}]$

// <<< sl:end pin_tool >>>"""
    text = cfg_path.read_text(encoding="utf-8")
    if "// <<< sl:start pin_tool >>>" in text:
        text = re.sub(
            r"// <<< sl:start pin_tool >>>.*?// <<< sl:end pin_tool >>>",
            pin_tool,
            text,
            flags=re.DOTALL,
        )
    else:
        text = text.replace(
            "// <<< end of configuration section >>>",
            "// <<< end of configuration section >>>\n\n" + pin_tool,
        )
    cfg_path.write_text(text, encoding="utf-8")


def patch_slcp(slcp: Path, product: dict) -> None:
    text = slcp.read_text(encoding="utf-8")
    if "matter_diagnostic_logs" not in text:
        text = text.replace(
            "- package: matter\n  vendor: silabs\n  id: matter_general_diagnostics\n",
            "- package: matter\n  vendor: silabs\n  id: matter_diagnostic_logs\n"
            "- package: matter\n  vendor: silabs\n  id: matter_general_diagnostics\n",
        )
    text = re.sub(
        r"- path: config/common/[\w-]+\.zap",
        f"- path: config/common/{product['zap_name']}",
        text,
    )
    inst = product["pwm_instances"]
    if inst:
        inst_yaml = "\n".join(f"  - {x}" for x in inst)
        text = re.sub(
            r"  instance:\n(?:  - pwm\d+\n)+",
            f"  instance:\n{inst_yaml}\n",
            text,
        )
    else:
        text = re.sub(
            r"- package: simplicity_sdk\n  vendor: silabs\n  id: hal_iadc\n"
            r"- package: simplicity_sdk\n  instance:\n(?:  - pwm\d+\n)+  vendor: silabs\n  id: pwm\n",
            "- package: simplicity_sdk\n  vendor: silabs\n  id: hal_iadc\n",
            text,
        )
        for src_name in (
            "CtDualPwmDriver.cpp",
            "SinglePwmDriver.cpp",
            "RgbcwPwmDriver.cpp",
        ):
            text = text.replace(f"- path: src/{src_name}\n", "")
        for hdr in ("CtDualPwmDriver.h", "CtPwmVersion.h", "SinglePwmDriver.h", "RgbcwPwmDriver.h"):
            text = text.replace(f"  - path: {hdr}\n", "")
    slcp.write_text(text, encoding="utf-8")


def patch_driver_refs(app: Path) -> None:
    """Replace CtDualPwmDriver with LightOutput in shared protection/flash code."""
    for rel in ("src/OvercurrentProtector.cpp", "src/DeviceUserFlash.cpp", "src/CustomerAppTask.cpp"):
        f = app / rel
        if not f.is_file():
            continue
        text = f.read_text(encoding="utf-8")
        if "CtDualPwmDriver" not in text:
            continue
        text = text.replace('#include "CtDualPwmDriver.h"', '#include "LightOutput.h"')
        text = text.replace("CtDualPwmDriver::", "LightOutput::")
        f.write_text(text, encoding="utf-8")


def patch_slcp_sources(slcp: Path, add: list[str], remove: list[str]) -> None:
    text = slcp.read_text(encoding="utf-8")
    for r in remove:
        text = text.replace(f"- path: src/{r}\n", "")
    for a in add:
        if f"- path: src/{a}\n" not in text:
            text = text.replace("- path: src/main.cpp\n", f"- path: src/main.cpp\n- path: src/{a}\n")
    slcp.write_text(text, encoding="utf-8")


def migrate_product(name: str, product: dict) -> None:
    root = PROJECT_ROOT / name
    app_dir = product.get("app_dir", "lighting_app")
    slcp_name = product.get("slcp", "matter_thread_soc_lighting_app_freertos.slcp")
    app = root / app_dir
    if not app.is_dir():
        raise SystemExit(f"missing app dir: {app}")

    print(f"[{name}] {product['label']}")

    ref_cfg = REF_APP / "config"
    app_cfg = app / "config"
    if app_cfg.exists():
        shutil.rmtree(app_cfg)
    shutil.copytree(ref_cfg, app_cfg)

    zap_dst = app_cfg / "common" / product["zap_name"]
    shutil.copy2(product["zap_src"], zap_dst)
    fix_zap_paths(zap_dst)
    if name in ("colortemperature_light", "extended_color_light"):
        patch_color_temp_mireds_range(zap_dst)
    old_zap = app_cfg / "common" / "lighting-thread-app.zap"
    if old_zap.is_file() and old_zap != zap_dst:
        old_zap.unlink()

    for pwm, spec in product["pwm_pins"].items():
        cfg = app_cfg / f"sl_pwm_init_{pwm}_config.h"
        if cfg.is_file():
            patch_pwm_config(cfg, spec)
    for extra in ("pwm0", "pwm1", "pwm2", "pwm3", "pwm4"):
        if extra not in product["pwm_pins"]:
            p = app_cfg / f"sl_pwm_init_{extra}_config.h"
            if p.is_file():
                p.unlink()

    slcp_path = app / slcp_name
    patch_slcp(slcp_path, product)

    for sub in ("src", "include"):
        s = REF_APP / sub
        d = app / sub
        if d.exists():
            shutil.rmtree(d)
        shutil.copytree(s, d)

    overlay = OVERLAYS / name
    if overlay.is_dir():
        for rel in overlay.rglob("*"):
            if rel.is_file():
                target = app / rel.relative_to(overlay)
                target.parent.mkdir(parents=True, exist_ok=True)
                shutil.copy2(rel, target)

    drivers = SCRIPT_DIR / "light_drivers"
    if name == "dimmable_light":
        shutil.copy2(drivers / "SinglePwmDriver.h", app / "include" / "SinglePwmDriver.h")
        shutil.copy2(drivers / "SinglePwmDriver.cpp", app / "src" / "SinglePwmDriver.cpp")
        patch_slcp_sources(
            slcp_path,
            ["SinglePwmDriver.cpp"],
            ["CtDualPwmDriver.cpp"],
        )
        for stale_drv in ("CtDualPwmDriver.cpp", "CtDualPwmDriver.h", "CtPwmVersion.h"):
            p = app / ("src" if stale_drv.endswith(".cpp") else "include") / stale_drv
            if p.is_file():
                p.unlink()
    elif name == "extended_color_light":
        for f in ("RgbcwPwmDriver.h", "RgbcwPwmDriver.cpp"):
            shutil.copy2(drivers / f, app / ("include" if f.endswith(".h") else "src") / f)
        patch_slcp_sources(
            slcp_path,
            ["RgbcwPwmDriver.cpp"],
            ["CtDualPwmDriver.cpp"],
        )
        for stale_drv in ("CtDualPwmDriver.cpp", "CtDualPwmDriver.h", "CtPwmVersion.h"):
            p = app / ("src" if stale_drv.endswith(".cpp") else "include") / stale_drv
            if p.is_file():
                p.unlink()
    elif name == "extended_color_light_strip":
        for f in ("RgbcwStripDriver.h", "RgbcwStripDriver.cpp"):
            shutil.copy2(drivers / f, app / ("include" if f.endswith(".h") else "src") / f)
        patch_slcp_sources(
            slcp_path,
            ["RgbcwStripDriver.cpp"],
            ["CtDualPwmDriver.cpp"],
        )
        strip_app = OVERLAYS / "extended_color_light" / "src" / "CustomerAppTask.cpp"
        if strip_app.is_file():
            text = strip_app.read_text(encoding="utf-8").replace("RgbcwPwmDriver.h", "RgbcwStripDriver.h")
            (app / "src" / "CustomerAppTask.cpp").write_text(text, encoding="utf-8")
        for stale_drv in ("CtDualPwmDriver.cpp", "CtDualPwmDriver.h", "CtPwmVersion.h"):
            p = app / ("src" if stale_drv.endswith(".cpp") else "include") / stale_drv
            if p.is_file():
                p.unlink()

    patch_driver_refs(app)

    pintool = app / slcp_name.replace(".slcp", ".pintool")
    if pintool.is_file():
        if name == "extended_color_light":
            patch_rgbcw_pintool(pintool)
        elif name == "colortemperature_light":
            patch_ct_dual_pintool(pintool)

    text = slcp_path.read_text(encoding="utf-8")
    extra_headers = ["LightOutput.h", "SinglePwmDriver.h", "RgbcwPwmDriver.h", "RgbcwStripDriver.h"]
    for hdr in extra_headers:
        entry = f"  - path: {hdr}\n"
        if entry not in text and (app / "include" / hdr).is_file():
            text = text.replace("  - path: CustomerAppTask.h\n", f"  - path: CustomerAppTask.h\n{entry}")
    slcp_path.write_text(text, encoding="utf-8")

    if REF_BOOT.exists():
        copy_tree(REF_BOOT, root / "matter_bootloader")

    for stale in ("autogen", "cmake_gcc", "cmake_iar", "cmake_llvm", ".uceditor"):
        p = app / stale
        if p.exists():
            shutil.rmtree(p)


def main() -> None:
    for name, product in PRODUCTS.items():
        migrate_product(name, product)
    print("Migration done. Regenerate with slc/sl_build.py per project.")


if __name__ == "__main__":
    main()
