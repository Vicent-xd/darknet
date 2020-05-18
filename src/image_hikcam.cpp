#include "image_hikcam.h"
//#ifndef IMAGE_OPENCV_H
#include "image_opencv.h"
//#endif
#include <iostream>

#include <stdio.h>
#include <string.h>

#include "HCNetSDK.h"
#include "PlayM4.h"
#include "LinuxPlayM4.h"

#include <unistd.h>
#include "opencv2/opencv.hpp"

using namespace std;
using namespace cv;
cv::Mat dst_hik;
//cv::Mat mat;
LONG lUserID;
//cv::Mat src;
//cv::Mat* ptr
pthread_mutex_t dst_lock;//=PTHREAD_MUTEX_INITIALIZER;
//pthread_mutex_init(&dst_lock,NULL);
//extern mat_cv* dst_hik_ptr=(mat_cv*)&dst_hik;
//extern "C" mat_cv* dst_hik_ptr;
//cv::Mat new_img = cv::Mat(h, w, CV_8UC(c));
cv::Mat new_img = cv::Mat();
LONG nPort=-1;
HWND h = NULL;
std::list<cv::Mat> g_frameList;
//int frameok=-1;
//extern "C" cv::Mat image_to_mat(image img);
extern "C" image mat_to_image(cv::Mat mat);

void CALLBACK DecCBFun(LONG nPort, char *pBuf, LONG nSize, FRAME_INFO *pFrameInfo, void* nReserved1, LONG nReserved2)
{
    long lFrameType = pFrameInfo->nType;
     //std::cerr << lFrameType << std::endl; 
     if (lFrameType == T_YV12)
     {
      //cv::Mat dst(pFrameInfo->nHeight, pFrameInfo->nWidth,
      //            CV_8UC3);  // 8UC3表示8bit uchar无符号类型,3通道值
           //frameok=0;
           //static cv::Mat dst_hik;
           //cv::Mat *ptr=&dst_hik;
           
           dst_hik.create(pFrameInfo->nHeight, pFrameInfo->nWidth,
                 CV_8UC3);
 
           cv::Mat src(pFrameInfo->nHeight + pFrameInfo->nHeight / 2, pFrameInfo->nWidth, CV_8UC1, (uchar *)pBuf);
           //cv::cvtColor(src, dst_hik, cv::COLOR_YUV2BGR_YV12);//CV_YUV2BGR_YV12);
           
           cv::cvtColor(src, dst_hik, cv::COLOR_YUV2BGR_YV12);
           //dst_hik_ptr=(mat_cv*)ptr;
           pthread_mutex_lock(&dst_lock);
           //frameok=1;
           g_frameList.clear();
           g_frameList.push_back(dst_hik);
	         //cv::imshow("bgr", dst_hik);
           //cv::waitKey(10);
           pthread_mutex_unlock(&dst_lock);
     }
    usleep(1000);
   //cv::Mat src(pFrameInfo->nHeight + pFrameInfo->nHeight / 2, pFrameInfo->nWidth, CV_8UC1, (uchar *)pBuf);
   //cv::cvtColor(src, dst, CV_YUV2BGR_YV12);
   //cv::imshow("bgr", dst);
   //pthread_mutex_lock(&mutex);
   //g_frameList.push_back(dst);
   //pthread_mutex_unlock(&mutex);
   //vw << dst;
   //cv::waitKey(10);
 
}
void CALLBACK g_RealDataCallBack_V30(LONG lRealHandle, DWORD dwDataType, BYTE *pBuffer, DWORD dwBufSize,DWORD dwUser)
{
   /*
   if (dwDataType == 1)
   {
       PlayM4_GetPort(&nPort);
       PlayM4_SetStreamOpenMode(nPort, STREAME_REALTIME);
       PlayM4_OpenStream(nPort, pBuffer, dwBufSize, 1024 * 1024);
       PlayM4_SetDecCallBackEx(nPort, DecCBFun, NULL, NULL);
       PlayM4_Play(nPort, h);
   }
   else
   {
       BOOL inData = PlayM4_InputData(nPort, pBuffer, dwBufSize);
   }*/
   //std::cerr << dwDataType << std::endl;
   DWORD dRet;
   switch (dwDataType)
   {
     case NET_DVR_SYSHEAD:           //系统头
       if (!PlayM4_GetPort(&nPort))  //获取播放库未使用的通道号
       {
         break;
       }
       if (dwBufSize > 0) {
         if (!PlayM4_SetStreamOpenMode(nPort, STREAME_REALTIME)) {
           dRet = PlayM4_GetLastError(nPort);
           break;
         }
         if (!PlayM4_OpenStream(nPort, pBuffer, dwBufSize, 1024 * 1024)) {
           dRet = PlayM4_GetLastError(nPort);
           break;
         }
         //设置解码回调函数 只解码不显示
         if (!PlayM4_SetDecCallBack(nPort, DecCBFun)) {
            dRet = PlayM4_GetLastError(nPort);
            break;
         }
 
         //设置解码回调函数 解码且显示
         /*if (!PlayM4_SetDecCallBackEx(nPort, DecCBFun, NULL, NULL))
         {
           dRet = PlayM4_GetLastError(nPort);
           break;
         }*/
 
         //打开视频解码
         if (!PlayM4_Play(nPort, h))
         {
           dRet = PlayM4_GetLastError(nPort);
           break;
         }
 
         //打开音频解码, 需要码流是复合流
         /*if (!PlayM4_PlaySound(nPort)) {
           dRet = PlayM4_GetLastError(nPort);
           break;
         }*/
       }
       break;
       //usleep(500);
     case NET_DVR_STREAMDATA:  //码流数据
       if (dwBufSize > 0 && nPort != -1) {
         BOOL inData = PlayM4_InputData(nPort, pBuffer, dwBufSize);
         while (!inData) {
           sleep(100);
           inData = PlayM4_InputData(nPort, pBuffer, dwBufSize);
           std::cerr << "PlayM4_InputData failed \n" << std::endl;
         }
       }
       break;
   }
}
void CALLBACK g_ExceptionCallBack(DWORD dwType, LONG lUserID, LONG lHandle, void *pUser)
{
    char tempbuf[256] = {0};
    switch(dwType) 
    {
    case EXCEPTION_RECONNECT:			
        printf("pyd----------reconnect--------%d\n", time(NULL));
        break;
    default:
        break;
    }
}
extern "C" int hikcam_init(char* ip, char* usr, char* password,int port)
{
    //pthread_t hThread;
    pthread_mutex_init(&dst_lock, NULL);
    NET_DVR_Init();
    NET_DVR_SetExceptionCallBack_V30(0, NULL, g_ExceptionCallBack, NULL);
    //long lUserID;
    //char cUserChoose = 'r';
    //login
    NET_DVR_USER_LOGIN_INFO struLoginInfo = {0};
    NET_DVR_DEVICEINFO_V40 struDeviceInfoV40 = {0};
    struLoginInfo.bUseAsynLogin = false;


    struLoginInfo.wPort = 8000;
    memcpy(struLoginInfo.sDeviceAddress, "172.17.1.99", NET_DVR_DEV_ADDRESS_MAX_LEN);
    memcpy(struLoginInfo.sUserName, "admin", NAME_LEN);
    memcpy(struLoginInfo.sPassword, "Ab12345678", NAME_LEN);
    lUserID = NET_DVR_Login_V40(&struLoginInfo, &struDeviceInfoV40);

    if (lUserID < 0)
    {
        std::cerr << "pyd1---Login error," << NET_DVR_GetLastError() << std::endl ;
        //printf("pyd1---Login error, %d\n", NET_DVR_GetLastError());
        return HPR_ERROR;
    }
    printf("Login SUCCESS\n");
    long lRealPlayHandle;
    NET_DVR_PREVIEWINFO struPlayInfo = {0};
    struPlayInfo.lChannel     = 1;  //channel NO
    struPlayInfo.dwLinkMode   = 0;
    struPlayInfo.bBlocked = 1;
    struPlayInfo.dwDisplayBufNum = 1;
    lRealPlayHandle = NET_DVR_RealPlay_V40(lUserID, &struPlayInfo, NULL, NULL);
    
    if (lRealPlayHandle < 0)
    {
        std::cerr << "pyd1---NET_DVR_RealPlay_V40 error\n";
        //printf("pyd1---NET_DVR_RealPlay_V40 error\n");
        NET_DVR_Logout(lUserID);
        NET_DVR_Cleanup();
        return HPR_ERROR;
    }
    //printf("Init RealPlay SUCCESS\n");
    int iRet=0;
    iRet = NET_DVR_SetRealDataCallBack(lRealPlayHandle, g_RealDataCallBack_V30,0);
    if (!iRet)
    {
        std::cerr << "pyd1---NET_DVR_RealPlay_V40 error\n";
        //printf("pyd1---NET_DVR_RealPlay_V40 error\n");
        NET_DVR_StopRealPlay(lRealPlayHandle);
        NET_DVR_Logout_V30(lUserID);
        NET_DVR_Cleanup();  
        //return iRet;
        return HPR_ERROR;
    }
    printf("Init RealPlay SUCCESS\n");
    //cv::Mat *img_ptr=&dst_hik;
   // img_ptr=&dst_hik;
    // cv::Mat *src = &dst_hik;
    // *(cv::Mat **)in_img = src;

    //Demo_RealPlay(lUserID);
    //return (mat_cv*)img_ptr;
    return HPR_OK;    
}
extern "C" int hikcam_logout(void)
{
	NET_DVR_PTZControl_Other(lUserID,1,PAN_LEFT,1);
        NET_DVR_Logout_V30(lUserID);
        NET_DVR_Cleanup();
	return 0;
}
extern "C" image get_image_from_hikcam_resize(int w, int h, int c, mat_cv** in_img)
{
    c = c ? c : 3;
    //cv::Mat *src = NULL;//lock it while returning 
    cv::Mat *src=NULL;
    //cv::Mat mat;
    //mat = new cv::Mat(h, w, CV_8UC(c));
    //src = new cv::Mat();
    //src = new cv::Mat(h, w, CV_8UC(c));
    static int once = 1;
    new_img = cv::Mat(h, w, CV_8UC(c));
    pthread_mutex_lock(&dst_lock);
    src=(cv::Mat *)&g_frameList.front();
    
    //mat=g_frameList.front();
    //pthread_mutex_unlock(&dst_lock);
    /*if (!g_frameList.front().empty()) 
    {
      src=(cv::Mat *)&g_frameList.front();
      *in_img = (mat_cv *)new cv::Mat(src->rows, src->cols, CV_8UC(c));
      cv::resize(*src, **(cv::Mat**)in_img, (*(cv::Mat**)in_img)->size(), 0, 0, cv::INTER_LINEAR);
    }
    else 
      {
        pthread_mutex_unlock(&dst_lock);
        return make_random_image(w, h, c);
      }*/
    //show_image_mat(*in_img, "in_img");
    if((*src).empty())
    {   
      pthread_mutex_unlock(&dst_lock);
      return make_empty_image(0, 0, 0);
    }
    //src=(cv::Mat *)&mat;
    *in_img = (mat_cv *)new cv::Mat(src->rows, src->cols, CV_8UC(c));
    cv::resize(*src, **(cv::Mat**)in_img, (*(cv::Mat**)in_img)->size(), 0, 0, cv::INTER_LINEAR);   
    //cv::Mat new_img = cv::Mat(h, w, CV_8UC(c));
    cv::resize(*src, new_img, new_img.size(), 0, 0, cv::INTER_LINEAR);
    pthread_mutex_unlock(&dst_lock);
    //if (c>1) cv::cvtColor(new_img, new_img, cv::COLOR_RGB2BGR);
    image im = mat_to_image(new_img);
    //cv::imshow("bgr", src);
    //cv::waitKey(10);
    //delete src;
    //delete mat;
    //release_mat((mat_cv **)&src);
    // show_image_mat(*in_img, "in_img");
    //if (!im.data) return make_random_image(w, h, c);
    return im; 
}
extern "C" int enalbe_hikcam_control(detection *dets,int num,float thresh,char **names, int classes)
{  
    int Ret=-1;
    int tracked=0;
    try {
      int i, j;
      for (i = 0; i < num; ++i) {
            char labelstr[4096] = { 0 };
            int class_id = -1;
            for (j = 0; j < classes; ++j) {
                //int show = strncmp(names[j], "dont_show", 9);
                if (dets[i].prob[j] > thresh) {
                    if (class_id < 0) {
                        strcat(labelstr, names[j]);
                        class_id = j;
                        char buff[10];
                        //sprintf(buff, " (%2.0f%%)", dets[i].prob[j] * 100);
                        strcat(labelstr, buff);
                        //printf("%s: %.0f%% ", names[j], dets[i].prob[j] * 100);
                    }
                    else {
                        strcat(labelstr, ", ");
                        strcat(labelstr, names[j]);
                        //printf(", %s: %.0f%% ", names[j], dets[i].prob[j] * 100);
                    }
                }
            }
            if (class_id>=0) std::cout<<"class_id"<<class_id<<"name:"<<names[class_id]<<std::endl;
            if (class_id >= 0 && strcmp(names[class_id],"person")==0) {
                int tolerance=0.4;//0~0.5
                tracked=1;
                box b = dets[i].bbox;
                if (std::isnan(b.w) || std::isinf(b.w)) b.w = 0.5;
                if (std::isnan(b.h) || std::isinf(b.h)) b.h = 0.5;
                if (std::isnan(b.x) || std::isinf(b.x)) b.x = 0.5;
                if (std::isnan(b.y) || std::isinf(b.y)) b.y = 0.5;
                b.w = (b.w < 1) ? b.w : 1;
                b.h = (b.h < 1) ? b.h : 1;
                b.x = (b.x < 1) ? b.x : 1;
                b.y = (b.y < 1) ? b.y : 1;
                //NET_DVR_PTZControl_Other(lUserID, lChannel, dwPTZCommand, dwStop);

	       	      //Ret=NET_DVR_PTZControl_Other(lUserID,1,PAN_LEFT,0);
                //stop condition
                if (b.x>=(0.5-tolerance) && b.x<=(0.5+tolerance) && b.y>=(0.5-tolerance) && b.y<=(0.5+tolerance))
                {
                  Ret=NET_DVR_PTZControl_Other(lUserID,1,PAN_LEFT,1);//no need to adjust
                }
                else
                {
                  //1.up,down condition
                  if (abs(b.x-0.5)<abs(b.y-0.5))
                  {
                    if (b.y<(0.5-tolerance)) Ret=NET_DVR_PTZControl_Other(lUserID,1,TILT_UP,0);//uper
                    if (b.y>(0.5+tolerance)) Ret=NET_DVR_PTZControl_Other(lUserID,1,TILT_DOWN,0);//downer
                    std::cout<<"up,down\n"<<std::endl;
                  }
                  //2.left,right condition
                  else
                  {
                    if (b.x<(0.5-tolerance)) Ret=NET_DVR_PTZControl_Other(lUserID,1,PAN_LEFT,0);//lefter
                    if (b.x>(0.5+tolerance)) Ret=NET_DVR_PTZControl_Other(lUserID,1,PAN_RIGHT,0);//righter
                  }
                }
                break;//quit 1st loop after person has been found
            }

      }
      if (tracked==0)
      {
        Ret=NET_DVR_PTZControl_Other(lUserID,1,TILT_UP,1);//no person in all box
        std::cout<<"\nNo person in this frame! stop ptz\n"<<std::endl;
      }
      
    }
    catch (...) {
        std::cerr << "Hikcam_control exception: enalbe_hikcam_control() \n";
    }
    std::cout<<"HIKCAM control ret:"<<Ret<<std::endl;
    return Ret;
}
