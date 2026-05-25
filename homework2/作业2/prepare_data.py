#和01_多元线性回归.ipynb里预处理一样，单独跑也行
import csv
import math
import random
import os
ROOT=os.path.dirname(os.path.abspath(__file__))
DATA_DIR=os.path.join(ROOT,"data")
RAW=os.path.join(DATA_DIR,"housing.csv")
FEAT=["longitude","latitude","housing_median_age","total_rooms","total_bedrooms","population","households","median_income"]
rows=[]
with open(RAW,newline="",encoding="utf-8") as f:
    for row in csv.DictReader(f):
        try:
            x=[float(row[c]) for c in FEAT]
            y=float(row["median_house_value"])/100000.0
            rows.append(x+[y])
        except:
            pass
for j in range(len(FEAT)):
    vals=[r[j] for r in rows if not math.isnan(r[j])]
    med=sorted(vals)[len(vals)//2]
    for r in rows:
        if math.isnan(r[j]):
            r[j]=med
random.seed(42)
random.shuffle(rows)
n_train=int(len(rows)*0.8)
train,test=rows[:n_train],rows[n_train:]
means=[sum(r[i] for r in train)/len(train) for i in range(len(FEAT))]
stds=[]
for i in range(len(FEAT)):
    v=sum((r[i]-means[i])**2 for r in train)/len(train)
    stds.append(math.sqrt(v) if v>1e-12 else 1.0)
def norm(data):
    out=[]
    for r in data:
        x=[(r[i]-means[i])/stds[i] for i in range(len(FEAT))]
        out.append(x+[r[-1]])
    return out
train,test=norm(train),norm(test)
os.makedirs(DATA_DIR,exist_ok=True)
for path,data in [("train.csv",train),("test.csv",test)]:
    with open(os.path.join(DATA_DIR,path),"w",newline="") as f:
        w=csv.writer(f)
        for r in data:
            w.writerow(r)
print("训练",len(train),"测试",len(test))
