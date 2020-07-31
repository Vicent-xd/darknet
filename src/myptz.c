#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "soapH.h"
#include "wsseapi.h"
#include "wsaapi.h"
#include "MediaBinding.nsmap"
#include "myptz.h"
//#ifndef IMAGE_OPENCV_H
#include "image_opencv.h"
#include "image.h"
#include "math.h"
//#endif
//宏定义设备鉴权的用户名和密码
//注意对于海康相机而言，鉴权的用户名和密码需要单独设置，不一定等同于登录账户密码
//设置方法参考https://zongxp.blog.csdn.net/article/details/89632354
#define USERNAME    "admin"
#define PASSWORD    "Ab12345678"

char * ip;
char Mediaddr[256]="";
char profile[256]="";
float pan = 1;
float panSpeed = 1;
float tilt = 1;
float tiltSpeed = 0.5;
float zoom = 0;
float zoomSpeed = 0.5;
struct soap soap;
struct _tds__GetCapabilities            	req;
struct _tds__GetCapabilitiesResponse    	rep;
struct _trt__GetProfiles 			getProfiles;
struct _trt__GetProfilesResponse		response;
struct _tptz__AbsoluteMove           absoluteMove;
struct _tptz__AbsoluteMoveResponse   absoluteMoveResponse;
struct _tptz__ContinuousMove           continuousMove;
struct _tptz__ContinuousMoveResponse   continuousMoveResponse;
char PTZendpoint[255];

SOAP_FMAC3 struct tt__PTZVector * SOAP_FMAC4 soap_new_tt__PTZVector(struct soap *soap, int n)
{
    struct tt__PTZVector *p;
    struct tt__PTZVector *a = (struct tt__PTZVector*)soap_malloc((soap), (n = (n < 0 ? 1 : n)) * sizeof(struct tt__PTZVector));
    for (p = a; p && n--; p++)
        soap_default_tt__PTZVector(soap, p);
    return a;
}

SOAP_FMAC3 struct tt__Vector2D * SOAP_FMAC4 soap_new_tt__Vector2D(struct soap *soap, int n)
{
    struct tt__Vector2D *p;
    struct tt__Vector2D *a = (struct tt__Vector2D*)soap_malloc((soap), (n = (n < 0 ? 1 : n)) * sizeof(struct tt__Vector2D));
    for (p = a; p && n--; p++)
        soap_default_tt__Vector2D(soap, p);
    return a;
}

SOAP_FMAC3 struct tt__PTZSpeed * SOAP_FMAC4 soap_new_tt__PTZSpeed(struct soap *soap, int n)
{
    struct tt__PTZSpeed *p;
    struct tt__PTZSpeed *a = (struct tt__PTZSpeed*)soap_malloc((soap), (n = (n < 0 ? 1 : n)) * sizeof(struct tt__PTZSpeed));
    for (p = a; p && n--; p++)
        soap_default_tt__PTZSpeed(soap, p);
    return a;
}

SOAP_FMAC3 struct tt__Vector1D * SOAP_FMAC4 soap_new_tt__Vector1D(struct soap *soap, int n)
{
    struct tt__Vector1D *p;
    struct tt__Vector1D *a = (struct tt__Vector1D*)soap_malloc((soap), (n = (n < 0 ? 1 : n)) * sizeof(struct tt__Vector1D));
    for (p = a; p && n--; p++)
        soap_default_tt__Vector1D(soap, p);
    return a;
}
int onvif_init(char* ip)
{
    soap_init(&soap);
    req.Category = (enum tt__CapabilityCategory *)soap_malloc(&soap, sizeof(int));
    req.__sizeCategory = 1;
    *(req.Category) = (enum tt__CapabilityCategory)0;
      //第一步：获取capability
    char endpoint[255];
    memset(endpoint, '\0', 255);
    sprintf(endpoint, "http://%s/onvif/device_service", ip);
    soap_call___tds__GetCapabilities(&soap, endpoint, NULL, &req, &rep);
    if (soap.error)
    {
        printf("[%s][%d]--->>> soap result: %d, %s, %s\n", __func__, __LINE__,
	                                        soap.error, *soap_faultcode(&soap),
	                                        *soap_faultstring(&soap));
    }
    else
	{
        printf("get capability success\n");
        printf("Dev_XAddr====%s\n",rep.Capabilities->Device->XAddr);
        printf("Med_XAddr====%s\n",rep.Capabilities->Media->XAddr);
        printf("PTZ_XAddr====%s\n",rep.Capabilities->PTZ->XAddr);
        strcpy(Mediaddr,rep.Capabilities->Media->XAddr);
    }
    printf("\n");

    //第二步：获取profile,需要鉴权
    //自动鉴权
    soap_wsse_add_UsernameTokenDigest(&soap, NULL, USERNAME, PASSWORD);

    //获取profile
    if(soap_call___trt__GetProfiles(&soap,Mediaddr,NULL,&getProfiles,&response)==SOAP_OK)
    {
        strcpy(profile, response.Profiles[0].token);
        printf("get profile succeed \n");
	    printf("profile====%s\n",profile);
    }
    else
    {
        printf("get profile failed \n");
	    printf("[%s][%d]--->>> soap result: %d, %s, %s\n", __func__, __LINE__,
	                                        soap.error, *soap_faultcode(&soap),
	                                        *soap_faultstring(&soap));
    }
    printf("\n");

    //第三步：PTZ结构体填充
    //char PTZendpoint[255];
    memset(PTZendpoint, '\0', 255);
    sprintf(PTZendpoint, "http://%s/onvif/PTZ", ip);
    printf("PTZendpoint is %s \n", PTZendpoint);

}
int tilt_up(void)
{
    continuousMove.ProfileToken = profile;
    continuousMove.Velocity = soap_new_tt__PTZSpeed(&soap, -1);
    continuousMove.Velocity->PanTilt = soap_new_tt__Vector2D(&soap, -1);
    continuousMove.Velocity->PanTilt->x = 0;
    continuousMove.Velocity->PanTilt->y = 1;

    soap_wsse_add_UsernameTokenDigest(&soap, NULL, USERNAME, PASSWORD);
    soap_call___tptz__ContinuousMove(&soap, PTZendpoint, NULL, &continuousMove,
                                     &continuousMoveResponse);
    printf("continuousMoveUP");
    sleep(0.1);
    return 0;
}
int tilt_down(void)
{
    continuousMove.ProfileToken = profile;
    continuousMove.Velocity = soap_new_tt__PTZSpeed(&soap, -1);
    continuousMove.Velocity->PanTilt = soap_new_tt__Vector2D(&soap, -1);
    continuousMove.Velocity->PanTilt->x = 0;
    continuousMove.Velocity->PanTilt->y = -1;

    soap_wsse_add_UsernameTokenDigest(&soap, NULL, USERNAME, PASSWORD);
    soap_call___tptz__ContinuousMove(&soap, PTZendpoint, NULL, &continuousMove,
                                     &continuousMoveResponse);
    printf("continuousMoveDOWN");
    sleep(0.1);
    return 0;
}
int pan_left(void)
{
    continuousMove.ProfileToken = profile;
    continuousMove.Velocity = soap_new_tt__PTZSpeed(&soap, -1);
    continuousMove.Velocity->PanTilt = soap_new_tt__Vector2D(&soap, -1);
    continuousMove.Velocity->PanTilt->x = -1;
    continuousMove.Velocity->PanTilt->y = 0;

    soap_wsse_add_UsernameTokenDigest(&soap, NULL, USERNAME, PASSWORD);
    soap_call___tptz__ContinuousMove(&soap, PTZendpoint, NULL, &continuousMove,
                                     &continuousMoveResponse);
    printf("continuousMoveLEFT");
    return 0;
}
int pan_right(void)
{
    continuousMove.ProfileToken = profile;
    continuousMove.Velocity = soap_new_tt__PTZSpeed(&soap, -1);
    continuousMove.Velocity->PanTilt = soap_new_tt__Vector2D(&soap, -1);
    continuousMove.Velocity->PanTilt->x = 1;
    continuousMove.Velocity->PanTilt->y = 0;

    soap_wsse_add_UsernameTokenDigest(&soap, NULL, USERNAME, PASSWORD);
    soap_call___tptz__ContinuousMove(&soap, PTZendpoint, NULL, &continuousMove,
                                     &continuousMoveResponse);
    printf("continuousMoveRIGHT");
    return 0;
}
int stop_move(void)
{
    continuousMove.ProfileToken = profile;
    continuousMove.Velocity = soap_new_tt__PTZSpeed(&soap, -1);
    continuousMove.Velocity->PanTilt = soap_new_tt__Vector2D(&soap, -1);
    continuousMove.Velocity->PanTilt->x = 0;
    continuousMove.Velocity->PanTilt->y = 0;

    soap_wsse_add_UsernameTokenDigest(&soap, NULL, USERNAME, PASSWORD);
    soap_call___tptz__ContinuousMove(&soap, PTZendpoint, NULL, &continuousMove,
                                     &continuousMoveResponse);
    printf("continuousMoveSTOP");
    return 0;
}
/*
int main(int argc, char** argv)
{
    struct soap soap;
	soap_init(&soap);

    char * ip;
    char Mediaddr[256]="";
    char profile[256]="";
    float pan = 1;
    float panSpeed = 1;
    float tilt = 1;
    float tiltSpeed = 0.5;
    float zoom = 0;
    float zoomSpeed = 0.5;
    struct _tds__GetCapabilities            	req;
    struct _tds__GetCapabilitiesResponse    	rep;
    struct _trt__GetProfiles 			getProfiles;
    struct _trt__GetProfilesResponse		response;
    struct _tptz__AbsoluteMove           absoluteMove;
    struct _tptz__AbsoluteMoveResponse   absoluteMoveResponse;
    struct _tptz__ContinuousMove           continuousMove;
    struct _tptz__ContinuousMoveResponse   continuousMoveResponse;

    req.Category = (enum tt__CapabilityCategory *)soap_malloc(&soap, sizeof(int));
    req.__sizeCategory = 1;
    *(req.Category) = (enum tt__CapabilityCategory)0;

    //第一步：获取capability
    char endpoint[255];
    memset(endpoint, '\0', 255);
    if (argc > 1)
    {
        ip = argv[1];
    }
    else
    {
        ip = "192.168.100.145";
    }
    sprintf(endpoint, "http://%s/onvif/device_service", ip);
    soap_call___tds__GetCapabilities(&soap, endpoint, NULL, &req, &rep);
    if (soap.error)
    {
        printf("[%s][%d]--->>> soap result: %d, %s, %s\n", __func__, __LINE__,
	                                        soap.error, *soap_faultcode(&soap),
	                                        *soap_faultstring(&soap));
    }
    else
	{
        printf("get capability success\n");
        printf("Dev_XAddr====%s\n",rep.Capabilities->Device->XAddr);
        printf("Med_XAddr====%s\n",rep.Capabilities->Media->XAddr);
        printf("PTZ_XAddr====%s\n",rep.Capabilities->PTZ->XAddr);
        strcpy(Mediaddr,rep.Capabilities->Media->XAddr);
    }
    printf("\n");

    //第二步：获取profile,需要鉴权
    //自动鉴权
    soap_wsse_add_UsernameTokenDigest(&soap, NULL, USERNAME, PASSWORD);

    //获取profile
    if(soap_call___trt__GetProfiles(&soap,Mediaddr,NULL,&getProfiles,&response)==SOAP_OK)
    {
        strcpy(profile, response.Profiles[0].token);
        printf("get profile succeed \n");
	    printf("profile====%s\n",profile);
    }
    else
    {
        printf("get profile failed \n");
	    printf("[%s][%d]--->>> soap result: %d, %s, %s\n", __func__, __LINE__,
	                                        soap.error, *soap_faultcode(&soap),
	                                        *soap_faultstring(&soap));
    }
    printf("\n");

    //第三步：PTZ结构体填充
    char PTZendpoint[255];
    memset(PTZendpoint, '\0', 255);
    sprintf(PTZendpoint, "http://%s/onvif/PTZ", ip);
    printf("PTZendpoint is %s \n", PTZendpoint);

//    absoluteMove.ProfileToken = profile;
//    //setting pan and tilt
//    absoluteMove.Position = soap_new_tt__PTZVector(&soap, -1);
//    absoluteMove.Position->PanTilt = soap_new_tt__Vector2D(&soap, -1);
//    absoluteMove.Speed = soap_new_tt__PTZSpeed(&soap, -1);
//    absoluteMove.Speed->PanTilt = soap_new_tt__Vector2D(&soap, -1);
//    //pan
//    absoluteMove.Position->PanTilt->x = pan;
//    absoluteMove.Speed->PanTilt->x = panSpeed;
//    //tilt
//    absoluteMove.Position->PanTilt->y = tilt;
//    absoluteMove.Speed->PanTilt->y = tiltSpeed;
//    //setting zoom
//    absoluteMove.Position->Zoom = soap_new_tt__Vector1D(&soap, -1);
//    absoluteMove.Speed->Zoom = soap_new_tt__Vector1D(&soap, -1);
//    absoluteMove.Position->Zoom->x = zoom;
//    absoluteMove.Speed->Zoom->x = zoomSpeed;
//
//    //第四步：执行绝对位置控制指令，需要再次鉴权
//    soap_wsse_add_UsernameTokenDigest(&soap, NULL, USERNAME, PASSWORD);
//    soap_call___tptz__AbsoluteMove(&soap, PTZendpoint, NULL, &absoluteMove,
//	                                        &absoluteMoveResponse);
//
//    printf("absoluteMove");


    continuousMove.ProfileToken = profile;
    continuousMove.Velocity = soap_new_tt__PTZSpeed(&soap, -1);
    continuousMove.Velocity->PanTilt = soap_new_tt__Vector2D(&soap, -1);
    continuousMove.Velocity->PanTilt->x = 0;
    continuousMove.Velocity->PanTilt->y = 1;

    soap_wsse_add_UsernameTokenDigest(&soap, NULL, USERNAME, PASSWORD);
    soap_call___tptz__ContinuousMove(&soap, PTZendpoint, NULL, &continuousMove,
                                     &continuousMoveResponse);
    printf("continuousMove");

    sleep(1);


    //第五步：清除结构体
    soap_destroy(&soap); // clean up class instances
    soap_end(&soap); // clean up everything and close socket, // userid and passwd were deallocated
    soap_done(&soap); // close master socket and detach context
    printf("\n");

    return 0;
}*/

int hikcam_logout(void)
{
    stop_move();
    //第五步：清除结构体
    soap_destroy(&soap); // clean up class instances
    soap_end(&soap); // clean up everything and close socket, // userid and passwd were deallocated
    soap_done(&soap); // close master socket and detach context
    //printf("\n");
}
float absf(float x){
    float res = 0;
    if(x>=0)res = x;
    else res = -1*x;
    return res;
}
int enalbe_hikcam_control(detection *dets,int num,float thresh,char **names, int classes)
{  
    int Ret=-1;
    int tracked=0;

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
        //if (class_id>=0) printf("class_id:%d,name:%s",class_id,names[class_id]);//std::cout<<"class_id"<<class_id<<"name:"<<names[class_id]<<std::endl;
        if (class_id >= 0 && strcmp(names[class_id],"person")==0) {
            int tolerance=0.4;//0~0.5
            int vtolerance=0.4;
	    tracked=1;
            box b = dets[i].bbox;
            /*if (std::isnan(b.w) || std::isinf(b.w)) b.w = 0.5;
            if (std::isnan(b.h) || std::isinf(b.h)) b.h = 0.5;
            if (std::isnan(b.x) || std::isinf(b.x)) b.x = 0.5;
            if (std::isnan(b.y) || std::isinf(b.y)) b.y = 0.5;
            */
	    if (isnan(b.w)==1 || isinf(b.w)!=0) b.w=0.5;
	    if (isnan(b.h)==1 || isinf(b.h)!=0) b.h=0.5;
	    if (isnan(b.x)==1 || isinf(b.x)!=0) b.x=0.5;
	    if (isnan(b.y)==1 || isinf(b.y)!=0) b.y=0.5; 
	    b.w = (b.w < 1) ? b.w : 1;
            b.h = (b.h < 1) ? b.h : 1;
            b.x = (b.x < 1) ? b.x : 1;
            b.y = (b.y < 1) ? b.y : 1;
            //NET_DVR_PTZControl_Other(lUserID, lChannel, dwPTZCommand, dwStop);

                //Ret=NET_DVR_PTZControl_Other(lUserID,1,PAN_LEFT,0);
            //stop condition
	    //printf("\n\nb.x:%f,b.y:%f,b.w:%f,b.h:%f \n",b.x,b.y,b.w,b.h);
            if (b.x>=(0.5-tolerance) && b.x<=(0.5+tolerance) && b.y>=(0.5-vtolerance) && b.y<=(0.5+vtolerance))
            {
                Ret=stop_move();//Ret=NET_DVR_PTZControl_Other(lUserID,1,PAN_LEFT,1);//no need to adjust
            }
            else
            {
	//	printf("\nb.x:%f\n",b.x);
          //      printf("\n|x-0.5|:%f,|y-0.5|:%f\n",absf(b.x-0.5),absf(b.y-0.5));
		 //1.up,down condition
		if (absf(b.x-0.5)<absf(b.y-0.5))
                {
	    //    printf("\nup,down\n");
                if (b.y<(0.5-vtolerance)) Ret=tilt_up();//NET_DVR_PTZControl_Other(lUserID,1,TILT_UP,0);//uper
                if (b.y>(0.5+vtolerance)) Ret=tilt_down();//NET_DVR_PTZControl_Other(lUserID,1,TILT_DOWN,0);//downer
                //std::cout<<"up,down\n"<<std::endl;
                }
                //2.left,right condition
                else
                {
	        //sleep(5);
	//	printf("\nleft,right\n");
                if (b.x<(0.5-tolerance)) Ret=pan_left();//NET_DVR_PTZControl_Other(lUserID,1,PAN_LEFT,0);//lefter
                if (b.x>(0.5+tolerance)) Ret=pan_right();//NET_DVR_PTZControl_Other(lUserID,1,PAN_RIGHT,0);//righter
                }
            }
            break;//quit 1st loop after person has been found
        }

    }
    if (tracked==0)
    {
    //Ret=NET_DVR_PTZControl_Other(lUserID,1,TILT_UP,1);//no person in all box
    Ret=stop_move();
    printf("\nNo person in this frame! stop ptz\n");
    //std::cout<<"\nNo person in this frame! stop ptz\n"<<std::endl;
    }


    //std::cout<<"HIKCAM control ret:"<<Ret<<std::endl;
    return Ret;
}
