#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/image.hpp>
#include <sensor_msgs/msg/camera_info.hpp>
#include <cv_bridge/cv_bridge.h>
#include <opencv2/opencv.hpp>

class UvcCameraDriverNode : public rclcpp::Node
{
public:
    UvcCameraDriverNode() : Node("uvc_camera_driver_node")
    {
        // Parameter for the camera device
        this->declare_parameter<std::string>("video_device", "/dev/video0");
        std::string video_device = this->get_parameter("video_device").as_string();

        // 1. Open the raw UVC stream
        cap_.open(video_device, cv::CAP_V4L2);
        if (!cap_.isOpened()) {
            RCLCPP_ERROR(this->get_logger(), "Failed to open video device %s", video_device.c_str());
            rclcpp::shutdown();
            return;
        }

        // 2. Set the combined ZED resolution (e.g., 2560x720 for 720p @ 30fps)
        cap_.set(cv::CAP_PROP_FRAME_WIDTH, 2560);
        cap_.set(cv::CAP_PROP_FRAME_HEIGHT, 720);
        cap_.set(cv::CAP_PROP_FPS, 30);

        // 3. Initialize Publishers
        image_pub_ = this->create_publisher<sensor_msgs::msg::Image>("zed/left/image_rect", 10);
        info_pub_  = this->create_publisher<sensor_msgs::msg::CameraInfo>("zed/left/camera_info", 10);

        // 4. Setup Calibration Matrices (Replace with your downloaded factory values)
        // Values for fx, fy, cx, cy
        camera_matrix_ = (cv::Mat_<double>(3, 3) << 528.0, 0.0, 640.0, 
                                                    0.0, 528.0, 360.0, 
                                                    0.0, 0.0, 1.0);
        // Distortion coefficients [k1, k2, p1, p2, k3]
        dist_coeffs_ = (cv::Mat_<double>(1, 5) << -0.04, 0.01, 0.0, 0.0, 0.0); 

        // Compute optimal new camera matrix for undistortion
        new_camera_matrix_ = cv::getOptimalNewCameraMatrix(
            camera_matrix_, dist_coeffs_, cv::Size(1280, 720), 0.0);

        // 5. Start the capture loop
        timer_ = this->create_wall_timer(
            std::chrono::milliseconds(33), // ~30 fps
            std::bind(&UvcCameraDriverNode::timer_callback, this));
            
        RCLCPP_INFO(this->get_logger(), "ZED UVC Node started, publishing left image.");
    }

private:
    void timer_callback()
    {
        cv::Mat raw_frame;
        if (!cap_.read(raw_frame) || raw_frame.empty()) {
            RCLCPP_WARN(this->get_logger(), "Dropped frame!");
            return;
        }

        auto timestamp = this->get_clock()->now();

        // 6. Split the image in half (Left Image)
        cv::Rect left_roi(0, 0, raw_frame.cols / 2, raw_frame.rows);
        cv::Mat left_image = raw_frame(left_roi);

        // 7. Undistort the left image
        cv::Mat rectified_image;
        cv::undistort(left_image, rectified_image, camera_matrix_, dist_coeffs_, new_camera_matrix_);

        // 8. Convert to ROS Image Message
        std_msgs::msg::Header header;
        header.stamp = timestamp;
        header.frame_id = "zed_left_camera_optical_frame";

        sensor_msgs::msg::Image::SharedPtr img_msg = 
            cv_bridge::CvImage(header, "bgr8", rectified_image).toImageMsg();

        // 9. Build and publish CameraInfo
        sensor_msgs::msg::CameraInfo info_msg;
        info_msg.header = header;
        info_msg.height = 720;
        info_msg.width = 1280;
        info_msg.distortion_model = "plumb_bob";
        
        // Flatten matrices into arrays for the ROS message
        info_msg.d.assign((double*)dist_coeffs_.datastart, (double*)dist_coeffs_.dataend);
        for (int i = 0; i < 9; i++) {
            info_msg.k[i] = camera_matrix_.at<double>(i / 3, i % 3);
        }
        // Simplified Projection matrix assuming no rotation/translation to itself
        info_msg.p = {new_camera_matrix_.at<double>(0,0), 0.0, new_camera_matrix_.at<double>(0,2), 0.0,
                      0.0, new_camera_matrix_.at<double>(1,1), new_camera_matrix_.at<double>(1,2), 0.0,
                      0.0, 0.0, 1.0, 0.0};

        image_pub_->publish(*img_msg);
        info_pub_->publish(info_msg);
    }

    cv::VideoCapture cap_;
    cv::Mat camera_matrix_, dist_coeffs_, new_camera_matrix_;
    rclcpp::Publisher<sensor_msgs::msg::Image>::SharedPtr image_pub_;
    rclcpp::Publisher<sensor_msgs::msg::CameraInfo>::SharedPtr info_pub_;
    rclcpp::TimerBase::SharedPtr timer_;
};

int main(int argc, char * argv[])
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<UvcCameraDriverNode>());
    rclcpp::shutdown();
    return 0;
}
