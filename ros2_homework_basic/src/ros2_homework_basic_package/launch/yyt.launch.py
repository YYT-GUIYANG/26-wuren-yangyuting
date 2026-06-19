from pathlib import Path
from launch import LaunchDescription
from launch.actions import RegisterEventHandler, TimerAction
from launch.event_handlers import OnProcessStart
from launch_ros.actions import Node
#这里是我写错yaml文件的位置后，ai给的建议，避免下次再将yaml文件写错，所以多加了这一串代码
def _find_config_file():
    # 向上查找同时包含 src/ 和 config/yyt.yaml 的工作空间根目录
    for parent in Path(__file__).resolve().parents:
        config_file = parent / 'config' / 'yyt.yaml'
        if config_file.exists() and (parent / 'src').is_dir():
            return str(config_file)
    raise RuntimeError('未找到 config/yyt.yaml，请确认文件位于工作空间根目录 config/ 下')
# 这两个基本上是固定的格式，一个是 launch 的描述容器，一个是启动节点
def generate_launch_description():
    yyt_config = _find_config_file()

    turtlesim_node = Node(
        package='turtlesim',  # 包名
        executable='turtlesim_node',  # 节点名
        name='turtlesim',  # 这里是可以省略的，因为它走默认节点的形式
        output='screen',  # 节点信息打印在终端屏幕
    )
    draw_eight_node = Node(
        package='ros2_homework_basic_package',
        executable='yyt',
        name='draw_zuoye_node',
        output='screen',
        parameters=[yyt_config],
    )
    # 等 turtlesim 窗口就绪后再启动 yyt，避免过早发布 cmd_vel
    delayed_draw_eight_node = TimerAction(
        period=1.0,
        actions=[draw_eight_node],
    )
    start_draw_after_turtlesim = RegisterEventHandler(
        OnProcessStart(
            target_action=turtlesim_node,
            on_start=[delayed_draw_eight_node],
        )
    )
    return LaunchDescription([
        turtlesim_node,
        start_draw_after_turtlesim,
    ])
