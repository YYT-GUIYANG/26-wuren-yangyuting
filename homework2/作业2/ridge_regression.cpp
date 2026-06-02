//作业2-2 Ridge回归，L2正则
//在作业2-1基础上，梯度多加2*lambda*w，防止权重太大
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cmath>
using namespace std;
vector<vector<double>> read_data(string path,int n_feat)
{
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
class RidgeRegression
{
public:
    vector<double> w;
    double buchang;
    int counters;
    double lambda;//正则系数，PPT上说λ
    RidgeRegression(int dim,double learning_rate,int n_epochs,double lam)
    {
        w.assign(dim,0);
        buchang=learning_rate;
        counters=n_epochs;
        lambda=lam;
    }
    double predict(vector<double> x)
    {
        double y_hat=w[0];
        for(int j=1;j<(int)w.size();j++)
            y_hat+=w[j]*x[j-1];
        return y_hat;
    }
    void fit(vector<vector<double>> data,int n_feat)
    {   
    /* 流程：跟普通线性回归一样，但梯度对w1~wn多加2*lambda*w
         局部变量：n,grad,err
         成员变量：w更新，lambda不变*/
        int n=data.size();
        for(int ep=0;ep<counters;ep++)
          {
            vector<double> grad(w.size(),0);
            for(int i=0;i<n;i++){
                vector<double> x(data[i].begin(),data[i].begin()+n_feat);
                double y=data[i][n_feat];
                double err=predict(x)-y;
                grad[0]+=err;
                for(int j=1;j<(int)w.size();j++)
                    grad[j]+=err*x[j-1];
            }
            for(int j=0;j<(int)w.size();j++)
            {
                grad[j]=grad[j]/n;
                if(j>0)
                    grad[j]+=2*lambda*w[j];
                w[j]-=buchang*grad[j];
            }
        }
    }
    double mse(vector<vector<double>> data,int n_feat)
   {
        double s=0;
        for(auto row:data)
        {
            vector<double> x(row.begin(),row.begin()+n_feat);
            double e=predict(x)-row[n_feat];
            s+=e*e;
        }
        return s/data.size();
    }
};
int main()
{
    int n_feat=8;
    auto train=read_data("data/train.csv",n_feat);
    auto test=read_data("data/test.csv",n_feat);
    double lam=0.01;//试过0.1太大，0.01还行
    RidgeRegression model(n_feat+1,0.05,800,lam);
    model.fit(train,n_feat);
    cout<<"Ridge lambda="<<lam<<endl;
    cout<<"训练MSE:"<<model.mse(train,n_feat)<<endl;
    cout<<"测试MSE:"<<model.mse(test,n_feat)<<endl;
    ofstream fout("data/pred_ridge.csv");
    fout<<"y_true,y_pred\n";
    for(auto row:test)
    {
        vector<double> x(row.begin(),row.begin()+n_feat);
        fout<<row[n_feat]<<","<<model.predict(x)<<"\n";
    }
    fout.close();
    cout<<"预测写进data/pred_ridge.csv了"<<endl;
    return 0;
}
