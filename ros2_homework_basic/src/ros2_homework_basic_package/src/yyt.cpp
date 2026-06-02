#include<chrono>
#include<cmath>//库函数
#include<functional>
#include<memory>
#include"geometry_msgs/msg/twist.hpp"//这里是运动控制消息，主要是用于发布速度的，有线速度，有角速度
#include"rclcpp/rclcpp.hpp"//这里是核心库
using namespace std;
using namespace chrono_literals;
//先创建一个类，然后发布节点，然后在建立一个发布者
class DrawzuoyeNode:public rclcpp::Node
{
  public:
  DrawzuoyeNode():Node("draw_zuoye_node")
  {
    //声明参数然后给出一个默认值
    this->declare_parameter<double>("linear_x",2.0);
    this->declare_parameter<double>("angular_z_abs",1.0);
    //读取yaml文件的参数
    linear_x_=this->get_parameter("linear_x").as_double();
    angular_z_abs_=this->get_parameter("angular_z_abs").as_double();
    publisher_=this->create_publisher<geometry_msgs::msg::Twist>("/turtle1/cmd_vel",10);
    duration_per_phase_=2.0*M_PI/angular_z_abs_;
    phase_start_time_=this->now();
    current_phase_=0;
    timer_=this->create_wall_timer(50ms,std::bind(&DrawzuoyeNode::publish_velocity,this));
  }
  /*这里的日志消息可写可不写，我这里是在学习代码，所以写了一个作为示范
  RCLCPP_INFO(this->get_logger(),"节点已启动")
  */
  private:
  //先定义我们要用到的变量，还有一些固定的定时器的格式
  rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr publisher_;
  rclcpp::TimerBase::SharedPtr timer_;
  double linear_x_;
  double angular_z_abs_;
  double duration_per_phase_;
  rclcpp::Time phase_start_time_;
  int current_phase_;
  void publish_velocity()
  {
    auto now=this->now();
    double elapsed=(now-phase_start_time_).seconds();
    if(current_phase_==0&&elapsed>=duration_per_phase_)
    {
      current_phase_=1;// 第一阶段结束：应已完成第一个圆，开始反方向画第二个圆
      phase_start_time_=now;// 重置计时，给第二个圆同样长的 duration
    }
    else if(current_phase_==1&&elapsed>=duration_per_phase_)
    {
      current_phase_=2;
      timer_->cancel();// 画完后停止定时器，避免节点一直空跑；也减少重复 launch 时多个节点互相干扰
    }
    geometry_msgs::msg::Twist twist_msg;
    if(current_phase_==0)
    {
      twist_msg.linear.x=linear_x_;
      twist_msg.angular.z=angular_z_abs_;
    }
    else if(current_phase_==1)
    {
      twist_msg.linear.x=linear_x_;
      twist_msg.angular.z=-angular_z_abs_;
    }
    else
    {
      twist_msg.linear.x=0.0;
      twist_msg.angular.z=0.0;
    }
    publisher_->publish(twist_msg);
  }
};
int main(int argc,char* argv[])
{
  rclcpp::init(argc,argv);
  auto node=make_shared<DrawzuoyeNode>();
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}
