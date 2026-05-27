#!/usr/bin/env python3
"""SLC generate, optional config patch, configure CMake subprojects, then build."""

from __future__ import annotations

import argparse
import logging
import shutil
import subprocess
import sys
from pathlib import Path

REPO = Path(__file__).resolve().parents[2]
SLC_DIR = REPO / "slc"
SCRIPT_DIR = Path(__file__).resolve().parent
SL_BUILD = SLC_DIR / "sl_build.py"
PATCH_PWM = SCRIPT_DIR / "patch_pwm_configs.py"
PATCH_PINTOOL = SCRIPT_DIR / "patch_pintools.py"

sys.path.insert(0, str(SLC_DIR))
from sl_build import parse_project_file, run_slc_generate  # noqa: E402
from sl_create_new_app import CreateApp  # noqa: E402


def _cmake_workflow(cmake_dir: Path) -> None:
    cmd = ["cmake", "--workflow", "--preset", "project"]
    logging.info("cmake workflow in %s", cmake_dir)
    subprocess.run(cmd, cwd=cmake_dir, check=True)


def _remove_stale_cmake_build_dirs(slcw: Path) -> None:
    product_dir = slcw.parent.resolve()
    solution_build = product_dir / f"{slcw.stem}_cmake" / "build"
    cache = solution_build / "CMakeCache.txt"
    if cache.is_file():
        cached = cache.read_text(encoding="utf-8", errors="ignore")
        if str(product_dir) not in cached or "single_light" in cached:
            shutil.rmtree(solution_build)
            logging.info("removed stale solution build: %s", solution_build)

    for cmake_gcc in product_dir.glob("*/cmake_gcc"):
        build_dir = cmake_gcc / "build"
        sub_cache = build_dir / "CMakeCache.txt"
        if build_dir.is_dir() and not sub_cache.is_file():
            shutil.rmtree(build_dir)
            logging.info("removed stale build dir: %s", build_dir)


def _configure_subprojects(slcw: Path) -> None:
    product_dir = slcw.parent
    for cmake_gcc in sorted(product_dir.glob("*/cmake_gcc")):
        if (cmake_gcc / "CMakePresets.json").is_file():
            _cmake_workflow(cmake_gcc)


def _slc_generate(slcw: Path, board: str) -> None:
    app = CreateApp()
    if not app.validate_board_argument(board):
        raise SystemExit(1)
    CreateApp.validate_tools()
    project_flag, _, output_dir, _, build_type = parse_project_file(str(slcw))
    run_slc_generate(app, app.slc_path, output_dir, project_flag, str(slcw), board, build_type)


def main() -> int:
    logging.basicConfig(level=logging.INFO, format="[%(levelname)s] %(message)s")
    parser = argparse.ArgumentParser(description="Build a project/* Matter light solution")
    parser.add_argument("slcw", type=Path, help="Path to .slcw under project/")
    parser.add_argument("-b", "--board", default="brd2703a")
    parser.add_argument("-j", "--jobs", type=int, default=None)
    parser.add_argument("-s", "--skip-gen", action="store_true", help="Skip SLC generate")
    parser.add_argument(
        "--patch-config",
        action="store_true",
        help="Run patch_pintools.py and patch_pwm_configs.py after generate (overrides Studio pin_tool)",
    )
    args = parser.parse_args()

    slcw = args.slcw.resolve()
    if not slcw.is_file():
        logging.error("missing slcw: %s", slcw)
        return 1

    if not args.skip_gen:
        _slc_generate(slcw, args.board.lower())
        if args.patch_config:
            for patch in (PATCH_PINTOOL, PATCH_PWM):
                logging.info("run %s", patch.name)
                if subprocess.call([sys.executable, str(patch)]) != 0:
                    return 1

    _remove_stale_cmake_build_dirs(slcw)
    try:
        _configure_subprojects(slcw)
    except subprocess.CalledProcessError as exc:
        logging.error("subproject cmake workflow failed")
        return exc.returncode or 1

    cmd = [sys.executable, str(SL_BUILD), "-p", str(slcw), "-b", args.board, "-s"]
    if args.jobs is not None:
        cmd.extend(["-j", str(args.jobs)])
    logging.info("run: %s", " ".join(cmd))
    return subprocess.call(cmd)


if __name__ == "__main__":
    sys.exit(main())
