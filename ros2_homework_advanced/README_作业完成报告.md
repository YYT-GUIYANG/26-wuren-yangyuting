
# ROS2 进阶作业完成报告
### 1. 首先还是明确给我们的文件夹是什么，然后需要我做什么

作业给了一个 bag 文件（`map_to_visualize`），里面只有一个话题 `/estimation/slam/map`，消息类型是自定义的 `fsd_common_msgs/msg/Map`。我用 `ros2 interface show fsd_common_msgs/msg/Map` 看了一下，发现 Map 消息里有四个数组：`cone_yellow`、`cone_blue`、`cone_red`、`cone_unknown`。每个数组里装的是 `Cone` 消息，每个锥桶有 `position`（x, y, z 坐标）和 `color` 等字段。

所以我的任务很清晰：订阅这个话题，收到 Map 消息后，遍历四个数组，为每个锥桶生成一个 Marker（可视化小球），然后发布到另一个话题，让 RViz 显示出来。

---

### 2. 创建工作空间和功能包

我把工作空间直接建在了作业文件夹 `ros2_homework_advanced` 下面，方便最后整个目录打包提交，不用到处找 bag 和消息包。目录结构大致是这样：

```
ros2_homework_advanced/        ← 工作空间根目录
├── map_to_visualize/          ← bag 文件
├── fsd_common_msgs/           ← 老师给的消息包（原位置保留）
├── src/
│   ├── fsd_common_msgs/       ← 复制一份到这里参与编译
│   └── cone_visualizer_package/
```

---

### 3. 写节点的思路，具体体现在代码里面

我的节点代码写在 `src/cone_visualizer_package/src/abc.cpp` 里，主要做了三件事：

- **订阅** `/estimation/slam/map`，在回调函数里拿到 Map 消息。
- **解析**：分别处理 `cone_yellow`、`cone_blue`、`cone_red`、`cone_unknown` 四个数组。对每个锥桶，读取它的 `position.x`、`position.y`、`position.z`，并给它分配颜色——黄色、蓝色、红色、灰色（未知）。
- **发布**：把所有锥桶的 Marker 打包成一个 `MarkerArray`（一次发布多个 Marker，比一个一个发效率高），发布到 `/cone_markers` 话题。

---

### 4. 运行测试

#### 一键 launch（参考上一个作业后来加的）

```bash
cd "/home/yangyuting/桌面/ROS2 learning/ros2_learning/ros2_homework_advanced"
source /opt/ros/humble/setup.bash
source ~/ros2_hw_adv_install/setup.bash
ros2 launch cone_visualizer_package cone_visualizer.launch.py
```

会自动播放 bag、启动节点、打开 RViz。  
运行后，RViz 里慢慢出现了彩色的小球——黄色、蓝色、红色、灰色（其实只有蓝色和红色，还有一些未被识别出的颜色），对应四种锥桶，位置也和 bag 里的坐标对得上。

---

### 5. 思考题

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
