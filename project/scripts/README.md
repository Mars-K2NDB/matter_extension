# project/scripts

本目录**不参与固件编译**。SLC/CMake 只编译各产品 `project/<产品>/*_app/` 下的源码。

此处为**迁移与维护工具**：从 `lighting_mot` 同步工程，并把模板驱动/覆盖文件部署到各产品目录。

## 参与编译的源码位置

| 产品 | 实际编译目录 |
|------|----------------|
| 调光灯 | `project/dimmable_light/dimmable_light_app/` |
| 色温灯 | `project/colortemperature_light/colortemperature_light_app/` |
| RGBCW 灯 | `project/extended_color_light/extended_color_light_app/` |
| SPI 灯带 | `project/extended_color_light_strip/extended_color_light_strip_app/` |

编译请使用仓库根目录 `./build.sh <产品目录名>`（内部调用 `slc/sl_build.py`）。

## 目录结构

```
scripts/
  setup_light_projects.py   # 主迁移脚本
  patch_pwm_configs.py        # 可选：批量写回 PWM pin_tool 宏
  patch_pintools.py           # 可选：批量写回 .pintool 路由
  templates/
    drivers/                  # 各灯型驱动模板（与工程内文件名一致，snake_case）
    overlays/<产品>/          # 每产品 light_output.h、CustomerAppTask.cpp 等覆盖
```

## 脚本说明

| 文件 | 用途 |
|------|------|
| `setup_light_projects.py` | 从 `lighting_mot` 复制 config/src，部署 `templates/`，打 ZAP/PWM/slcp 补丁 |
| `patch_pwm_configs.py` | 在 `slc generate` 后恢复 `sl_pwm_init_pwm*_config.h` 引脚块 |
| `patch_pintools.py` | 在 `slc generate` 后恢复色温/RGBCW 的 TIMER 路由 |

## 维护模板

修改某产品驱动后，请同步回 `templates/drivers/`（或对应 `templates/overlays/`），以便下次执行 `setup_light_projects.py` 时不会覆盖为旧代码。

```bash
# 示例：同步调光灯驱动模板
cp project/dimmable_light/dimmable_light_app/src/single_pwm_driver.cpp \
   project/scripts/templates/drivers/
cp project/dimmable_light/dimmable_light_app/include/single_pwm_driver.h \
   project/scripts/templates/drivers/
```

## 典型流程

```bash
# 1. 创建工程骨架（每个产品一次）
python3 slc/sl_create_new_app.py -n project/dimmable_light \
  -p lighting_mot/matter_thread_soc_lighting_app_series_2_internal_freertos.slcw \
  -b brd2703a

# 2. 迁移配置与驱动
python3 project/scripts/setup_light_projects.py

# 3. 编译
./build.sh dimmable_light
```

可选（会覆盖 Studio Pin Tool 生成内容）：

```bash
python3 project/scripts/patch_pintools.py
python3 project/scripts/patch_pwm_configs.py
```
