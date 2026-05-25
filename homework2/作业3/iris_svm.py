#作业3 SVM鸢尾花分类
#题目说可以用sklearn或libsvm，我电脑sklearn能import就用它了
import numpy as np
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
from sklearn import svm
from sklearn.datasets import load_iris
from sklearn.model_selection import train_test_split
from sklearn.preprocessing import StandardScaler
from sklearn.metrics import accuracy_score,confusion_matrix
#1读数据，iris是sklearn自带的，省事
iris=load_iris()
X=iris.data
y=iris.target
print("样本数",len(y))
print("类别",iris.target_names)
#2划分，7:3，random_state固定好复现
X_train,X_test,y_train,y_test=train_test_split(X,y,test_size=0.3,random_state=0)
#SVM对特征尺度敏感，不标准化我试过效果差
scaler=StandardScaler()
X_train=scaler.fit_transform(X_train)
X_test=scaler.transform(X_test)
#3训练，kernel用rbf，C=1是默认差不多
model=svm.SVC(kernel='rbf',C=1)
model.fit(X_train,y_train)
pred=model.predict(X_test)
print("准确率",accuracy_score(y_test,pred))
print("混淆矩阵")
print(confusion_matrix(y_test,pred))
#4老师可能问核函数，我三个都跑一下
print("换核试试")
for ker in ['linear','rbf','poly']:
    m=svm.SVC(kernel=ker)
    m.fit(X_train,y_train)
    acc=accuracy_score(y_test,m.predict(X_test))
    print(ker,acc)
#5画散点，花瓣长宽比较好分，书上例子
plt.figure(figsize=(5,4))
plt.scatter(X[:,2],X[:,3],c=y,cmap='viridis',alpha=0.8)
plt.xlabel('petal length')
plt.ylabel('petal width')
plt.title('iris')
plt.savefig('iris_scatter.png')
plt.close()
print("图存好了iris_scatter.png")
