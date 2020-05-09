#rm test_dnn_out.avi

./darknet detector demo ./cfg/coco.data ./cfg/yolov3-tiny-prn.cfg ./weights/yolov3-tiny-prn.weights rtsp://admin:Ab12345678@172.17.1.99:554 -i 0 -thresh 0.25



