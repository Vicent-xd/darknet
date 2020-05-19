Yolov4 with HIKVISION camera SDK
tested on Ubuntu 20.04
          Opencv 4.2
          CUDA10 with CUDNN

Befor COMPILE change IP/PORT/USERNAME/PASSWD in src/image_hikcam.cpp

COMPILE:
          RUN make clean && make
TEST:
          RUN cd lib && ./net_cam_tinyv3_prn.sh
