# xinzhu_nav
特此说明：本项目适合初次接触ros1与导航的同学，项目只适用与ros1

环境：
    ros-noetic
    ubuntu 20.04


Livox和Fast-Lio配置流程

1.安装Livox_sdk2,readme有写相关过程，注意：要更改主机ip为192.168.1.50ubuntu修改方法，本人雷达ip为192.168.1.180
2.安装livox_ros_driver2, readme有写相关过程， 注意运行前要注意更改config里面的主机ip和雷达IP
鱼香ros安装ros1 
sudo apt install libeigen3-dev
sudo apt install libpcl-dev
# 编译fast-lio
cd ~/$A_ROS_DIR$/src
git clone https://github.com/hku-mars/FAST_LIO.git
cd FAST_LIO
git submodule update --init
cd ../..
catkin_make
source devel/setup.bash
#注意，如果使用的是mid360，即使用的是livox_ros_driver2而非1的话，
#需要前往fast-lio的CmakeLists文件修改find_package里的livox_ros_driver为livox_ros_driver2，同时package.xml里面的也一样

# 安装sophus
git clone https://github.com/strasdat/Sophus.git
cd Sophus
git checkout a621ff
mkdir build
cd build
cmake ../ -DUSE_BASIC_LOGGING=ON
make
sudo make install

# 所需包
sudo apt install ros-$ROS_DISTRO-ros-numpy
pip install numpy==1.21
pip install open3d

在配置时，我们发现了一些问题，主要还是版本导致。
一个是原仓库使用的是python2，python2使用和安装外部库已经不太方便了，所以换成了python3。
另一个是FAST_LIO_LOCALIZATION里面也包含了FAST_LIO，但是这里面的FAST_LIO还是比较老的版本，建议还是从最新的FAST_LIO仓库里拉最新的(因为做了mid360的适配)，然后记得在cmakelist里修改生成的可执行文件的名称，否则会和上一步的fast_lio的mapping重复报错。
所以想使用的话建议还是直接使用我们的sentry_slam/FAST_LIO_LOCALIZATION和sentry_slam/FAST_LIO这两个修改好的包。

usage：
 roslaunch livox_ros_driver2 msg_MID360.launch

 roslaunch fast_lio_localization localization_MID360.launch 

# 发布初始位姿(也可以用rviz，第一次尝试的时候更建议使用rviz)
 rosrun fast_lio_localization publish_initial_pose.py 0 0 0 0 0 0
 also you could publish your initial point use RVIZ
 这里的原点是你建图时候的起点。

地图转换主要是因为move_base是基于2d的栅格地图进行路径规划，而fast_lio默认的输出地图是三维点云的PCD文件，我们需要用一些方法获取2d的栅格地图：
sudo apt install ros-noetic-map-server
# 打开一个终端.(ctrl+alt+T)输入下面指令安装octomap.
 sudo apt-get install ros-noetic-octomap-ros #安装octomap
 sudo apt-get install ros-noetic-octomap-msgs
 sudo apt-get install ros-noetic-octomap-server
 
# 安装octomap 在 rviz 中的插件
 sudo apt-get install ros-noetic-octomap-rviz-plugins
# install move_base
 sudo apt-get install ros-noetic-move-base

使用octomap_server功能包中的octomap_server_node节点, 实时读取三维点云, 并生成栅格地图.

我们在 FAST_LIO 功能包中添加了 Pointcloud2Map.launch, which will update the 2D mapping at same time, if you publish the PointCloud2 from FAST_LIO.

然后我们综合了 SLAM, relocalization, 实时构建栅格地图三个功能, in only one launch file ==> sentry_build.launch.

运行此功能包后，系统便会开始自动同步构建栅格地图.

如果你对你构建的三维点云地图和二维栅格地图满意，并希望保存下来：

三维点云地图pcd文件会在sentry_build.launch运行结束后自动保存到fast_lio/PCD文件夹下
如果你希望保存二位栅格地图，请运行以下命令：

# save the pgm map file
 rosrun map_server map_saver map:=/<Map Topic> -f PATH_TO_YOUR_FILE/mymap
 #eg，举例:
 rosrun map_server map_saver map:=/projected_map -f /home/rm/ws_sentry/src/FAST_LIO/PCD/scans

# 坐标系映射

 input：body frame，即机器人在三维点云坐标系下的位姿
 output: body_2d frame，即机器人在二维栅格地图坐标系下的位姿

 由于fast_lio_localization输出的 body frame是当前机器人在三维点云坐标系下的位姿，而move_base需要的map frame是二维栅格地图坐标系下的坐标body_2d，因此需要进行坐标系转换。

 此处需要根据你的雷达安装方式进行相应的转换。如：你的雷达是正向安装，或者正向稍微倾斜一点安装，即mid360的底部始终指向地面。这种情况下，对于坐标系的位置，只需要把body frame中的(x,y,z)取(x,y,0)赋给body_2d即可。同时，对于坐标系的姿态，把body frame 中的四元数(x,y,z,w)取(0,0,z,w)赋给body_2d即可。

 这样最后的效果就是，3d中的机器人坐标系映射到二维body_2d，位置信息z始终为0，而位置信息x，y始终跟随body frame同步。同时姿态信息只有yaw轴会跟着3d中的机器人同步。这样坐标系body_2d的xy轴就可以始终贴合地图，便于路径规划。

 如果你的机器人将mid360反装，即底部指向天空，你需要修改tf的转换
