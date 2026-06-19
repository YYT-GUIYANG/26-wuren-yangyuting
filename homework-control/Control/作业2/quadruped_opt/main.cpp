#include<Eigen/Dense>
#include<cmath>
#include<iomanip>
#include<iostream>
#include<vector>
extern"C" 
{
#include "osqp.h"
}
using namespace std;
namespace 
{
constexpr double kTargetX=3.0;
constexpr double kTargetY=3.0;
constexpr double kTolerance=1e-3;
constexpr int kMaxIter=100000;
Eigen::Vector2d gradient(const Eigen::Vector2d& state) 
{
    return{state(0)-kTargetX,10.0*(state(1)-kTargetY)};
}
double objective(const Eigen::Vector2d& state) 
{
    const Eigen::Vector2d diff=state-Eigen::Vector2d(kTargetX,kTargetY);
    return 0.5*diff(0)*diff(0)+5.0*diff(1)*diff(1);
}
int runGradientDescent(double eta) 
{
    Eigen::Vector2d state(0.0,0.0);
    for(int iter=1;iter<=kMaxIter;++iter) 
    {
        state-=eta*gradient(state);
        const double error=(state-Eigen::Vector2d(kTargetX,kTargetY)).norm();
        if (error<kTolerance) 
        {
            cout <<"eta="<<setw(6)<<eta<<"->iter="<<iter<<",state=("<<state(0)<<","<<state(1)<<")"<<",f="<<objective(state)<<'\n';
            return iter;
        }
    }
    cout<<"eta="<<setw(6)<<eta<<"->未在最大迭代次数内收敛\n";
    return -1;
}
void solveConstrainedWithOsqp()
{
    c_int n=2;
    c_int m=1;
    c_float P_x[2]={1.0f,10.0f};
    c_int P_i[2]={0,1};
    c_int P_p[3]={0,1,2};
    c_float q[2]={-3.0f,-30.0f};
    c_float A_x[2]={1.0f,1.0f};
    c_int A_i[2]={0,0};
    c_int A_p[3]={0,1,2};
    c_float l[1]={-OSQP_INFTY};
    c_float u[1]={4.0f};
    OSQPSettings settings;
    osqp_set_default_settings(&settings);
    settings.verbose=0;
    settings.eps_abs=1e-6;
    settings.eps_rel=1e-6;
    OSQPData data{};
    data.n=n;
    data.m=m;
    data.P=csc_matrix(n,n,2,P_x,P_i,P_p);
    data.q=q;
    data.A=csc_matrix(m,n,2,A_x,A_i,A_p);
    data.l=l;
    data.u=u;
    OSQPWorkspace*work=nullptr;
    c_int setup_status=osqp_setup(&work,&data,&settings);
    if(setup_status!=0) 
    {
        //cerr<<"OSQP"<<setup_status<<'\n';
        c_free(data.A);
        c_free(data.P);
        return;
    }
    osqp_solve(work);
    cout<<"作业三:OSQP 约束二次规划结果\n";
    cout<<"状态"<<work->info->status<<'\n';
    cout<<"最优解x="<< work->solution->x[0]
        <<",y="<<work->solution->x[1]<<'\n';
    cout<<"约束x+y="<<work->solution->x[0]+work->solution->x[1]<<'\n';
    cout<<"目标函数值f="
              <<objective({work->solution->x[0],work->solution->x[1]})<<'\n';
    osqp_cleanup(work);
    c_free(data.A);
    c_free(data.P);
}
} 
int main() 
{
    cout<<fixed<<setprecision(6);
    cout<<"目标点:("<<kTargetX<<","<<kTargetY<<")\n";
    cout<<"收敛阈值:"<<kTolerance<<'\n';
    cout<<"问题一：梯度下降(这里采用不同的学习率)\n";
    const vector<double>learning_rates={0.05,0.1,0.2,0.4,0.8,1.0,1.2};
    for(const double eta:learning_rates) 
    {
        runGradientDescent(eta);
    }
    cout<<"问题二\n";
    cout <<"约束x+y<=4下最优解:(13/11, 31/11),mu=20/11\n";
    solveConstrainedWithOsqp();
    return 0;
}
