#作业4 K-Means聚类

##数据从哪来
原始文件：`cluster_dataset.mat`（老师发的，和Kmeans_Exp.ipynb在同一文件夹）

里面是二维点坐标，变量名叫`data`，用`loadmat`读：
```python
data=loadmat('cluster_dataset.mat')
X=data['data']
```

##怎么跑
用Jupyter或VSCode打开`Kmeans_Exp.ipynb`，从上到下运行。
把模板里几个函数（init_centers、distance等）填好就行。

##我填的思路（很简单）
1.随机选k个点当初中心
2.每个点找最近的中心，分到那一簇
3.中心改成簇里点的平均值
4.重复2-3直到中心不变
5.画k-cost曲线选k，我看图选的是3

##文件
-Kmeans_Exp.ipynb：主作业
-cluster_dataset.mat：数据
