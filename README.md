## 本程序可实现云台目标跟踪
## STEP1:编译gsoap
下载multi-platform分支代码
automake --add-missing
autoreconf
（sudo apt-get install bison flex）
https://gitweb.gentoo.org/repo/gentoo.git/tree/net-libs/gsoap/files/gsoap-2.8.93-fix-parallel-build.patch?id=e035a4bf9fbd9853270a0a665ea80f59009b060d
./configure --disable-ssl

### onvif.h文件生成
cd bin/
wsdl2h -c -t ../typemap.dat -o onvif.h ./wsdl/devicemgmt.wsdl ./wsdl/event.wsdl ./wsdl/media.wsdl ./wsdl/ptz.wsdl

#import "wsse.h" >> onvif.h

### c文件生成
soapcpp2 -c -x onvif.h -I ../ -I ../import -I ../custom

cp *.nsmap onvif
cp *.c onvif
cp *.h onvif

### ONVIF摄像头
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

### 注意事项：
gsoap版本 gsoap_2.8.22.zip
https://sourceforge.net/projects/gsoap2/files/oldreleases/
error==4 错误修改
https://blog.csdn.net/u011124985/article/details/80046904

## STEP2:将bin文件夹所有内容拷贝到src
