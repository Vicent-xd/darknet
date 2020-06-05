Yolov4 with HIKVISION camera SDK
tested on Ubuntu 20.04
          Opencv 4.2
          CUDA10 with CUDNN

Befor COMPILE change IP/PORT/USERNAME/PASSWD in src/image_hikcam.cpp

COMPILE:
          RUN make clean && make
TEST:
          RUN cd lib && ./net_cam_tinyv3_prn.sh

说明：
1.lib文件夹内容均为海康摄像头动态链接库
2.必须使用MAKEFILE进行编译，不能用CMAKE
3.多个线程同时运行
	hikcam_init //海康相机初始化，包括登录、解码、获得handle，初始化成功后延时5s
        fetch_in_thread //取图像线程，有改动
	detect_in_thread //推理线程，无改动
	hikcam_control_in_thread //云台电机控制线程，依据推理生成的全局detection类型指针"*dets"获得信息，使用run_detect_in_thread变量等待推理线程结束。
4.demo.c 中全局变量 hikcam/hikcontrol 默认为1，即默认开启从海康相机获取图像、开启控制，设置成0为不开启
5.编译完成后darknet在lib文件夹内，因为运行时需要海康动态链接库
6.hikcam_tinyv3_prn.sh 为运行脚本，其中data/cfg/weights可以调整成yolov4的（未测试）	
