#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/point_cloud2.hpp>
#include <sensor_msgs/point_cloud2_iterator.hpp>
#include <livox_interfaces_ext/msg/custom_msg.hpp>
#include <cstdint>

class LivoxConverter : public rclcpp::Node {
public:
    LivoxConverter() : Node("livox_converter") {
        // Subscribe to the Livox custom message topic
        sub_ = this->create_subscription<livox_interfaces_ext::msg::CustomMsg>(
            "/livox/lidar", 10,
            std::bind(&LivoxConverter::callback, this, std::placeholders::_1));
        
        // Publish the standard PointCloud2 message
        pub_ = this->create_publisher<sensor_msgs::msg::PointCloud2>(
            "/livox/points", 10);
            
        RCLCPP_INFO(this->get_logger(), "Livox CustomMsg to PointCloud2 converter started.");
    }

private:
    void callback(const livox_interfaces_ext::msg::CustomMsg::SharedPtr msg) {
        sensor_msgs::msg::PointCloud2 cloud;
        cloud.header = msg->header;
        cloud.height = 1;
        cloud.width = msg->point_num;
        cloud.is_dense = true;
        cloud.is_bigendian = false;
        
        // 1. Manually configure the binary fields for the point cloud
        cloud.fields.clear();
        int offset = 0;
        auto add_field = [&](const std::string& name, uint8_t datatype, int size) {
            sensor_msgs::msg::PointField f;
            f.name = name; 
            f.offset = offset; 
            f.datatype = datatype; 
            f.count = 1;
            cloud.fields.push_back(f);
            offset += size;
        };
        
        add_field("x", sensor_msgs::msg::PointField::FLOAT32, 4);
        add_field("y", sensor_msgs::msg::PointField::FLOAT32, 4);
        add_field("z", sensor_msgs::msg::PointField::FLOAT32, 4);
        add_field("intensity", sensor_msgs::msg::PointField::FLOAT32, 4);
        add_field("t", sensor_msgs::msg::PointField::UINT32, 4); // Offset time
        add_field("tag", sensor_msgs::msg::PointField::UINT8, 1);
        add_field("line", sensor_msgs::msg::PointField::UINT8, 1);
        
        cloud.point_step = offset;
        cloud.row_step = cloud.width * cloud.point_step;
        cloud.data.resize(cloud.row_step);
        
        // 2. Map safe iterators to the newly allocated byte array
        sensor_msgs::PointCloud2Iterator<float> iter_x(cloud, "x");
        sensor_msgs::PointCloud2Iterator<float> iter_y(cloud, "y");
        sensor_msgs::PointCloud2Iterator<float> iter_z(cloud, "z");
        sensor_msgs::PointCloud2Iterator<float> iter_i(cloud, "intensity");
        sensor_msgs::PointCloud2Iterator<uint32_t> iter_t(cloud, "t");
        sensor_msgs::PointCloud2Iterator<uint8_t> iter_tag(cloud, "tag");
        sensor_msgs::PointCloud2Iterator<uint8_t> iter_line(cloud, "line");

        // 3. Iterate through CustomMsg points and populate PointCloud2
        for (const auto& pt : msg->points) {
            *iter_x = pt.x;
            *iter_y = pt.y;
            *iter_z = pt.z;
            *iter_i = static_cast<float>(pt.reflectivity); 
            *iter_t = pt.offset_time;
            *iter_tag = pt.tag;
            *iter_line = pt.line;

            ++iter_x; ++iter_y; ++iter_z; ++iter_i; ++iter_t; ++iter_tag; ++iter_line;
        }

        pub_->publish(cloud);
    }

    rclcpp::Subscription<livox_interfaces_ext::msg::CustomMsg>::SharedPtr sub_;
    rclcpp::Publisher<sensor_msgs::msg::PointCloud2>::SharedPtr pub_;
};

int main(int argc, char** argv) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<LivoxConverter>());
    rclcpp::shutdown();
    return 0;
}
