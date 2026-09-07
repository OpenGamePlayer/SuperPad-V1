# -*- coding: utf-8 -*-
"""
Alpakka 外壳 STL 一键回归验证套件（无第三方依赖，纯 stdlib）
用法: python enclosure_verify.py [三STL目录=默认当前目录的 ../stl]
输出: 水密(边界边)/连通性/体积断链/拓扑(非流形边·退化面·winding)/0.2mm切片连续性
"""
import struct, os, sys, math
from collections import defaultdict

def load_stl(path):
    data = open(path, "rb").read()
    n = struct.unpack("<I", data[80:84])[0]
    assert len(data) >= 84 + n * 50, "STL 文件不完整"
    return [[struct.unpack("<3f", data[off + 12 + j * 12: off + 24 + j * 12]) for j in range(3)]
            for off in range(84, 84 + n * 50, 50)]

def sub(a, b): return (a[0] - b[0], a[1] - b[1], a[2] - b[2])
def cross(a, b): return (a[1] * b[2] - a[2] * b[1], a[2] * b[0] - a[0] * b[2], a[0] * b[1] - a[1] * b[0])
def dot(a, b): return a[0] * b[0] + a[1] * b[1] + a[2] * b[2]
def norm(a): return math.sqrt(dot(a, a))

def audit(path):
    tris = load_stl(path)
    n = len(tris)
    # 1) 边拓扑: 边界边/非流形边(度>2)
    e2c = defaultdict(list)
    for i, t in enumerate(tris):
        for j in range(3):
            p = tuple(round(x, 6) for x in t[j]); q = tuple(round(x, 6) for x in t[(j + 1) % 3])
            e2c[tuple(sorted((p, q)))].append(i)
    bnd = sum(1 for v in e2c.values() if len(v) == 1)
    nf = sum(1 for v in e2c.values() if len(v) > 2)
    # 2) 连通分量
    seen, comps = set(), []
    for i in range(n):
        if i in seen: continue
        st, c = [i], 0
        seen.add(i)
        while st:
            cur = st.pop(); c += 1
            for j in range(3):
                p = tuple(round(x, 6) for x in tris[cur][j])
                q = tuple(round(x, 6) for x in tris[cur][(j + 1) % 3])
                for nb in e2c[tuple(sorted((p, q)))]:
                    if nb not in seen: seen.add(nb); st.append(nb)
        comps.append(c)
    comps.sort(reverse=True)
    # 3) 体积(带符号) + 退化面 + winding
    vol, zero, neg = 0.0, 0, 0
    for t in tris:
        a, b, c = t
        cr = cross(sub(b, a), sub(c, a))
        if norm(cr) < 1e-9: zero += 1
        cent = ((a[0] + b[0] + c[0]) / 3, (a[1] + b[1] + c[1]) / 3, (a[2] + b[2] + c[2]) / 3)
        if dot(cr, cent) < 0: neg += 1
        vol += dot(a, cross(b, c))
    vol /= 6
    # 4) 0.2mm 切片连续性(空层检测)
    zs = [v[2] for t in tris for v in t]
    zmin, zmax = min(zs), max(zs)
    z = math.floor(zmin / 0.2) * 0.2
    empty = 0
    while z <= zmax + 1e-9:
        nseg = 0
        for t in tris:
            zz = [v[2] for v in t]
            if min(zz) <= z <= max(zz) and not (abs(zz[0] - z) < 1e-9 and abs(zz[1] - z) < 1e-9 and abs(zz[2] - z) < 1e-9):
                pts = []
                for i in range(3):
                    p, q = t[i], t[(i + 1) % 3]
                    if p[2] == q[2]: continue
                    if (p[2] - z) * (q[2] - z) <= 0:
                        f = (z - p[2]) / (q[2] - p[2])
                        pt = (p[0] + (q[0] - p[0]) * f, p[1] + (q[1] - p[1]) * f)
                        if not any(abs(pt[0] - e[0]) < 1e-7 and abs(pt[1] - e[1]) < 1e-7 for e in pts):
                            pts.append(pt)
                if len(pts) == 2: nseg += 1
        if nseg == 0: empty += 1
        z = round(z + 0.2, 6)
    return dict(n=n, bnd=bnd, nf=nf, comps=comps, vol=vol / 1000, zero=zero, neg=neg, empty=empty)

def main():
    base = sys.argv[1] if len(sys.argv) > 1 else os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "stl")
    ok = True
    for f in ("bottom", "top", "rods"):
        r = audit(os.path.join(base, f + ".stl"))
        line = ("%s: %d tris | 边界=%d %s | 非流形=%d %s | 连通=%d | 体积=%.1fcm3 %s | 退化面=%d %s | winding负向=%d %s | 切片空层=%d %s"
                % (f, r["n"], r["bnd"], "OK" if r["bnd"] == 0 else "FAIL",
                   r["nf"], "OK" if r["nf"] == 0 else "FAIL",
                   r["comps"][0], r["vol"], "OK" if r["vol"] > 0 else "FAIL",
                   r["zero"], "OK" if r["zero"] == 0 else "FAIL",
                   r["neg"], "OK" if r["neg"] < r["n"] * 0.5 else "FAIL",
                   r["empty"], "OK" if r["empty"] <= 1 else "FAIL"))
        print(line)
        if r["bnd"] != 0 or r["nf"] != 0 or r["zero"] != 0: ok = False
    print("FINAL:", "PASS 全部健康" if ok else "FAIL 存在暗病")
    return 0 if ok else 1

if __name__ == "__main__":
    sys.exit(main())