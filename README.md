## 本程序可实现云台目标跟踪
## BEFORE:编译openssl1.0.2r
```
wget https://www.openssl.org/source/old/1.0.2/openssl-1.0.2r.tar.gz
cd openssl-1.0.2r
./configure --prefix=/usr/local/openssl
make -j$(nproc)
sudo make install
```
openssl安装路径需要与gsoap/bin/onvif/Makefile.inc内的 OPENSSL_DIR 一致，例如：
```
OPENSSL_DIR = /usr/local/openssl
```
安装libssl-dev
```
sudo apt install libssl-dev
```
## STEP1:编译gsoap
下载xavier分支代码
```
git clone -b xavier https://github.com/Vicent-xd/darknet.git
```
```
cd darknet/gsoap-2.8
automake --add-missing
autoreconf
（sudo apt-get install bison flex）
./configure --disable-ssl
make -j$(nproc)
```
yacc.h找不到解决办法：
```https://gitweb.gentoo.org/repo/gentoo.git/tree/net-libs/gsoap/files/gsoap-2.8.93-fix-parallel-build.patch?id=e035a4bf9fbd9853270a0a665ea80f59009b060d```
### onvif.h文件生成
```
cd bin/
cp ../wsdl/wsdl2h ./
cp ../src/soapcpp2 ./
wsdl2h -c -t ../typemap.dat -o onvif.h ./wsdl/devicemgmt.wsdl ./wsdl/event.wsdl ./wsdl/media.wsdl ./wsdl/ptz.wsdl
```
onvif.h内添加一行：#import "wsse.h"

### c文件生成
```
soapcpp2 -c -x onvif.h -I ../ -I ../import -I ../custom
cp *.nsmap onvif&&cp *.c onvif&&cp *.h onvif
```
### 编译测试文件myptz.c
```
cd onvif
make -j$(nproc)
```

## STEP2:
编译测试通过后将bin文件夹除了myptz.c/myptz.h外所有内容拷贝到darknet/src
```
mv myptz.c myptz.c.bak
mv myptz.h myptz.h.bak
cp *.c ../../../../../src && cp *.h ../../../../../src && cp *.nsmap ../../../../../src
```
## STEP3:
返回到darknet根目录
```
cd /root/darknet
```
编译darknet
```
make -j$(nproc)
```
## STEP4:
下载权重文件yolov4.weights
运行脚本
```
./net_cam_v4.sh 
```
### ps:ONVIF摄像头
快速运行

C控制程序 gsoap-2.8/gsoap/bin/onvif
进入路径运行一下命令进行编译运行
```
make && ./PTZ
```
PTZ是控制程序
myptz.c 主要代码

onvif python版编译运行参考
```
https://blog.csdn.net/zong596568821xp/article/details/89644654
```
C语言版编译运行参考
```
https://zongxp.blog.csdn.net/article/details/90108569
```
### 注意事项：
gsoap版本 gsoap_2.8.22.zip
```
https://sourceforge.net/projects/gsoap2/files/oldreleases/
```
error==4 错误修改
```
https://blog.csdn.net/u011124985/article/details/80046904
```
