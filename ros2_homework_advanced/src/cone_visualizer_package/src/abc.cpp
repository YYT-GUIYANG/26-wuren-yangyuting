#include <functional>
#include <memory>
#include <string>
#include <vector>
#include "fsd_common_msgs/msg/cone.hpp"
#include "fsd_common_msgs/msg/map.hpp"
#include "geometry_msgs/msg/point.hpp"
#include "rclcpp/rclcpp.hpp"
#include "visualization_msgs/msg/marker.hpp"
#include "visualization_msgs/msg/marker_array.hpp"
using namespace std;
using placeholders::_1;
class ConeVisualizer:public rclcpp::Node
{
public:
  ConeVisualizer():Node("cone_visualizer")
  {
    subscription_=this->create_subscription<fsd_common_msgs::msg::Map>(
      "/estimation/slam/map", 10,
      std::bind(&ConeVisualizer::topic_callback,this, _1));
    marker_pub_=this->create_publisher<visualization_msgs::msg::MarkerArray>("/cone_markers", 10);
    RCLCPP_INFO(this->get_logger(),"Cone Visualizer Node has been started.");
  }
private:
  void topic_callback(const fsd_common_msgs::msg::Map::SharedPtr msg) const
  {
    visualization_msgs::msg::MarkerArray marker_array;
    const string & frame_id=msg->header.frame_id.empty() ? "map" : msg->header.frame_id;
    add_cones_to_marker_array(msg->cone_yellow,frame_id,msg->header.stamp,"yellow_cones", 1.0, 1.0, 0.0, marker_array);
    add_cones_to_marker_array(msg->cone_blue,frame_id,msg->header.stamp,"blue_cones", 0.0, 0.0, 1.0, marker_array);
    add_cones_to_marker_array(msg->cone_red,frame_id,msg->header.stamp,"red_cones", 1.0, 0.0, 0.0, marker_array);
    add_cones_to_marker_array(msg->cone_unknown,frame_id,msg->header.stamp,"unknown_cones", 0.5, 0.5, 0.5, marker_array);
    marker_pub_->publish(marker_array);
  }
  void add_cones_to_marker_array(
    const vector<fsd_common_msgs::msg::Cone> & cones,
    const string&frame_id,
    const builtin_interfaces::msg::Time & stamp,
    const string&ns,
    float r,float g,float b,
    visualization_msgs::msg::MarkerArray & marker_array) const
  {
    if(cones.empty()) 
    {
      return;
    }
    for(size_t i=0;i<cones.size();++i) 
    {
      visualization_msgs::msg::Marker marker;
      marker.header.frame_id=frame_id;
      marker.header.stamp=stamp;
      marker.ns=ns;
      marker.id=static_cast<int>(i);
      marker.type=visualization_msgs::msg::Marker::SPHERE;
      marker.action=visualization_msgs::msg::Marker::ADD;
      marker.pose.position.x=cones[i].position.x;
      marker.pose.position.y=cones[i].position.y;
      marker.pose.position.z=cones[i].position.z;
      marker.pose.orientation.w=1.0;
      marker.scale.x=0.3;
      marker.scale.y=0.3;
      marker.scale.z=0.3;
      marker.color.r=r;
      marker.color.g=g;
      marker.color.b=b;
      marker.color.a=1.0;
      marker.lifetime=rclcpp::Duration::from_seconds(0);
      marker_array.markers.push_back(marker);
    }
  }
  rclcpp::Subscription<fsd_common_msgs::msg::Map>::SharedPtr subscription_;
  rclcpp::Publisher<visualization_msgs::msg::MarkerArray>::SharedPtr marker_pub_;
};
int main(int argc,char*argv[])
{
  rclcpp::init(argc,argv);
  rclcpp::spin(std::make_shared<ConeVisualizer>());
  rclcpp::shutdown();
  return 0;
}