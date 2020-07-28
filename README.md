ONVIF摄像头
快速运行
树莓派路径
C控制程序 /home/pi/onvif/gsoap-2.8/gsoap/bin/onvif
python控制程序 /home/pi/onvif.py

Xavier路径
C控制程序 /home/mbab/gsoap-2.8/gsoap/bin/onvif

进入路径运行一下命令进行编译运行
make && ./PTZ

PYZ是控制程序
myptz.c 主要代码

onvif python版编译运行参考
https://blog.csdn.net/zong596568821xp/article/details/89644654
C语言版编译运行参考
https://zongxp.blog.csdn.net/article/details/90108569

注意事项：
gsoap版本 gsoap_2.8.22.zip
https://sourceforge.net/projects/gsoap2/files/oldreleases/
error==4 错误修改
https://blog.csdn.net/u011124985/article/details/80046904

## 将bin文件夹所有内容拷贝到src
