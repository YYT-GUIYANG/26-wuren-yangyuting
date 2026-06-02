# ROS2 进阶作业完成报告

## 进阶作业：把 SLAM 地图里的锥桶画到 RViz 里

基础作业是让乌龟画“8”字，进阶作业就突然上强度了——要用 bag 文件播放真实的 SLAM 地图数据，然后自己写一个节点，把地图里的锥桶用不同颜色的立体图标显示在 RViz 里。一开始我连 `fsd_common_msgs` 是什么都不知道，只能照着作业说明一步步磕。

---

### 1. 先搞清楚数据长什么样

作业给了一个 bag 文件（`map_to_visualize`），里面只有一个话题 `/estimation/slam/map`，消息类型是自定义的 `fsd_common_msgs/msg/Map`。我用 `ros2 interface show fsd_common_msgs/msg/Map` 看了一下，发现 Map 消息里有四个数组：`cone_yellow`、`cone_blue`、`cone_red`、`cone_unknown`。每个数组里装的是 `Cone` 消息，每个锥桶有 `position`（x, y, z 坐标）和 `color` 等字段。

所以我的任务很清晰：订阅这个话题，收到 Map 消息后，遍历四个数组，为每个锥桶生成一个 Marker（可视化小球），然后发布到另一个话题，让 RViz 显示出来。

---

### 2. 创建工作空间和功能包

我把工作空间直接建在了作业文件夹 `ros2_homework_advanced` 下面，方便最后整个目录打包提交，不用到处找 bag 和消息包。

目录结构大致是这样：

```
ros2_homework_advanced/        ← 工作空间根目录
├── map_to_visualize/          ← bag 文件
├── fsd_common_msgs/           ← 老师给的消息包（原位置保留）
├── src/
│   ├── fsd_common_msgs/       ← 复制一份到这里参与编译
│   └── cone_visualizer_package/
```

**踩坑记录：** 一开始我在 `src/` 目录里面执行 `mv fsd_common_msgs src/`，当然找不到文件——因为 `fsd_common_msgs` 在上一级。正确做法是在**工作空间根目录**执行：

```bash
cp -r fsd_common_msgs src/
```

或者：

```bash
ln -s ../fsd_common_msgs src/fsd_common_msgs
```

另外，根目录和 `src/` 里如果同时存在 `fsd_common_msgs`，`colcon build` 会报“重复包名”。我的解决办法是在根目录那份加了 `COLCON_IGNORE`，只让 `src/` 里的参与编译。

创建自己的功能包：

```bash
cd src
ros2 pkg create cone_visualizer_package --build-type ament_cmake \
  --dependencies rclcpp fsd_common_msgs visualization_msgs geometry_msgs
```

`--dependencies` 里写上了 `fsd_common_msgs`，这样编译时才能找到那些自定义消息。

---

### 3. 写节点的核心逻辑

我的节点代码写在 `src/cone_visualizer_package/src/abc.cpp` 里，主要做了三件事：

- **订阅** `/estimation/slam/map`，在回调函数里拿到 Map 消息。
- **解析**：分别处理 `cone_yellow`、`cone_blue`、`cone_red`、`cone_unknown` 四个数组。对每个锥桶，读取它的 `position.x`、`position.y`、`position.z`，并给它分配颜色——黄色、蓝色、红色、灰色（未知）。
- **发布**：把所有锥桶的 Marker 打包成一个 `MarkerArray`（一次发布多个 Marker，比一个一个发效率高），发布到 `/cone_markers` 话题。

**关键点：** Marker 的 `header.frame_id` 必须和 Map 消息里的 `header.frame_id` 保持一致，否则 RViz 不知道把球画在哪个坐标系下。我是从收到的 Map 消息里直接取 `frame_id`，没有写死。

我还给不同颜色的锥桶设置了不同的 `ns`（命名空间），比如 `yellow_cones`、`blue_cones`，这样 RViz 里区分起来更清楚。

---

### 4. 编译时的坑

**坑一：找不到 `can_msgs`**

第一次 `colcon build` 时，`fsd_common_msgs` 报错，说找不到 `can_msgs`。查了一下，`can_msgs` 是 ROS2 官方的一个包，安装一下就好了：

```bash
sudo apt install ros-humble-can-msgs
```

装完重新编译，就过了。

**坑二：路径里有空格**

我的工作空间在 `ROS2 learning` 文件夹下，路径中间有空格，直接在目录里 `colcon build` 会编译失败。最后把 build 和 install 放到了 home 目录下没有空格的路径：

```bash
cd "/home/yangyuting/桌面/ROS2 learning/ros2_learning/ros2_homework_advanced"
source /opt/ros/humble/setup.bash
colcon build \
  --build-base ~/ros2_hw_adv_build \
  --install-base ~/ros2_hw_adv_install
source ~/ros2_hw_adv_install/setup.bash
```

源码还是在作业目录里，只是编译产物换了个地方，不影响提交。

**坑三：CMakeLists.txt 忘了加可执行文件**

`ros2 pkg create` 生成的 CMakeLists.txt 只有空的 `ament_package()`，需要自己加上：

```cmake
add_executable(abc src/abc.cpp)
ament_target_dependencies(abc rclcpp fsd_common_msgs visualization_msgs geometry_msgs)
install(TARGETS abc DESTINATION lib/${PROJECT_NAME})
```

不然编译通过了也找不到节点可以 run。

---

### 5. 运行测试

#### 方式一：三个终端手动启动

- **终端 1**：播放 bag 文件  
  ```bash
  ros2 bag play map_to_visualize --loop
  ```
- **终端 2**：运行我的可视化节点  
  ```bash
  ros2 run cone_visualizer_package abc
  ```
- **终端 3**：打开 RViz2  
  ```bash
  rviz2
  ```
  在 RViz 里把 Fixed Frame 设为 `world`（下面会解释为什么不是 `map`），Add → By topic → 选择 `/cone_markers` 的 MarkerArray 显示。

#### 方式二：一键 launch（后来加的）

写好了 `launch/cone_visualizer.launch.py` 之后，可以一条命令全启动：

```bash
cd "/home/yangyuting/桌面/ROS2 learning/ros2_learning/ros2_homework_advanced"
source /opt/ros/humble/setup.bash
source ~/ros2_hw_adv_install/setup.bash
ros2 launch cone_visualizer_package cone_visualizer.launch.py
```

会自动播放 bag、启动节点、打开 RViz。

运行后，RViz 里慢慢出现了彩色的小球——黄色、蓝色、红色、灰色，对应四种锥桶，位置也和 bag 里的坐标对得上。

---

### 6. 遇到的主要问题

- **RViz 里看不到任何东西**  
排查发现有两个原因：一是 Marker 的 `header.frame_id` 没有和 Map 消息保持一致；二是 RViz 的 Fixed Frame 设错了。我一开始想当然设成了 `map`，但用 `ros2 topic echo /estimation/slam/map --field header.frame_id` 一看，bag 里实际的 frame_id 是 `**world`**。把 RViz Fixed Frame 改成 `world` 之后，锥桶就显示出来了。
- **Marker 一闪一闪**  
需要把 `lifetime` 设为 0（表示永久显示），否则 RViz 会很快把老的 Marker 删掉。另外 `color.a` 必须大于 0，不然也是透明的，看不见。
- **bag 播放时节点收不到消息**  
因为节点启动比 bag 晚，可能错过前面几条消息。解决办法是先启动节点再播放 bag，或者 bag 加 `--loop` 循环播放。我在 launch 文件里给节点加了 1 秒延迟，等 bag 先跑起来。
- **改了 yaml 或代码后没效果**  
进阶作业改 C++ 代码需要重新 `colcon build` 并 `source install/setup.bash`；如果用了 launch，也要重新编译才能更新 launch 和 rviz 配置文件。

---

### 7. 思考题

**Q1：如何获取包中地图的坐标系？**

用命令直接看：

```bash
ros2 topic echo /estimation/slam/map --field header.frame_id
```

我跑出来的结果是 `world`。在代码里就是读 `msg->header.frame_id`，然后原样赋给每个 Marker 的 `header.frame_id`。

**Q2：如果它和 RViz 默认的 map 坐标系不符，怎么办？**

最简单的方法：在 RViz 里把 **Fixed Frame** 改成消息里实际的 frame_id（我这里是 `world`）。Marker 的 frame_id 也要和 Map 保持一致。

如果非要用 `"map"` 作为 Fixed Frame，就需要发布一个静态 TF 变换，把 `world` 和 `map` 连起来。但作业里不需要那么复杂，两边保持一致就行了。

**Q3：锥桶怎么实现？**

作业没有严格要求形状，我用的是 `SPHERE`（球体），每个锥桶一个 Marker，四种颜色分别对应四个数组。用 `MarkerArray` 一次发布所有锥桶，比单个 Marker 效率高。不同颜色用了不同的 `ns` 命名空间，避免 id 冲突。

---

### 8. 最终效果

bag 播放约 29 秒，共 292 条 Map 消息。我的节点每收到一条 Map 消息就发布一批 Marker，RViz 里锥桶随着时间逐渐出现，最终呈现出整个赛场上的锥桶分布——左边一列蓝色、中间赛道两侧黄色、偶尔有红色和灰色（未知颜色）的锥桶。

截图和录屏放在 `result/` 文件夹里了。

---

### 9. AI 使用情况

使用 Cursor AI 辅助了：工作空间搭建、编译报错排查（`can_msgs`、路径空格、重复包名）、launch 和 rviz 配置文件编写、以及 `frame_id` 逻辑修正。

核心的可视化逻辑（订阅 Map、遍历四个锥桶数组、发布 MarkerArray）是我自己写的，在 `src/abc.cpp` 里。

---

### 10. 启动命令汇总（方便复现）

```bash
# 编译
cd "/home/yangyuting/桌面/ROS2 learning/ros2_learning/ros2_homework_advanced"
source /opt/ros/humble/setup.bash
colcon build --build-base ~/ros2_hw_adv_build --install-base ~/ros2_hw_adv_install
source ~/ros2_hw_adv_install/setup.bash

# 一键运行
ros2 launch cone_visualizer_package cone_visualizer.launch.py

# 验证
ros2 topic echo /estimation/slam/map --field header.frame_id
ros2 topic echo /cone_markers --once
```

---

整个过程让我对 ROS2 的自定义消息、Marker 可视化、bag 播放和坐标系有了更深的理解。从“连 fsd_common_msgs 是什么都不知道”，到 RViz 里看到满地图的彩色锥桶，还是挺有成就感的。