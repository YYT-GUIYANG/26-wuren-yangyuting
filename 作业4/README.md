#作业4 K-Means聚类
##数据从哪来
原始文件：`cluster_dataset.mat`
里面是二维点坐标，变量名叫`data`，用`loadmat`读：
```python
data=loadmat('cluster_dataset.mat')
X=data['data']
```
##我填的思路（很简单）
1.随机选k个点当初中心
2.每个点找最近的中心，分到那一簇
3.中心改成簇里点的平均值
4.重复2-3直到中心不变
5.画k-cost曲线选k，我看图选的是3
##文件
-Kmeans_Exp.ipynb：主作业
-cluster_dataset.mat：数据
