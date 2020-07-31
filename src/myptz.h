#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "soapH.h"
#include "wsseapi.h"
#include "wsaapi.h"
//#include "MediaBinding.nsmap"
#include "darknet.h"

SOAP_FMAC3 struct tt__PTZVector * SOAP_FMAC4 soap_new_tt__PTZVector(struct soap *soap, int n);
SOAP_FMAC3 struct tt__Vector2D * SOAP_FMAC4 soap_new_tt__Vector2D(struct soap *soap, int n);
SOAP_FMAC3 struct tt__PTZSpeed * SOAP_FMAC4 soap_new_tt__PTZSpeed(struct soap *soap, int n);
SOAP_FMAC3 struct tt__Vector1D * SOAP_FMAC4 soap_new_tt__Vector1D(struct soap *soap, int n);
int onvif_init(char* ip);
int hikcam_logout(void);
int enalbe_hikcam_control(detection *dets,int num,float thresh,char **names, int classes);
int tilt_up(void);
int tilt_down(void);
int pan_left(void);
int pan_right(void);
int stop_move(void);
