# PlatformIO pre-build hook: 生成上游 version.sh 产出的 src/headers/version.h
# 上游用 Makefile 的 `make version` 生成该文件；PlatformIO 里等价于本钩子。
# 用法：platformio.ini 中  extra_scripts = pre:scripts/gen_version.py
import os

Import("env")

HEADER = os.path.join(env.subst("$PROJECT_DIR"), "src", "headers", "version.h")

def generate_version_header():
    os.makedirs(os.path.dirname(HEADER), exist_ok=True)
    # 本地有 git 时优先用 tag/短哈希；否则回退默认版本。
    content = '#define VERSION "0.1.0"\n'
    try:
        import subprocess
        tag = subprocess.run(
            ["git", "describe", "--tags"], capture_output=True, text=True, cwd=env.subst("$PROJECT_DIR")
        ).stdout.strip()
        if tag:
            content = f'#define VERSION "{tag}"\n'
    except Exception:
        pass
    with open(HEADER, "w", encoding="utf-8") as f:
        f.write(content)

generate_version_header()
