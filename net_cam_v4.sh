#rm test_dnn_out.avi
./darknet detector demo ./cfg/coco.data ./cfg/yolov4-ori-416.cfg ./weights/yolov4.weights rtsp://admin:Ab12345678@192.168.100.145:554 -i 0 -thresh 0.25
#./darknet detector demo ./cfg/coco.data ./cfg/yolov4-sam-mish.cfg ./weights/yolov4-sam-mish.weights rtsp://admin:Ab12345678@192.168.100.145:554 -i 0 -thresh 0.25


