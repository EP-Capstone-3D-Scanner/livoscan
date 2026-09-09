from launch import LaunchDescription
from launch_ros.actions import ComposableNodeContainer
from launch_ros.descriptions import ComposableNode

def generate_launch_description():
    # Define the container that will hold both pcl_ros nodes
    container = ComposableNodeContainer(
        name='pointcloud_processing_container',
        namespace='',
        package='rclcpp_components',
        executable='component_container',
        composable_node_descriptions=[
            
            # 1. Voxel Grid Node (Downsampling/Averaging)
            ComposableNode(
                package='pcl_ros',
                plugin='pcl_ros::VoxelGrid',
                name='voxel_grid_filter',
                parameters=[{
                    'leaf_size': 0.01  # 10cm voxels. Increase to thin out more points.
                }],
                remappings=[
                    # TODO: Replace with your actual fast-livo2 pointcloud topic
                    ('input', '/cloud_registered'), 
                    ('output', '/voxel_grid/output')
                ],
                extra_arguments=[{'use_intra_process_comms': True}]
            ),
            
            # 2. Statistical Outlier Removal Node (Noise Filtering)
            ComposableNode(
                package='pcl_ros',
                plugin='pcl_ros::StatisticalOutlierRemoval',
                name='outlier_removal_filter',
                parameters=[{
                    'mean_k': 50,     # Number of neighboring points to analyze for each point
                    'stddev': 1.0     # Standard deviation multiplier. Lower = more aggressive filtering.
                }],
                remappings=[
                    # This must match the output of the VoxelGrid node
                    ('input', '/voxel_grid/output'),
                    # Final clean map topic
                    ('output', '/clean_map')
                ],
                extra_arguments=[{'use_intra_process_comms': True}]
            ),
        ],
        output='screen',
    )

    return LaunchDescription([container])
