# SuperPad-V1

开源游戏手柄（控制器）项目 —— 基于 **Input Labs Alpakka** 参考设计开发。

## 许可

- 本仓库整体使用 **GNU General Public License v2（GPL-2.0）**，全文见 [LICENSE](LICENSE)。
- 许可依据：上游 **Alpakka 固件**使用 GPL-2.0，本项目遵循同样的开源许可。
- 注意：上游 **PCB / CAD 设计**为 **CC BY-NC-SA 4.0**（非商业、署名、相同方式共享），见 `hardware/alpakka_kicad/license.md`；搬运硬件文件前请务必确认许可兼容性（NC 限制）。

## 目录结构

```
SuperPad-V1/
├── LICENSE                    # GPL-2.0
├── hardware/                  # 硬件设计
│   ├── superpad_v1_main.eprj2 # 嘉立创 EDA 专业版工程（本项目）
│   ├── alpakka_kicad/         # 上游 Alpakka KiCad 源文件（参考）
│   └── IMPORT_ALPAKKA_TO_LCEDA.md  # 导入嘉立创 EDA 指南
├── firmware/                  # 固件（多平台）
│   ├── keil/                  #   Keil 工程
│   │   └── RP2040/            #     RP2040（Alpakka 官方目标）
│   └── platformio_ide/        #   PlatformIO 工程
│       └── RP2040/            #     RP2040（Alpakka 官方目标）
└── ForeverThanksAlpakka/      # 上游归档（zip + 说明 + 许可）
```

## 快速开始

- **硬件**：参照 `hardware/IMPORT_ALPAKKA_TO_LCEDA.md` 把 Alpakka 工程导入嘉立创 EDA 专业版。
- **固件（PlatformIO）**：见 `firmware/platformio_ide/RP2040/readme.md`。
- **固件（Keil）**：见 `firmware/keil/RP2040/readme.md`。

## 致谢

感谢 [Input Labs](https://github.com/orgs/inputlabs/repositories?sort=name) 开源 Alpakka 控制器参考设计（固件 GPL-2.0、PCB/CAD CC BY-NC-SA 4.0）。详见 `ForeverThanksAlpakka/README.md`。
