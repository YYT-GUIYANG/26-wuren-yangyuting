//作业2-1多元线性回归，加州房价
//C++手搓，没用现成的机器学习库
//数据是prepare_data.py先处理好train.csv和test.csv
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cmath>
using namespace std;
//读csv，一行8个特征+1个房价y
vector<vector<double>> read_data(string path,int n_feat){
    vector<vector<double>> data;
    ifstream fin(path);
    string line;
    while(getline(fin,line))
    {
        stringstream ss(line);
        string cell;
        vector<double> row;
        while(getline(ss,cell,','))
            row.push_back(stod(cell));
        if((int)row.size()==n_feat+1)
            data.push_back(row);
    }
    return data;
}
class LinearRegression
{
public:
    vector<double> w;//w[0]是偏置b，后面是各特征权重，课上讲的w和b
    double buchang;//学习率
    int counters;//迭代多少次，梯度下降要循环
    LinearRegression(int dim,double learning_rate,int n_epochs)
    {
        w.assign(dim,0);
        buchang=learning_rate;
        counters=n_epochs;
    }
    double predict(vector<double> x)
    {
        //预测y_hat=w0+w1*x1+...，先加偏置
        double y_hat=w[0];
        for(int j=1;j<(int)w.size();j++)
            y_hat+=w[j]*x[j-1];
        return y_hat;
    }
    void fit(vector<vector<double>> data,int n_feat)
    {
    /*流程：外层循环counters次，每次算所有样本梯度，更新w
         局部变量：n样本数，grad梯度，err预测误差
         成员变量：w被更新，buchang和counters不变
         依赖：predict*/
        int n=data.size();
        for(int ep=0;ep<counters;ep++)
        {
            vector<double> grad(w.size(),0);
            for(int i=0;i<n;i++)
            {
                vector<double> x(data[i].begin(),data[i].begin()+n_feat);
                double y=data[i][n_feat];
                double err=predict(x)-y;//预测减真实
                grad[0]+=err;
                for(int j=1;j<(int)w.size();j++)
                    grad[j]+=err*x[j-1];
            }
            for(int j=0;j<(int)w.size();j++)
                w[j]-=buchang*grad[j]/n;//梯度下降，buchang别太大不然发散
        }
    }
    double mse(vector<vector<double>> data,int n_feat){
        //均方误差，越小越好，用来检查拟合效果
        double s=0;
        for(auto row:data){
            vector<double> x(row.begin(),row.begin()+n_feat);
            double e=predict(x)-row[n_feat];
            s+=e*e;
        }
        return s/data.size();
    }
};
int main()
{
    int n_feat=8;//8个特征，ocean那个分类变量没加，太麻烦
    auto train=read_data("data/train.csv",n_feat);
    auto test=read_data("data/test.csv",n_feat);
    LinearRegression model(n_feat+1,0.05,800);//0.05和800是试出来的
    model.fit(train,n_feat);
    cout<<"训练MSE:"<<model.mse(train,n_feat)<<endl;
    cout<<"测试MSE:"<<model.mse(test,n_feat)<<endl;
    ofstream fout("data/pred_linear.csv");
    fout<<"y_true,y_pred\n";
    for(auto row:test)
    {
        vector<double> x(row.begin(),row.begin()+n_feat);
        fout<<row[n_feat]<<","<<model.predict(x)<<"\n";
    }
    fout.close();
    cout<<"预测写进data/pred_linear.csv了"<<endl;
    return 0;
}
