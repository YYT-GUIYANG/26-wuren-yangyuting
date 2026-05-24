#作业2说明
数据来源（加州房价 California Housing）
原始文件在：`data/housing.csv`
**从哪里下载：**
- GitHub（书里配套数据）：https://github.com/ageron/handson-ml2/tree/master/datasets/housing
- 直接下 csv：https://raw.githubusercontent.com/ageron/handson-ml2/master/datasets/housing/housing.csv
**数据大概长什么样：**
- 约 **20640** 条（2 万多条）加州各区块的统计记录
- 原始 csv 有 10 列，例如：
  - `longitude`、`latitude`：经纬度
  - `housing_median_age`：房龄中位数
  - `total_rooms`、`total_bedrooms`：总房间数、卧室数
  - `population`、`households`：人口、户数
  - `median_income`：收入中位数
  - `median_house_value`：**房价中位数（要预测的目标）**
  - `ocean_proximity`：离海远近（文字类别，我预处理时没用到）
这里是deepseek帮我寻找的数据，理由是比较经典，但是具体经典在哪里没有看出来！

文件结构：
1.linear_regression.cpp：普通多元线性回归
2.ridge_regression.cpp：加L2正则
3.prepare_data.py：处理housing.csv（这个是deepseek修改处理过的，非完全手搓，这个数据的预处理部分还有一些没有弄懂）
4.01_多元线性回归.ipynb
5.02_正则化回归.ipynb

