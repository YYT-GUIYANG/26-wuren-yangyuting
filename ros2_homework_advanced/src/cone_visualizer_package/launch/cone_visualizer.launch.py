from pathlib import Path
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, ExecuteProcess, TimerAction
from launch.substitutions import LaunchConfiguration, PathJoinSubstitution
from launch_ros.actions import Node
from launch_ros.substitutions import FindPackageShare
def _find_bag_path():
    search_roots = list(Path(__file__).resolve().parents) + [Path.cwd(), *Path.cwd().parents]
    for root in search_roots:
        candidates = [
            root / 'map_to_visualize',
            root / 'ros2_homework_advanced' / 'map_to_visualize',
        ]
        for bag_dir in candidates:
            if bag_dir.is_dir() and (bag_dir / 'metadata.yaml').exists():
                return str(bag_dir)
    raise RuntimeError(
        '未找到 map_to_visualize，请在作业目录下 launch，或使用 bag_path:=/完整路径/map_to_visualize'
    )
def generate_launch_description():
    bag_path = _find_bag_path()
    rviz_config = PathJoinSubstitution([
        FindPackageShare('cone_visualizer_package'),
        'config',
        'cone_visualizer.rviz',
    ])
    return LaunchDescription([
        DeclareLaunchArgument(
            'bag_path',
            default_value=bag_path,
            description='rosbag2 目录路径',
        ),
        ExecuteProcess(
            cmd=['ros2', 'bag', 'play', LaunchConfiguration('bag_path'), '--loop'],
            output='screen',
        ),
        TimerAction(
            period=1.0,
            actions=[
                Node(
                    package='cone_visualizer_package',
                    executable='abc',
                    name='cone_visualizer',
                    output='screen',
                ),
            ],
        ),
        TimerAction(
            period=2.0,
            actions=[
                Node(
                    package='rviz2',
                    executable='rviz2',
                    name='rviz2',
                    output='screen',
                    arguments=['-d', rviz_config],
                ),
            ],
        ),
    ])
