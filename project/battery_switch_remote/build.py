#!/usr/bin/env python3
"""Battery Switch 工程编译：SLC generate → codegen 补丁 → 编译。"""

from __future__ import annotations

import argparse
import logging
import subprocess
import sys
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parents[2]
SLC_DIR = REPO_ROOT / "slc"
PROJECT_DIR = Path(__file__).resolve().parent
SLCW = PROJECT_DIR / "matter_thread_soc_battery_switch_remote_app_series_2_internal_freertos.slcw"
PATCH_SCRIPT = PROJECT_DIR / "scripts" / "patch_codegen.py"
SL_BUILD = REPO_ROOT / "slc" / "sl_build.py"

sys.path.insert(0, str(SLC_DIR))

from sl_build import parse_project_file, run_build, run_slc_generate  # noqa: E402
from sl_create_new_app import CreateApp  # noqa: E402


def run_patch_codegen() -> None:
    if not PATCH_SCRIPT.is_file():
        logging.error("缺少 codegen 补丁脚本: %s", PATCH_SCRIPT)
        sys.exit(1)
    logging.info("运行 codegen 补丁: %s", PATCH_SCRIPT)
    result = subprocess.run([sys.executable, str(PATCH_SCRIPT)], check=False)
    if result.returncode != 0:
        logging.error("codegen 补丁失败，退出码 %s", result.returncode)
        sys.exit(result.returncode)


def main() -> None:
    parser = argparse.ArgumentParser(description="编译 Battery Switch 遥控器工程。")
    parser.add_argument(
        "-b",
        "--silabs_board",
        default="brd2703a",
        help="Silicon Labs 开发板名称（默认 brd2703a）",
    )
    parser.add_argument("-j", "--jobs", type=int, default=13, help="并行编译任务数")
    parser.add_argument(
        "-s",
        "--skip_gen",
        action="store_true",
        help="跳过 SLC generate 与 codegen 补丁，仅编译",
    )
    parser.add_argument("-v", "--verbose", action="store_true", help="输出调试日志")
    args = parser.parse_args()

    level = logging.DEBUG if args.verbose else logging.INFO
    logging.basicConfig(level=level, format="[%(levelname)s] %(message)s")

    reference_project_file = str(SLCW)
    silabs_board = args.silabs_board.lower()
    jobs = args.jobs

    if not SLCW.is_file():
        logging.error("未找到工程文件: %s", SLCW)
        sys.exit(1)

    if args.skip_gen:
        if not SL_BUILD.is_file():
            logging.error("缺少编译脚本: %s", SL_BUILD)
            sys.exit(1)
        cmd = [
            sys.executable,
            str(SL_BUILD),
            "-p",
            reference_project_file,
            "-b",
            silabs_board,
            "-s",
            "-j",
            str(jobs),
        ]
        logging.info("执行: %s", " ".join(cmd))
        result = subprocess.run(cmd, cwd=REPO_ROOT, check=False)
        if result.returncode != 0:
            sys.exit(result.returncode)
        return

    try:
        app = CreateApp()
    except SystemExit:
        logging.error("初始化编译环境失败")
        sys.exit(1)

    if not app.validate_board_argument(silabs_board):
        sys.exit(1)

    CreateApp.validate_tools()

    project_flag, _silabs_app, output_dir, project_path, build_type = parse_project_file(
        reference_project_file
    )
    run_slc_generate(
        app,
        app.slc_path,
        output_dir,
        project_flag,
        reference_project_file,
        silabs_board,
        build_type,
    )
    run_patch_codegen()
    run_build(app, output_dir, project_path, build_type, jobs)
    logging.info("编译完成")


if __name__ == "__main__":
    main()
