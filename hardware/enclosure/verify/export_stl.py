# -*- coding: utf-8 -*-
"""
从 enclosure.blend 重新导出 3 个交付 STL（几何修改后的维护工具）— 用法:
  blender --background enclosure.blend --python export_stl.py -- [输出目录=默认 ../stl]
导出映射（对象名 -> 文件）:
  NB_back                 -> bottom.stl
  NB_front                -> top.stl
  Keycap_1..13 + PressRod_S17..S20 + Slider_L1/R1 + EncExt (20 件) -> rods.stl
占位对象（PCB_Reference 集合、JoyProbe_*、S15_L1/S16_R1 占位键、光源/相机）不导出。
导出后必须重跑 verify/enclosure_verify.py 并更新 README SHA 表/三角数/体积。
Blender 5.x 使用 bpy.ops.wm.stl_export（旧 export_mesh.stl 已移除）。
"""
import bpy, os, sys


def get_output_dir():
    default = os.path.join(os.path.dirname(os.path.abspath(bpy.data.filepath)), "..", "stl")
    argv = sys.argv
    if "--" in argv:
        rest = argv[argv.index("--") + 1:]
        if rest:
            return os.path.abspath(rest[0])
    return os.path.abspath(default)


RODS_SINGLES = {"Slider_L1", "Slider_R1", "EncExt"}


def main():
    outdir = get_output_dir()
    if not os.path.isdir(outdir):
        os.makedirs(outdir)
    mesh = [o for o in bpy.data.objects if o.type == "MESH"]

    def export(name, objs):
        bpy.ops.object.select_all(action="DESELECT")
        for o in objs:
            o.select_set(True)
        bpy.context.view_layer.objects.active = objs[0]
        path = os.path.join(outdir, name)
        bpy.ops.wm.stl_export(
            filepath=path, export_selected_objects=True, ascii_format=False,
            global_scale=1.0, use_batch=False, apply_modifiers=True)
        print("exported %s (%d objs) -> %s" % (name, len(objs), path))

    back = [o for o in mesh if o.name == "NB_back"]
    front = [o for o in mesh if o.name == "NB_front"]
    rods = [o for o in mesh if o.name.startswith("Keycap_") or o.name.startswith("PressRod_") or o.name in RODS_SINGLES]
    rods.sort(key=lambda o: o.name)

    if not back or not front:
        print("ERROR: NB_back/NB_front not found")
        sys.exit(1)
    if len(rods) != 20:
        print("ERROR: rods component count = %d (expected 20)" % len(rods))
        sys.exit(1)

    export("bottom.stl", back)
    export("top.stl", front)
    export("rods.stl", rods)
    print("export complete -> %s" % outdir)


if __name__ == "__main__":
    main()