import numpy as np
import matplotlib.pyplot as plt 
import matplotlib.patches as pat 
from collections import deque 
from maze_8x8 import  walls 

maze_size = 8
start = (7, 0)
goal = (maze_size//2 , maze_size//2)

DIRS = [(-1, 0), (0, 1), (1, 0), (0, -1)]#Up, right, down, left  
wall_map = {}
for y, x, l, r, t, b in walls:
    wall_map[(y, x)] = {'L': l, 'R': r, 'T': t, 'B': b}

def has_wall(y, x, ny, nx):

    if ny == y and nx == x+1: #right 
       return wall_map.get((y, x), {}).get('R', 0)
    elif ny == y and nx == x-1: #left
        return wall_map.get((y, x), {}).get('L', 0)
    elif ny == y-1 and nx == x: #up
        return wall_map.get((y, x), {}).get('T', 0)
    elif ny == y+1 and nx == x: #down 
        return wall_map.get((y, x), {}).get('B', 0)
    return True 

def flood_fill(goal): # the actual values on the map
    flood = np.full((maze_size, maze_size), np.inf)
    flood[goal] = 0
    q = deque([goal])

    while q:
        y, x = q.popleft()
        for dy, dx  in DIRS: 
            ny, nx = y + dy, x + dx
            if 0 <= ny < maze_size and 0 <= nx < maze_size:
                if not has_wall(y, x, ny, nx):
                    if flood[ny, nx] > flood[y, x] + 1:
                        flood[ny, nx] = flood[y, x] + 1
                        q.append((ny, nx))
    return flood 

def extract_path(flood, start, goal): # the decision is made based on the  min value 
    path = [start]
    current = start

    while current != goal:
        y,  x  = current 
        neighbors = []
        for dy, dx in DIRS:
            ny, nx = y + dy, x + dx
            if 0 <= ny < maze_size and 0 <= nx < maze_size:
                if not has_wall(y, x, ny, nx):
                    neighbors.append ((ny, nx))
        current = min(neighbors, key=lambda p: flood[p])
        path.append(current)
    return path 
def draw_walls(ax):
    for (y, x), w in wall_map.items():
        if w['T']: ax.plot([x, x+1], [y, y], 'k', lw = 4)
        if w['B']: ax.plot([x, x+1], [y+1, y+1], 'k', lw=4)
        if w['L']: ax.plot([x, x], [y, y+1], 'k', lw = 4)
        if w['R']: ax.plot([x+1, x+1], [y, y+1], 'k', lw = 4)

flood_map = flood_fill(goal)
path = extract_path(flood_map, start, goal)

fig, ax = plt.subplots(figsize = (6,  6))
ax.set_aspect('equal')

for i in range(maze_size+1):
    ax.axhline(i, color='gray', lw=1)
    ax.axvline(i, color='gray', lw=1)

for y in range(maze_size):
    for x in range(maze_size):
        if np.isinf(flood_map[y, x]):
            ax.text(x + 0.5, y + 0.5, "∞",
                    ha = 'center', va = 'center', color = 'red')
        else:
            ax.text(x + 0.5, y + 0.5, int(flood_map[y, x]),
                    ha = 'center', va  ='center')
            
ax.add_patch(pat.Rectangle((start[1], start[0]),1, 1,
                            facecolor = 'lightgreen', alpha = 0.6))
ax.add_patch(pat.Rectangle((goal[1], goal[0]), 1, 1,
                           facecolor='lightcoral', alpha = 0.6))
draw_walls(ax)

for (y1, x1), (y2, x2) in zip(path , path [1:]):
    ax.plot([x1+0.5, x2+0.5], [y1+0.5, y2+0.5],
            'g', lw= 3)

ax.set_xlim(0, maze_size)
ax.set_ylim(maze_size, 0)
ax.set_xticks([])
ax.set_yticks([])
plt.tight_layout()
plt.show()

