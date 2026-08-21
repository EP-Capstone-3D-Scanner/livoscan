#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/image.hpp>
#include <sensor_msgs/msg/camera_info.hpp>

class CameraTimeOffsetNode : public rclcpp::Node
{
  public:
    CameraTimeOffsetNode(const rclcpp::NodeOptions & options) : Node("camera_time_offset_node", options)
    {
      this->declare_parameter<double>("offset_sec", 5.0);

      img_pub_ = this->create_publisher<sensor_msgs::msg::Image>("image_out", 10);
      img_sub_ = this->create_subscription<sensor_msgs::msg::Image>(
        "image_in", 10,
        [this](std::unique_ptr<sensor_msgs::msg::Image> msg) {
            double offset = this->get_parameter("offset_sec").as_double();
        
            msg->header.stamp = rclcpp::Time(msg->header.stamp) + rclcpp::Duration::from_seconds(offset);
        
            img_pub_->publish(std::move(msg));
        }
      );

      info_pub_ = this->create_publisher<sensor_msgs::msg::CameraInfo>("camera_info_out", 10);
      info_sub_ = this->create_subscription<sensor_msgs::msg::CameraInfo>(
        "camera_info_in", 10,
        [this](std::unique_ptr<sensor_msgs::msg::CameraInfo> msg) {
            double offset = this->get_parameter("offset_sec").as_double();
            msg->header.stamp = rclcpp::Time(msg->header.stamp) + rclcpp::Duration::from_seconds(offset);
            info_pub_->publish(std::move(msg));
        }
      );
    };
  private:
    rclcpp::Publisher<sensor_msgs::msg::Image>::SharedPtr img_pub_;
    rclcpp::Subscription<sensor_msgs::msg::Image>::SharedPtr img_sub_;
    rclcpp::Publisher<sensor_msgs::msg::CameraInfo>::SharedPtr info_pub_;
    rclcpp::Subscription<sensor_msgs::msg::CameraInfo>::SharedPtr info_sub_;
};

#include <rclcpp_components/register_node_macro.hpp>
RCLCPP_COMPONENTS_REGISTER_NODE(CameraTimeOffsetNode)
