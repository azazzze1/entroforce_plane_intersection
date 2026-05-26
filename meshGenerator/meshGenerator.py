import numpy as np
import sys
from shapely.geometry import Polygon, Point
from shapely import contains

def generate_mesh(filename, n_target=50000):
    theta = np.linspace(0, 2*np.pi, 80, endpoint=False)
    r = 15 + 4*np.cos(3*theta) + 2*np.sin(5*theta)
    outer_verts = np.column_stack([r*np.cos(theta), r*np.sin(theta)])
    outer_poly = Polygon(outer_verts)
    
    hole1 = Polygon([(3, 3), (7, 3), (7, 7), (3, 7)])
    hole2 = Polygon([(-7, -7), (-3, -7), (-3, -3), (-7, -3)])
    
    domain = outer_poly.difference(hole1).difference(hole2)
    
    points = list(outer_verts)
    n_outer = len(outer_verts)
    
    h1_coords = np.array(hole1.exterior.coords)[:-1]
    points.extend(h1_coords)
    n_h1 = len(h1_coords)
    
    h2_coords = np.array(hole2.exterior.coords)[:-1]
    points.extend(h2_coords)
    n_h2 = len(h2_coords)
    
    minx, miny, maxx, maxy = domain.bounds
    raw_x = np.random.uniform(minx, maxx, n_target * 4)
    raw_y = np.random.uniform(miny, maxy, n_target * 4)
    raw_pts = np.column_stack([raw_x, raw_y])
    
    pts_array = np.array([Point(x, y) for x, y in raw_pts])
    inside_mask = contains(domain, pts_array)
    
    interior_pts = raw_pts[inside_mask]
    interior_pts = interior_pts[:n_target]
    points.extend(interior_pts)
    
    points = np.array(points)
    N = len(points)
    
    polygons = [
        list(range(n_outer)),
        list(range(n_outer, n_outer + n_h1)),
        list(range(n_outer + n_h1, n_outer + n_h1 + n_h2))
    ]
    K = len(polygons)
    
    x, y = points[:, 0], points[:, 1]
    z = np.sin(x * 0.4) * np.cos(y * 0.4) + 0.03 * (x**2 + y**2)
    plateau_mask = (x > 5) & (x < 10) & (y > -5) & (y < -1)
    z[plateau_mask] = 2.5
    
    with open(filename, 'w') as f:
        f.write(f"{N} {K}\n")
        for i in range(N):
            f.write(f"{x[i]:.8f} {y[i]:.8f} {z[i]:.8f}\n")
        for poly in polygons:
            f.write(f"{len(poly)}")
            for idx in poly:
                f.write(f" {idx}")
            f.write("\n")
            
    print(f"Сгенерировано: {N} точек, {K} полигонов -> {filename}")

if __name__ == "__main__":
    out_file = "mesh.txt"
    if len(sys.argv) > 1:
        out_file = sys.argv[1]
    generate_mesh(out_file, n_target=50000)