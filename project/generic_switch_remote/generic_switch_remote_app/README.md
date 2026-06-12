# Matter Thread Generic Switch (BRD2703A)

4 个 Generic Switch 端点（endpoint 1-4），Matter Switch 集群事件逻辑参考 `connectedhomeip/examples/generic-switch-app`。

**电池供电 ICD 模式**：Thread MTD/SED + ICD Management / Time Synchronization（与上游 `generic-switch-app` 一致），空闲时进入低功耗，按键唤醒发送 Switch 事件。

## 硬件映射

| 功能 | 引脚 | 说明 |
|------|------|------|
| 复位键 | BTN0 (PB02) | 低电平有效，长按恢复出厂 |
| Switch 键 c0 | PC00 | endpoint 1 |
| Switch 键 c1 | PC01 | endpoint 2 |
| Switch 键 c2 | PC02 | endpoint 3 |
| Switch 键 c3 | PC03 | endpoint 4 |
| LED0 (PA04) | 配网指示灯 | 未配网/BLE 配网时闪烁，已配网熄灭 |
| LED1 (PA07) | 按键指示灯 | 任一键按下亮起，全部释放熄灭 |

## 烧录

必须使用带内部 Bootloader 的完整固件（首次烧录或更换工程时）：

```
project/generic_switch_remote/artifact/matter_thread_soc_generic_switch_remote_app_series_2_internal_freertos_full.s37
```

不要只烧录 `*_app_only.gbl`，否则设备无法启动。

Simplicity Commander 示例：

```bash
commander flash <上述 full.s37 路径> --device EFR32MG24B210F1536IM48
```

## 串口日志

日志走 **VCOM 虚拟串口**（不是 RTT Viewer），参数：

- 波特率：**115200**
- 数据位：8，无校验，1 停止位
- 流控：**无**（Linux minicom/screen 默认即可）

Linux 查看设备（先打开串口，再按复位键）：

```bash
ls -l /dev/ttyACM* /dev/ttyUSB*
stty -F /dev/ttyACM0 115200 cs8 -cstopb -parenb -crtscts
cat /dev/ttyACM0
```

上电后应首先看到：

```
=== Generic Switch Boot ===
=====GenericSwitch starting=====
```

若仍无输出，请确认：

1. 烧录的是 `*_full.s37` 而非 `.gbl`
2. 打开的是 J-Link 对应的 VCOM 口（非 PTI 口）
3. c0-c3 外接按键时一端接 GND；未接按键时固件已启用内部上拉
4. 若曾烧录过旧版 FTD 固件，请 **长按 BTN0 恢复出厂** 后再测

## 编译

```bash
./build.sh generic_switch_remote
```

工程目录内也可直接调用（含 SLC generate 后的 codegen 补丁）：

```bash
python3 project/generic_switch_remote/build.py -b brd2703a
python3 project/generic_switch_remote/build.py -s   # 仅重编译，跳过 generate/补丁
```

`slc generate` 后需执行 codegen 补丁（`scripts/patch_codegen.py`），已由 `build.py` / `./build.sh` 自动串联。
