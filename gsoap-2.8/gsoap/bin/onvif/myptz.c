#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "soapH.h"
#include "wsseapi.h"
#include "wsaapi.h"
#include "MediaBinding.nsmap"

//宏定义设备鉴权的用户名和密码
//注意对于海康相机而言，鉴权的用户名和密码需要单独设置，不一定等同于登录账户密码
//设置方法参考https://zongxp.blog.csdn.net/article/details/89632354
#define USERNAME    "admin"
#define PASSWORD    "Ab12345678"

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
}
