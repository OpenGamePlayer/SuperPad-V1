# ForeverThanksAlpakka

> **永远感谢 Input Labs** —— 本项目（SuperPad-V1）的固件、PCB 与外壳设计均基于其开源的 **Alpakka** 控制器参考设计。此目录存放从上游下载的原始包（zip）及解压内容，用于学习与二次开发参考。谨向 Input Labs 团队致敬与致谢 🙏

## 本项目基于的上级项目（Input Labs / Alpakka）

| 上游仓库 | 对应内容 | 许可 |
| --- | --- | --- |
| [inputlabs/alpakka_firmware](https://github.com/inputlabs/alpakka_firmware) | 固件（Raspberry Pi Pico 参考固件） | GNU GPL v2 |
| [inputlabs/alpakka_pcb](https://github.com/inputlabs/alpakka_pcb) | PCB 参考设计（KiCad 8） | CC BY-NC-SA 4.0 |
| [inputlabs/alpakka_case](https://github.com/inputlabs/alpakka_case) | 外壳 / CAD 3D 模型 | CC BY-NC-SA 4.0 |

- 上游组织全部仓库：<https://github.com/orgs/inputlabs/repositories?sort=name>
- 官方手册：<https://inputlabs.io/devices/alpakka/manual>
- 上游路线图：<https://github.com/orgs/inputlabs/projects/2/views/2>

## 上游使用的许可协议

### 1. 固件 —— GNU GENERAL PUBLIC LICENSE v2（GPL-2.0）

- 全文见 `alpakka_firmware-main/alpakka_firmware-main/license.md`
- 要点：可以复制、修改、再分发，但**修改/衍生作品必须同样以 GPL-2.0 开源**，并保留版权声明与免责声明；可以收费分发，但须提供源码或书面要约。

### 2. PCB 与 CAD —— Creative Commons BY-NC-SA 4.0（CC BY-NC-SA 4.0）

- 全文见 `pcb-main/pcb-main/license.md` 与 `cad-main/cad-main/license.md`
- 要点：
  - **BY（署名）**：使用时须注明出处并链接许可，说明是否修改
  - **NC（非商业）**：不得用于商业目的
  - **SA（相同方式共享）**：基于它的改作须以相同许可发布
  - 这是"人类可读摘要"，完整法律文本见 <https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode>

### 3. 上游附加的安全限制（Safety）

上游许可附带的额外条款：**禁止将本项目用于医疗、交通、武器、执法与军事用途**；使用电气/电子设备时务必做好防护并有人看管。

## 使用提醒（重要）

- 上游固件是 **GPL-2.0**，PCB/CAD 是 **CC BY-NC-SA 4.0（含非商业限制）**。
- 若本项目后续商用或采用不同许可，**不能直接搬运**这些上游文件；二次开发请遵守各自许可义务，尤其是 NC（非商业）与 SA（相同方式共享）条款。
- 本目录内容仅作学习参考；SuperPad-V1 的最终成果如需开源发布，建议基于上游另行设计，避免许可冲突。

## 致谢

感谢 **Input Labs** 团队公开 Alpakka 参考设计 —— 高质量的固件、PCB 与外壳开源资料，让 DIY 控制器开发有了扎实的起点。
