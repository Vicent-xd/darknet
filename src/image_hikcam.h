#ifndef IMAGE_HIKCAM_H
#define IMAGE_HIKCAM_H
#pragma once
#include "image.h"
#include "matrix.h"
// #ifndef IMAGE_OPENCV_H
// #include "image_opencv.h"
// #endif
//#include "image_opencv.h"

#define HPR_ERROR       -1
#define HPR_OK           0
#define USECOLOR         0
// declaration
typedef void* mat_cv;
typedef void* cap_cv;
typedef void* write_cv;


#ifdef __cplusplus
extern "C" {
#endif

int hikcam_init(char* ip, char* usr, char* password,int port);
image get_image_from_hikcam_resize(int w, int h, int c, mat_cv** in_img);

#ifdef __cplusplus
}
#endif

#endif // IMAGE_HIKCAM_H