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
| `extended_color_light_strip` | SPI 幻彩灯带（无 PWM）      |

## 脚本目录

产品迁移与驱动源码位于 `project/scripts/`：

- `setup_light_projects.py` — 从 `lighting_mot` 同步配置与自定义代码
- `rename_official_naming.py` — 将 `lighting_app` 批量改为官方命名（已执行可跳过）
- `light_drivers/` — 各灯型 PWM/SPI 驱动
- `light_products/` — 各产品的 `CustomerAppTask` 等覆盖文件

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

以下命令均在**仓库根目录**执行。SLC 会生成工程并调用 CMake 完成编译。

### 1. 调光灯（Dimmable Light）

```bash
python3 slc/sl_build.py \
  -p project/dimmable_light/matter_thread_soc_dimmable_light_app_series_2_internal_freertos.slcw \
  -b brd2703a
```

### 2. 色温灯（Color Temperature Light）

```bash
python3 slc/sl_build.py \
  -p project/colortemperature_light/matter_thread_soc_colortemperature_light_app_series_2_internal_freertos.slcw \
  -b brd2703a
```

### 3. 五路 RGBCW 灯（Extended Color Light）

```bash
python3 slc/sl_build.py \
  -p project/extended_color_light/matter_thread_soc_extended_color_light_app_series_2_internal_freertos.slcw \
  -b brd2703a
```

### 4. RGBCW SPI 幻彩灯带（Extended Color Light Strip）

```bash
python3 slc/sl_build.py \
  -p project/extended_color_light_strip/matter_thread_soc_extended_color_light_strip_app_series_2_internal_freertos.slcw \
  -b brd2703a
```

## 推荐：使用 build_light.py 编译

在 Simplicity Studio 中用 **Pin Tool** 配好 PWM 后，用本脚本编译（**默认不会改写** `config/sl_pwm_init_pwm*_config.h` 与 `.pintool`）：

```bash
python3 project/scripts/build_light.py \
  project/<产品目录>/matter_thread_soc_<device>_app_series_2_internal_freertos.slcw \
  -b brd2703a
```

仅重新编译（已 `slc generate` 且引脚未改）：

```bash
python3 project/scripts/build_light.py project/<产品>/...slcw -b brd2703a -s
```

若需用脚本批量写回引脚宏（会覆盖 Studio 配置），加 `--patch-config`。

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
