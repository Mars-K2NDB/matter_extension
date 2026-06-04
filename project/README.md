# Matter Thread 照明产品工程

基于 `lighting_mot` 拆分的四个 Matter over Thread 照明产品。

| 目录                         | Matter 设备类型            | 应用目录                         | Workspace                                                                     |
| ---------------------------- | -------------------------- | -------------------------------- | ----------------------------------------------------------------------------- |
| `dimmable_light`             | Dimmable Light             | `dimmable_light_app`             | `matter_thread_soc_dimmable_light_app_series_2_internal_freertos`             |
| `colortemperature_light`     | Color Temperature Light    | `colortemperature_light_app`     | `matter_thread_soc_colortemperature_light_app_series_2_internal_freertos`     |
| `extended_color_light`       | Extended Color Light       | `extended_color_light_app`       | `matter_thread_soc_extended_color_light_app_series_2_internal_freertos`       |
| `extended_color_light_strip` | Extended Color Light Strip | `extended_color_light_strip_app` | `matter_thread_soc_extended_color_light_strip_app_series_2_internal_freertos` |

| 目录                         | PWM / 输出                  |
| ---------------------------- | --------------------------- |
| `dimmable_light`             | PB4                         |
| `colortemperature_light`     | PB4 冷 / PB5 暖             |
| `extended_color_light`       | PC0–PC2 (RGB), PB4–PB5 (CW) |
| `extended_color_light_strip` | SPI 幻彩灯带（WS2814，69 颗，内置 15 种灯效） |

## 脚本目录

`project/scripts/` 为迁移工具，**不参与编译**（说明见下文）。

- `setup_light_projects.py` — 从 `lighting_mot` 同步并部署 `templates/` 驱动与覆盖
- `patch_pwm_configs.py` / `patch_pintools.py` — 可选，批量恢复引脚配置
- `templates/drivers/` — 各灯型驱动模板（snake_case，与 `*_app` 内一致）
- `templates/overlays/` — 各产品 `light_output.h`、`CustomerAppTask.cpp` 等

## 环境准备

在仓库根目录执行（仅需一次）：

```bash
pip3 install -r requirements.txt
python3 slc/sl_setup_env.py
```

默认开发板：`brd2703a`（BRD2703A）。若使用其它板型，将下文命令中的 `-b brd2703a` 替换为对应板名。

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

| 产品       | 硬件引脚             | 驱动逻辑                                                |
| ---------- | -------------------- | ------------------------------------------------------- |
| 调光灯     | PB04                 | `SinglePwmDriver` → `sl_pwm_pwm0`                       |
| 色温灯     | PB04 冷 / PB05 暖    | `CtDualPwmDriver` 按 `OUTPUT_PORT/PIN` 选冷/暖 PWM 实例 |
| 五路 RGBCW | PC00/01/02 + PB04/05 | `RgbcwPwmDriver` 按引脚映射 R/G/B/C/W                   |
| SPI 灯带   | EUSART1 TX（如 PC09） | `RgbcwStripDriver` + `ws2814_strip`（WS2814 SPI）         |

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
