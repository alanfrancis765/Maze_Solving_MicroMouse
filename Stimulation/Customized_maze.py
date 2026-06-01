import numpy as np 
import matplotlib.pyplot as plt 
import matplotlib.patches as cir
from matplotlib.widgets import Button 

maze_size = 8
click_eps = 0.15 #click tolerances 

walls = np.zeros((maze_size, maze_size, 4), dtype = int)

start = (0.5, maze_size-0.5)    #[0.5, 0.5] is the center of a cell
goal = (maze_size/2 + 0.5 , maze_size/2 + 0.5)
 
def toggle_wall(y, x, d):

    if not(0 <= y < maze_size and 0 <= x < maze_size):
        return 
    
    walls[y, x, d] = 1 - walls[y, x, d] #add or remove the wall 
    #syncronize neighbour cell
    if d == 0 and x > 0:
        walls[y, x-1, 1] = walls[y, x, d]
    elif d == 1 and x < maze_size-1:
        walls[y, x+1, 0] = walls[y, x, d]
    elif d == 2 and y > 0:
        walls[y-1, x, 3] = walls[y, x, d]
    elif d == 3 and y < maze_size-1:
        walls[y+1, x, 2] = walls[y, x, d]

def on_click(event):
        
        if not event.inaxes:
            return 
        x = int(event.xdata)
        y = int(event.ydata)

        if not(0 <= x < maze_size and 0 <= y < maze_size):
            return 
        
        if abs(event.xdata -x) < click_eps:
            toggle_wall(y, x, 0)
        elif abs(event.xdata - (x + 1)) < click_eps:
            toggle_wall(y, x, 1)
        elif abs(event.ydata - y) < click_eps:
            toggle_wall(y, x, 2)
        elif abs(event.ydata - (y + 1)) < click_eps:
            toggle_wall(y, x, 3)

        draw_grid()

def draw_grid():
        
        ax.clear()

        for i in range(maze_size + 1):
            ax.axhline(i, lw= 2, color = 'black')
            ax.axvline(i, lw = 2, color = 'black')
        
        for y in range(maze_size):
            for x in range(maze_size):
                if walls[y, x, 0]:
                    ax.plot([x, x], [y, y+1], 'r', lw= 4)
                if walls[y, x, 1]:
                    ax.plot([x+1, x+1], [y, y+1], 'r', lw= 4)
                if walls[y, x, 2]:
                    ax.plot([x, x+1], [y, y], 'r', lw= 4)
                if walls[y, x, 3]:
                    ax.plot([x, x+1], [y+1, y+1], 'r', lw= 4)
                
        ax.add_patch(cir.Circle(start, 0.2, color = 'red'))
        ax.add_patch(cir.Circle(goal, 0.2, color = 'green'))

        ax.set_aspect('equal')
        ax.set_xlim(0, maze_size)
        ax.set_ylim(0, maze_size)
        ax.invert_yaxis()
        ax.axis('off')

        plt.draw()

def save_maze(event=None):

    with open ("maze_8x8.py", "w") as f:
        f.write("walls = [\n")
        for y in range(maze_size):
            for x in range(maze_size):
                if np.any(walls[y, x]):
                    l, r, t, b = walls[y, x]
                    f.write(f"  ({y}, {x}, {l}, {r}, {t}, {b}),\n")
        f.write("]\n")
    print("maze_8x8.py saved successfully")
#UI of the 'save button'
fig, ax = plt.subplots(figsize= (6, 6))
fig.canvas.mpl_connect('button_press_event', on_click)

ax_btn = plt.axes([0.7, .02, .2, .05])
btn =  Button(ax_btn, 'save maze')
btn.on_clicked(save_maze)

draw_grid()
plt.show()
