# Matter Thread 照明产品工程

基于 `lighting_mot` 拆分的四个 Matter over Thread 照明产品。

| 目录                         | Matter 设备类型            | 应用目录                         | Workspace                                                                     |
| ---------------------------- | -------------------------- | -------------------------------- | ----------------------------------------------------------------------------- |
| `dimmable_light`             | Dimmable Light             | `dimmable_light_app`             | `matter_thread_soc_dimmable_light_app_series_2_internal_freertos`             |
| `colortemperature_light`     | Color Temperature Light    | `colortemperature_light_app`     | `matter_thread_soc_colortemperature_light_app_series_2_internal_freertos`     |
| `extended_color_light`       | Extended Color Light       | `extended_color_light_app`       | `matter_thread_soc_extended_color_light_app_series_2_internal_freertos`       |
| `extended_color_light_strip` | Extended Color Light Strip | `extended_color_light_strip_app` | `matter_thread_soc_extended_color_light_strip_app_series_2_internal_freertos` |

| 目录                         | PWM / 输出                                    |
| ---------------------------- | --------------------------------------------- |
| `dimmable_light`             | PB4                                           |
| `colortemperature_light`     | PB4 冷 / PB5 暖                               |
| `extended_color_light`       | PC0–PC2 (RGB), PB4–PB5 (CW)                   |
| `extended_color_light_strip` | SPI 幻彩灯带（WS2814，69 颗，内置 15 种灯效） |

## 脚本目录

`project/scripts/` 为迁移与板型切换工具。

- `setup_light_projects.py` — 从 `lighting_mot` 同步并部署 `templates/` 驱动与覆盖
- `switch_board.py` — 在 `brd2703a` / `rf_bm_mg24b1` / `rf_bm_mg24b2` 间切换四个产品（含 RAIL PA 自动匹配）
- `patch_pwm_configs.py` / `patch_pintools.py` — 可选，批量恢复引脚配置
- `boards/` — 长期维护的硬件 profile（芯片、CTUNE、slcp 组件规则）
- `templates/drivers/` — 各灯型驱动模板（snake_case，与 `*_app` 内一致）
- `templates/overlays/` — 各产品 `light_output.h`、`CustomerAppTask.cpp` 等

## 硬件板型切换（多 profile）

仓库长期维护多套硬件目标，定义位于 `project/scripts/boards/`。`switch_board.py` 切换时会按 `pa_tx_dbm` 自动写入 RAIL sequencer / PA tables，避免 10 dBm 与 19.5 dBm 芯片混用导致 `RAIL Assert:83`。

| Profile        | 硬件                         | 芯片                   | PA       | HFXO CTUNE |
| -------------- | ---------------------------- | ---------------------- | -------- | ---------- |
| `brd2703a`     | Silicon Labs BRD2703A 开发板 | EFR32MG24B210F1536IM48 | 10 dBm   | 100        |
| `rf_bm_mg24b1` | RF-star RF-BM-MG24B1 模组    | EFR32MG24A410F1536IM48 | 10 dBm   | 92         |
| `rf_bm_mg24b2` | RF-star RF-BM-MG24B2 模组    | EFR32MG24A420F1536IM48 | 19.5 dBm | 92         |

当前激活板型记录在 `project/.board-active`（由 `switch_board.py` 或 `build.sh -b` 自动写入，本地文件不提交 Git）。

```bash
# 查看当前板型
python3 project/scripts/switch_board.py status

# 切换到 19.5 dBm 模组 B2
python3 project/scripts/switch_board.py rf_bm_mg24b2

# 切换到 10 dBm 模组 B1
python3 project/scripts/switch_board.py rf_bm_mg24b1

# 切回开发板
python3 project/scripts/switch_board.py brd2703a

# 强制重新应用 RAIL PA 配置（板型未变时）
python3 project/scripts/switch_board.py rf_bm_mg24b2 --force
```

编译时也可直接指定板型；若与 `.board-active` 不一致，`build.sh` 会自动调用 `switch_board.py`：

```bash
./build.sh extended_color_light -b rf_bm_mg24b2
./build.sh extended_color_light -b rf_bm_mg24b1
./build.sh extended_color_light -b brd2703a
```

**Simplicity Studio 6**：在打开 `.slcw` 前，先执行 `switch_board.py` 切换到目标板型，SS6 与 CLI 将读取同一份 `.slcp`。请勿将 Windows SS6 生成的 `autogen/`、`cmake_*` 提交到仓库。

## 环境准备

在仓库根目录执行（仅需一次）：

```bash
pip3 install -r requirements.txt
python3 slc/sl_setup_env.py
```

默认硬件目标：`brd2703a`（BRD2703A）。模组产品使用 `-b rf_bm_mg24b2`。

## 创建 / 更新工程

首次创建或从 `lighting_mot` 同步配置与源码后执行：

```bash
# 示例：创建工程骨架（将 <产品目录> 换为下表对应名称）
python3 slc/sl_create_new_app.py -n project/<产品目录> \
  -p lighting_mot/matter_thread_soc_lighting_app_series_2_internal_freertos.slcw \
  -b brd2703a

# 迁移 lighting_mot 配置、自定义 src 与 PWM 引脚
python3 project/scripts/setup_light_projects.py
```

## 编译指令

在**仓库根目录**使用 `build.sh`（内部调用 `slc/sl_build.py`）：

```bash
source ./build.sh                  # 列出项目并在当前终端启用 Tab 补全（每新开终端执行一次）
./build.sh list                    # 仅列出可编译项目
./build.sh dimmable_light
./build.sh colortemperature_light
./build.sh extended_color_light
./build.sh extended_color_light_strip
```

仅重新编译（跳过 SLC generate）：

```bash
./build.sh rebuild <产品目录名>
```

清除编译产物：

```bash
./build.sh clean <产品目录名>
```

也可直接调用：

```bash
python3 slc/sl_build.py \
  -p project/<产品目录>/matter_thread_soc_<device>_app_series_2_internal_freertos.slcw \
  -b brd2703a
```

## PWM 与驱动通道（应用层按引脚自动匹配）

| 产品       | 硬件引脚              | 驱动逻辑                                                |
| ---------- | --------------------- | ------------------------------------------------------- |
| 调光灯     | PB04                  | `SinglePwmDriver` → `sl_pwm_pwm0`                       |
| 色温灯     | PB04 冷 / PB05 暖     | `CtDualPwmDriver` 按 `OUTPUT_PORT/PIN` 选冷/暖 PWM 实例 |
| 五路 RGBCW | PC00/01/02 + PB04/05  | `RgbcwPwmDriver` 按引脚映射 R/G/B/C/W                   |
| SPI 灯带   | EUSART1 TX（如 PC09） | `RgbcwStripDriver` + `ws2814_strip`（WS2814 SPI）       |

## 常用选项

仅重新编译、跳过 SLC 生成（工程已生成且配置未改时）：

```bash
python3 slc/sl_build.py \
  -p project/<产品目录>/matter_thread_soc_<device>_app_series_2_internal_freertos.slcw \
  -b brd2703a \
  -s
```

并行编译任务数（默认 13）：

```bash
python3 slc/sl_build.py \
  -p project/dimmable_light/matter_thread_soc_dimmable_light_app_series_2_internal_freertos.slcw \
  -b brd2703a \
  -j 8
```

## 编译产物

Solution 成功构建后，合并固件通常位于各产品 `artifact/` 目录：

| 产品     | 合并固件                                                                                                                           |
| -------- | ---------------------------------------------------------------------------------------------------------------------------------- |
| 调光灯   | `project/dimmable_light/artifact/matter_thread_soc_dimmable_light_app_series_2_internal_freertos_full.s37`                         |
| 色温灯   | `project/colortemperature_light/artifact/matter_thread_soc_colortemperature_light_app_series_2_internal_freertos_full.s37`         |
| RGBCW 灯 | `project/extended_color_light/artifact/matter_thread_soc_extended_color_light_app_series_2_internal_freertos_full.s37`             |
| SPI 灯带 | `project/extended_color_light_strip/artifact/matter_thread_soc_extended_color_light_strip_app_series_2_internal_freertos_full.s37` |

应用单独镜像（未合并 bootloader）位于各 `*_app/artifact/` 或 `*_app/cmake_gcc/build/base/*.s37`。
