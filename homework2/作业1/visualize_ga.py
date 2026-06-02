#作业1画图，C++跑完make ex_so1后再运行
#不会用matplotlib，对着教程抄的
import os
import numpy as np
import matplotlib
matplotlib.use('Agg')#服务器上没界面，直接存png
import matplotlib.pyplot as plt
f="openGA/bin/result_so1.txt"
if not os.path.exists(f):
    print("先cd openGA，make ex_so1")
    exit(1)
data=np.loadtxt(f,skiprows=1)
step=data[:,0]
x=data[:,1]
y=data[:,2]
cost_avg=data[:,3]
cost_best=data[:,4]
#图1看cost有没有降下来
plt.figure(figsize=(6,4))
plt.plot(step,cost_best,'r-o',markersize=3,label='best')
plt.plot(step,cost_avg,'b--',label='avg')
plt.xlabel('generation')
plt.ylabel('cost')
plt.title('GA')
plt.legend()
plt.grid(True,alpha=0.3)
plt.savefig('openGA/bin/ga_convergence.png',dpi=120)
plt.close()
#图2看最优xy怎么往原点靠
lim=10
gx=np.linspace(-lim,lim,100)
gy=np.linspace(-lim,lim,100)
GX,GY=np.meshgrid(gx,gy)
GZ=GX*GX+GY*GY
plt.figure(figsize=(5,5))
plt.contour(GX,GY,GZ,levels=15,alpha=0.5)
plt.plot(x,y,'b.-',markersize=4)
plt.plot(0,0,'r*',markersize=10)
plt.xlabel('x')
plt.ylabel('y')
plt.title('path')
plt.savefig('openGA/bin/ga_search_path.png',dpi=120)
plt.close()
print("图在openGA/bin/")
