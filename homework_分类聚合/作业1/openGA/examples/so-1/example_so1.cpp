//作业1遗传算法
//我主要改这个文件，库是GitHub下的openGA，头文件openGA.hpp，结果输出到bin/result_so1.txt，后面用python画图
#include<iostream>
#include<string>
#include<fstream>
#include"openGA.hpd
//一个个体就两个基因x和y，求f(x,y)=x*x+y*y最小
struct MySolution
{
    double x;
    double y;
    std::string to_string() const
    {
        //打印用，方便看最优解
        return "{x:"+std::to_string(x)+", y:"+std::to_string(y)+"}";
    }
};
//适应度放这里，库规定要有这个结构体
struct MyMiddleCost
{
    double cost;
};
typedef EA::Genetic<MySolution,MyMiddleCost> GA_Type;
typedef EA::GenerationType<MySolution,MyMiddleCost> Generation_Type;
std::ofstream output_file;//全局变量，写每代结果，main里open
void init_genes(MySolution& p, const std::function<double(void)> &rnd01)
{
    /*
     *流程：随机初始化x和y，范围[-10,10]
     *局部变量：无，改的是参数p
     *全局变量：无
     *依赖：openGA初始化种群时会回调这个函数
     */
    p.x=20.0*rnd01()-10.0;
    p.y=20.0*rnd01()-10.0;
}
bool eval_solution(const MySolution& p, MyMiddleCost &c)
{
    /*
     *流程：算目标函数，越小越好，(0,0)时最小为0
     *局部变量：无
     *依赖：calculate_SO_total_fitness会读c.cost
     */
    c.cost=p.x*p.x+p.y*p.y;
    return true;
}
MySolution mutate(const MySolution& X_base,const std::function<double(void)> &rnd01,double shrink_scale)
{
    /*
     *流程：复制父代，加随机扰动，超出范围就重新变异
     *局部变量：X_new新个体，mu扰动幅度，in_range_x/y是否越界
     *依赖：库交叉变异步骤调用
     */
    MySolution X_new;
    bool in_range_x,in_range_y;
    double mu=0.2*shrink_scale;
    do{
        X_new=X_base;
        X_new.x+=mu*(rnd01()-rnd01());
        X_new.y+=mu*(rnd01()-rnd01());
        in_range_x=(X_new.x>=-10.0&&X_new.x<10.0);
        in_range_y=(X_new.y>=-10.0&&X_new.y<10.0);
    }while(!in_range_x||!in_range_y);
    return X_new;
}
MySolution crossover(const MySolution& X1,const MySolution& X2,const std::function<double(void)> &rnd01)
{
    /*
     *流程：随机权重混合父母，书上叫算术交叉
     *局部变量：r混合系数，X_new子代
     *依赖：库交叉时调用，据说会调两次
     */
    MySolution X_new;
    double r=rnd01();
    X_new.x=r*X1.x+(1.0-r)*X2.x;
    r=rnd01();
    X_new.y=r*X1.y+(1.0-r)*X2.y;
    return X_new;
}
double calculate_SO_total_fitness(const GA_Type::thisChromosomeType &X)
{
    /*
     *流程：单目标直接把cost当适应度返回
     *依赖：eval_solution要先算好
     */
    return X.middle_costs.cost;
}
void SO_report_generation(int generation_number,const EA::GenerationType<MySolution, MyMiddleCost> &last_generation,const MySolution& best_genes)
{
    /*
     *流程：每代打印一下，顺便写文件方便画图
     *局部变量：无
     *全局变量：output_file追加一行
     *依赖：库每代结束回调
     */
    std::cout<<"Generation ["<<generation_number<<"], Best="<<last_generation.best_total_cost<<", Average="<<last_generation.average_cost<<", Best genes=("<<best_genes.to_string()<<")"<<std::endl;
    output_file<<generation_number<<"\t"<<best_genes.x<<"\t"<<best_genes.y<<"\t"<<last_generation.average_cost<<"\t"<<last_generation.best_total_cost<<"\n";
}
int main()
{
    /*
     *流程：开文件->配参数->把上面几个函数注册给库->solve->关文件
     *局部变量：timer计时，ga_obj算法对象
     *全局变量：output_file
     *依赖：openGA.hpp的solve()
     */
    output_file.open("./bin/result_so1.txt");
    output_file<<"step\tx_best\ty_best\tcost_avg\tcost_best\n";
    EA::Chronometer timer;
    timer.tic();
    GA_Type ga_obj;
    ga_obj.problem_mode=EA::GA_MODE::SOGA;//单目标，别的模式没学
    ga_obj.multi_threading=false;//大二作业单线程够了
    ga_obj.verbose=false;
    ga_obj.population=20;//种群，改大跑更慢
    ga_obj.generation_max=200;
    ga_obj.calculate_SO_total_fitness=calculate_SO_total_fitness;
    ga_obj.init_genes=init_genes;
    ga_obj.eval_solution=eval_solution;
    ga_obj.mutate=mutate;
    ga_obj.crossover=crossover;
    ga_obj.SO_report_generation=SO_report_generation;
    ga_obj.best_stall_max=10;
    ga_obj.elite_count=2;
    ga_obj.crossover_fraction=0.7;
    ga_obj.mutation_rate=0.2;
    ga_obj.solve();
    std::cout<<"跑完了，用时"<<timer.toc()<<"秒"<<std::endl;
    output_file.close();
    return 0;
}
