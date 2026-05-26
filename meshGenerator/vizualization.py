import numpy as np, matplotlib.pyplot as plt, sys, argparse
from pathlib import Path

CONTOUR_COLORS = [
    '#E6194B', '#3CB44B', '#FFE119', '#4363D8', '#F58231',
    '#911EB4', '#46F0F0', '#F032E6', '#BCF60C', '#FABEBE'
]

def load_mesh(fn):
    with open(fn) as f:
        ln = [l.split() for l in f if l.strip() and not l.startswith('#')]
    N, K = map(int, ln[0]); i = 1
    pts = [list(map(float, ln[i+j][:3])) for j in range(N)]; i += N
    cons = []
    for _ in range(K):
        v = list(map(int, ln[i])); i += 1
        for j in range(v[0]): cons.append((v[j+1], v[(j%v[0])+1]))
    return np.array(pts), cons

def plot_contours(ax, fn, arrows=True):
    if not Path(fn).exists(): return
    with open(fn) as f: blocks = [b for b in f.read().strip().split('\n\n') if b.strip()]
    for idx, blk in enumerate(blocks):
        pts = [tuple(map(float, l.split())) for l in blk.split('\n') if l.strip()]
        if len(pts) < 2: continue
        xs, ys = zip(*pts)
        col = CONTOUR_COLORS[idx % len(CONTOUR_COLORS)]
        ax.plot(xs, ys, color=col, lw=2.5, label=f'#{idx+1}' if idx==0 else "", zorder=5)
        if arrows and len(pts) >= 3:
            step = max(1, len(pts)//8)
            for i in range(0, len(pts)-1, step):
                dx, dy = pts[i+1][0]-pts[i][0], pts[i+1][1]-pts[i][1]
                l = np.hypot(dx, dy)
                if l > 1e-6:
                    ax.arrow(pts[i][0], pts[i][1], dx*0.3, dy*0.3,
                            head_width=0.4, fc=col, ec=col, alpha=0.8, zorder=6)
    if len(blocks) <= 15: ax.legend(fontsize=8, loc='upper right', framealpha=0.9)

def viz(mesh_fn, out=None, zlbl=None, cont_fn="contours_final.txt"):
    pts, cons = load_mesh(mesh_fn)
    x, y, z = pts.T
    plt.figure(figsize=(10, 8))
    sc = plt.scatter(x, y, c=z, s=5, cmap='viridis', alpha=0.5)
    plt.colorbar(sc, label='Z', pad=0.02)
    for u,v in cons: plt.plot([x[u],x[v]], [y[u],y[v]], 'r-', lw=1, alpha=0.8)
    plot_contours(plt.gca(), cont_fn)
    plt.xlabel('X'); plt.ylabel('Y'); plt.gca().set_aspect('equal')
    plt.grid(alpha=0.3, ls='--')
    if zlbl: plt.title(f'Z = {zlbl}')
    plt.tight_layout()
    if out: plt.savefig(out, dpi=150, bbox_inches='tight'); print(f'Saved: {out}')
    else: plt.show()

if __name__ == '__main__':
    p = argparse.ArgumentParser()
    p.add_argument('file', nargs='?', default='mesh.txt')
    p.add_argument('-o', '--output'); p.add_argument('-z', '--height', type=float)
    p.add_argument('-c', '--contours', default='contours_final.txt')
    a = p.parse_args()
    try: viz(a.file, a.output, a.height, a.contours)
    except Exception as e: print(f'Error: {e}'); sys.exit(1)