from matplotlib import pyplot as plt
import numpy as np
import mpl_toolkits.mplot3d.axes3d as p3
from matplotlib import animation
import matplotlib



x,y,z = [],[],[]

file  = open("traj.bin","rb")
l = file.readlines()

c = 0
xt,yt,zt = [],[],[]
for ar in l:
     arr = list(map(float,ar.decode("utf-8").strip().split(" ")))
     xt.append(arr[0])
     yt.append(arr[1])
     zt.append(arr[2])
     c+=1
     if c%1000==0:
          x.append(xt)
          y.append(yt)
          z.append(zt)
          xt,yt,zt = [],[],[]
          
def update_graph(num):
    a,b,c = np.array(x[num]), np.array(y[num]),np.array(z[num])
    graph.set_data (a,b)
    graph.set_3d_properties(c)
    title.set_text('viz, time={}'.format(num))
    return title, graph, 


fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')
title = ax.set_title('3D Test')
ax.set_zlim(-40,450)
ax.set_ylim(-40,250)
ax.set_xlim(-40,150)

graph, = ax.plot(x[0], y[0], z[0], linestyle="", marker=".")

ani = matplotlib.animation.FuncAnimation(fig, update_graph, 19, 
                               interval=350, blit=False)

writergif = animation.PillowWriter(fps=30) 
ani.save('animation.gif', writer=writergif)

plt.show()
#ani.save('animation.mp4', writer = FFwriter, fps=10)
# points, = ax.plot(x[0], y[0], z[0], '.')
# txt = fig.suptitle('')

# def update_points(num, points):
#     txt.set_text('num={:d}'.format(num)) # for debug purposes

#     # calculate the new sets of coordinates here. The resulting arrays should have the same shape
#     # as the original x,y,z
#     new_x = x[num]
#     new_y = y[num]
#     new_z = z[num]

#     # update properties
#     points.set_data(new_x,new_y)
#     points.set_3d_properties(new_z, 'z')

#     # return modified artists
#     return points,txt

# ani=animation.FuncAnimation(fig, update_points, frames=1000, fargs=( points))

# plt.show()