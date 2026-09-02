# 嘉立创 EDA 专业版导入 Alpakka 工程指南

> 目标：把 Input Labs Alpakka 的 **KiCad 8 参考设计**（本目录 `alpakka_kicad/`）导入到**嘉立创 EDA 专业版**，生成可编辑的 `.eprj2` 工程，作为 SuperPad-V1 控制器的硬件起点。

## 为什么不能直接给你 `.eprj2`

`.eprj2` 是嘉立创 EDA 专业版的私有工程格式，**没有命令行转换工具**，只能在嘉立创 EDA 专业版软件里通过「打开 KiCad 文件」由软件自动导入。所以这一步需要你在软件界面里完成（约几分钟），本仓库已把官方 KiCad 源文件整理好，导入即可。

## 前置条件

- 安装 **嘉立创 EDA 专业版**（官网 https://lceda.cn 下载，登录免费账号即可用）
- 建议版本：**1.9 及以上**（对 KiCad 8 的兼容性更好）

## 导入步骤

1. **打开源文件**：菜单 `文件 → 打开 → 打开本地文件`（或直接拖拽）
   - 选择 `hardware/alpakka_kicad/projects/alpakka/alpakka.kicad_pro`
   - 也可直接选择 `.kicad_sch` / `.kicad_pcb`，软件会自动识别
2. **等待转换**：软件提示「正在转换 KiCad 工程…」，完成后会生成对应的 `.eprj2` 工程与 `alpakka_原理图` / `alpakka_PCB` 文档
3. **检查符号/封装缺失**：
   - 若弹「找不到封装」，需要在原理图编辑器里打开 `工具 → 符号管理器` / `工具 → 封装管理器` 确认
   - Alpakka 使用了大量自定义封装（位于 `symbols/ilo.kicad_fp/`），导入后**首次保存前先确认全部器件都有封装**
4. **保存为工程**：转换完成后 `文件 → 保存`，保存到你希望的工作目录（建议存到本仓库 `hardware/` 下），即得到 `xxx.eprj2`
5. **（可选）换嘉立创料号**：Alpakka 原设计按 JLCPCB 料号设计，原理图中已带 LCSC 编号，可对照 `工具 → 编辑符号字段 → 按 Group 分组` 核对

## 导入后常见问题

| 现象 | 处理 |
| --- | --- |
| 导入后网络/引脚丢失 | 检查源 `.kicad_sch` 是否有未命名网络；通常新版 EDA 自动处理 |
| 封装库路径报错 | `fp-lib-table` 中的相对路径应指向本目录 `symbols/ilo.kicad_fp/`，可按实际位置修正 |
| DRC 报错 | Alpakka 是成熟设计（官方 DRC 0 错误），导入后按嘉立创规则重跑 DRC 即可，个别警告可忽略 |
| 版本兼容 | 若你是老版本 EDA，建议先升级再导入 |

## 相关文件

| 文件 | 说明 |
| --- | --- |
| `alpakka_kicad/projects/alpakka/alpakka.kicad_pro` | Alpakka 主工程 |
| `alpakka_kicad/projects/alpakka/alpakka.kicad_sch` | 原理图 |
| `alpakka_kicad/projects/alpakka/alpakka.kicad_pcb` | PCB 布局 |
| `alpakka_kicad/projects/marmota/` | Marmota 模块（Alpakka v1 核心模块）参考设计 |
| `alpakka_kicad/symbols/ilo.kicad_sym` | 符号库（原理图用） |
| `alpakka_kicad/symbols/ilo.kicad_fp/` | 封装库（PCB 用） |
| `alpakka_kicad/plugins/kicad_ilo_export.py` | 官方 KiCad 导出插件（嘉立创生产文件用，非导入必需） |

## 许可提醒

- 本目录 `alpakka_kicad/` 内容来源于 Input Labs Alpakka，许可 **CC BY-NC-SA 4.0**（非商业、署名、相同方式共享），详见 `alpakka_kicad/license.md`
- 导入后若用于**本项目（SuperPad-V1）**：建议仅作学习/参考，最终自有硬件应基于它重新绘制或取得授权，避免商业用途冲突
