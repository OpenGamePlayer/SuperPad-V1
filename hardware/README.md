# hardware

SuperPad-V1 硬件设计目录。

## 内容

| 路径 | 说明 |
| --- | --- |
| `superpad_v1_main.eprj2` | **本项目自有硬件工程**（嘉立创 EDA 专业版 `.eprj2`） |
| `alpakka_kicad/` | Input Labs Alpakka 官方 **KiCad 8 源文件**（含 `projects/alpakka`、`projects/marmota`、`symbols`、`plugins`），用于导入嘉立创 EDA 作为参考 |
| `IMPORT_ALPAKKA_TO_LCEDA.md` | **嘉立创 EDA 专业版导入 Alpakka 工程的分步指南**（先读这个） |

## 当前状态

- [ ] 在嘉立创 EDA 专业版导入 `alpakka_kicad/`，生成可编辑的 `.eprj2`（按 `IMPORT_ALPAKKA_TO_LCEDA.md` 操作）
- [ ] 对照 Alpakka 参考设计完善 `superpad_v1_main.eprj2`
- [ ] 生成并核对 BOM（参照 `docs/` 上游 BOM 与嘉立创料号）

## 许可提醒

`alpakka_kicad/` 来自 Input Labs，许可 **CC BY-NC-SA 4.0**（非商业/署名/相同方式共享），详见其 `license.md`。自有工程 `superpad_v1_main.eprj2` 为本项目原创。
