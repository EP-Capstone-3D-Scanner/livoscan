import os
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch_ros.actions import ComposableNodeContainer
from launch_ros.descriptions import ComposableNode
from launch_ros.actions import Node

def generate_launch_description():

    camera_config_file_path = os.path.join(
        get_package_share_directory('livoscan'),
        'usb_cam',
        'params.yaml'
    )

    camera_tf_node = Node(
        package='tf2_ros',
        executable='static_transform_publisher',
        name='camera_to_lidar_tf',
        arguments=[
            '0.05', '0.0', '0.10',  # Translation (x, y, z) in meters
            '0.0', '0.0', '0.0',    # Rotation (yaw, pitch, roll) in radians
            'livoscan_lidar_frame',          # Parent frame ID (Your Lidar)
            'livoscan_camera_frame'  # Child frame ID (Your Camera)
        ]
    )

    usb_cam_node = ComposableNode(
        package='usb_cam',
        plugin='usb_cam::UsbCamNode',
        name='usb_cam_node',
        parameters=[camera_config_file_path],
        extra_arguments=[{'use_intra_process_comms': True}],
        remappings=[
            ('image_raw', '/zed/image_raw'),
            ('camera_info', '/zed/camera_info')
        ]
    )

    # 2. Crop Node (Crops 2560x720 down to 1280x720)
    crop_node = ComposableNode(
        package='image_proc',
        plugin='image_proc::CropDecimateNode',
        name='crop_decimate_node',
        parameters=[{
            'x_offset': 0,      # 0 for left eye, 1280 for right eye
            'y_offset': 0,
            'width': 1280,
            'height': 720,
        }],
        extra_arguments=[{'use_intra_process_comms': True}],
        remappings=[
            ('in/image_raw', '/zed/image_raw'),
            ('in/camera_info', '/zed/camera_info'),
            ('out/image_raw', '/camera/left/image_raw'),
            ('out/camera_info', '/camera/left/camera_info')
        ]
    )

    # 3. Rectify Node (Applies undistortion)
    rectify_node = ComposableNode(
        package='image_proc',
        plugin='image_proc::RectifyNode',
        name='rectify_node',
        remappings=[
            ('image', '/camera/left/image_raw'),
            ('camera_info', '/camera/left/camera_info'),
            ('image_rect', '/camera/left/image_rect')
        ],
        extra_arguments=[{'use_intra_process_comms': True}]
    )

    time_offset_node = ComposableNode(
        package='livoscan',
        plugin='CameraTimeOffsetNode',
        name='time_offset_node',
        remappings=[
            ('image_in', '/camera/left/image_rect'),
            ('camera_info_in', '/camera/left/camera_info'),
            ('image_out', '/camera/out/image'),
            ('camera_info_out', '/camera/out/camera_info')
        ],
        extra_arguments=[{'use_intra_process_comms': True}]
    )

    # Combined Container running in a single OS process
    container = ComposableNodeContainer(
        name='camera_pipeline_container',
        namespace='',
        package='rclcpp_components',
        executable='component_container',
        composable_node_descriptions=[
            usb_cam_node,
            crop_node,
            rectify_node,
            time_offset_node
        ],
        output='screen'
    )

    return LaunchDescription([container, camera_tf_node])
