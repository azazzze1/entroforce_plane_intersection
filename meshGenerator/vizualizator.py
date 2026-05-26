#!/usr/bin/env python3
"""
Визуализатор для задачи пересечения поверхности с плоскостью.
Показывает: облако точек, границы домена, замкнутые контуры сечения.
"""

import numpy as np
import matplotlib.pyplot as plt
from matplotlib import cm
from matplotlib.colors import Normalize
from pathlib import Path
import argparse
import sys

def load_mesh(filename):
    """Загружает mesh.txt (формат: N K, точки, полигоны)"""
    with open(filename, 'r') as f:
        lines = [l.strip() for l in f if l.strip() and not l.startswith('#')]
    
    if not lines:
        raise ValueError(f"Пустой файл: {filename}")
    
    idx = 0
    N, K = map(int, lines[idx].split()); idx += 1
    
    points = []
    for _ in range(N):
        x, y, z = map(float, lines[idx].split()); idx += 1
        points.append((x, y, z))
    
    # Конвертируем полигоны в рёбра для отрисовки границ
    constraints = []
    for _ in range(K):
        parts = list(map(int, lines[idx].split())); idx += 1
        poly_len = parts[0]
        verts = parts[1:]
        for i in range(poly_len):
            u = verts[i]
            v = verts[(i + 1) % poly_len]
            constraints.append((u, v))
            
    return np.array(points), np.array(constraints)

def plot_contours_colored(ax, filename, show_arrows=True, cmap_name='Set3'):
    """Отрисовывает замкнутые контуры разными цветами с направлением"""
    if not Path(filename).exists():
        print(f"⚠️ Файл контуров не найден: {filename}")
        return
    
    with open(filename, 'r') as f:
        content = f.read().strip()
    
    if not content:
        return
        
    # Разбиваем по пустым строкам: каждый блок = один контур
    contour_blocks = [b for b in content.split('\n\n') if b.strip()]
    n_contours = len(contour_blocks)
    
    if n_contours == 0:
        return
    
    # Выбираем цвета
    cmap = cm.get_cmap(cmap_name)
    if n_contours <= 10:
        colors = [cmap(i) for i in range(n_contours)]
    else:
        norm = Normalize(vmin=0, vmax=n_contours)
        colors = [cmap(norm(i)) for i in range(n_contours)]
    
    print(f"🎨 Отрисовка {n_contours} контуров...")
    
    for idx, (block, color) in enumerate(zip(contour_blocks, colors)):
        lines = [l.strip() for l in block.split('\n') if l.strip()]
        if len(lines) < 2:
            continue
            
        points = [tuple(map(float, line.split())) for line in lines]
        xs, ys = zip(*points)
        
        # Рисуем линию
        label = f'Контур #{idx+1}' if idx == 0 else None
        ax.plot(xs, ys, color=color, linewidth=2.5, label=label, zorder=5)
        
        # Стрелки направления
        if show_arrows and len(points) >= 3:
            step = max(1, len(points) // 8)
            for i in range(0, len(points) - 1, step):
                x0, y0 = points[i]
                x1, y1 = points[i+1]
                dx, dy = x1 - x0, y1 - y0
                length = np.hypot(dx, dy)
                if length > 1e-6:
                    arrow_len = 0.3 * length
                    ax.arrow(x0, y0, 
                            dx * arrow_len / length, 
                            dy * arrow_len / length,
                            head_width=0.4, head_length=0.6, 
                            fc=color, ec=color, alpha=0.8, zorder=6)
    
    if n_contours <= 15:
        ax.legend(loc='upper right', fontsize=8, framealpha=0.9)

def visualize_mesh(filename, output=None, cut_height=None, contours_file="contours_final.txt"):
    points, constraints = load_mesh(filename)
    x, y, z = points[:, 0], points[:, 1], points[:, 2]
    
    fig, ax = plt.subplots(1, 1, figsize=(10, 8))
    
    # 1. Облако точек (раскраска по Z)
    scatter = ax.scatter(x, y, c=z, s=5, cmap='viridis', alpha=0.5, edgecolors='none')
    plt.colorbar(scatter, ax=ax, label='Z-координата', pad=0.02)
    
    # 2. Границы домена (красные)
    for u, v in constraints:
        ax.plot([x[u], x[v]], [y[u], y[v]], 'r-', linewidth=1.0, alpha=0.8, label='Граница' if u==constraints[0][0] else "")
    
    # 3. Контуры сечения (цветные, со стрелками)
    plot_contours_colored(ax, contours_file, show_arrows=True, cmap_name='Set3')
    
    # Настройки отображения
    ax.set_xlabel("X"); ax.set_ylabel("Y")
    ax.set_aspect('equal')
    ax.grid(alpha=0.3, linestyle='--')
    
    title = f"Сечение поверхности при Z = {cut_height}" if cut_height is not None else "Визуализация поверхности"
    ax.set_title(title)
    
    plt.tight_layout()
    
    if output:
        plt.savefig(output, dpi=150, bbox_inches='tight')
        print(f"💾 Сохранено: {output}")
    else:
        plt.show()

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Визуализация сечения поверхности")
    parser.add_argument("file", nargs="?", default="mesh.txt", help="Путь к mesh.txt")
    parser.add_argument("-o", "--output", help="Сохранить в файл (png/svg)")
    parser.add_argument("-z", "--height", type=float, help="Высота среза (для заголовка)")
    parser.add_argument("-c", "--contours", default="contours_final.txt", help="Файл с контурами")
    
    args = parser.parse_args()
    
    try:
        visualize_mesh(args.file, args.output, args.height, args.contours)
    except FileNotFoundError as e:
        print(f"❌ Файл не найден: {e}")
        sys.exit(1)
    except Exception as e:
        print(f"❌ Ошибка: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)