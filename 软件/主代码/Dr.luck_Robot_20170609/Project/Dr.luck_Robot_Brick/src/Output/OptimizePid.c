/**
  ******************************************************************************
  * @file    EV3 Brick of Dr.luck/OptimizePid.c 
  * @author  STU AI & Robot Lab
  * @version V1.0.0
  * @date    14-Nov-2015
  * @brief   Main program body according to ST source
  ******************************************************************************
**/
#include "OptimizePid.h"

//-----------------����ʽPID�㷨�Ż�-------------------------------------
//
float in_kp = 3;
float in_ki = 5;
float in_kd = 1;

float PID_IncrementMode(float ValueWant,float ValueNow)
{
	 static float dErrP, dErrI, dErrD;
	 static float errNow,errOld1=0,errOld2=0;
	 static float dCtrOut,ctrOut,ctrOutOld=0;
	 static float Ud,UdOld=0,a=0.3;
	
	 //==============����׼��=================
	 errNow = ValueWant - ValueNow;
	 
	 //--------------��������----------------------
	 if(errNow>-2&&errNow<2)	errNow = 0;

	 dErrP = errNow - errOld1;
	 dErrI = errNow;
	 dErrD = errNow - 2 * errOld1 + errOld2;

	 //============����ʽPID����=================
	 //---���벻��ȫ΢�֣������ͨ�˲�������������΢�����ù�������,����ϵͳ���ƹ�����
	 Ud = in_kd*(1-a)*dErrD + a*UdOld;		//aС��1
	 
	 //---������ַ��룬����������Ŀ����ƫ��ϴ�ȡ����������---
	 if(errNow<-20||errNow>20){			//���ַ���ֵ����ʵ�����ȷ��
			dCtrOut = in_kp * dErrP +  Ud;
	 }else{
			dCtrOut = in_kp * dErrP + in_ki * dErrI + Ud;
	 }
	 
	 //================���======================

	 //----���뿹���ֱ��ͣ������˳�������(������)ʱ��---
	 if(ctrOutOld>50){
			if(dCtrOut<0)	ctrOut = ctrOutOld + dCtrOut;
			else ctrOut = ctrOutOld;
	 }
	 else if(ctrOutOld<-50){
			if(dCtrOut>0)	ctrOut = ctrOutOld + dCtrOut;
			else ctrOut = ctrOutOld;
	 }
	 
	 //===============��������======================
	 errOld2 = errOld1; //�������΢��
	 errOld1 = errNow;  //һ�����΢��
	 ctrOutOld = ctrOut;
	 UdOld = Ud;
	 
	 return ctrOut;
}


//-----------------λ��ʽPID�㷨�Ż�-------------------------------------

float ab_kp = 0.25;
float ab_ki = 0.3;
float ab_kd = 0.1;

float PID_AbsoluteMode(u8 Ochannel,float ValueWant,float ValueNow)
{
	 float errP, errD, errILim=30;
	 static float errI[4], errOld[4]={0,0,0,0}, UdOld[4]={0,0,0,0};
	 float errNow, ctrOut, Ud, a=0.3;
	 
	 //==============����׼��=================
	 errNow = ValueNow - ValueWant;
	 //--------------��������----------------------
//	 if(errNow>-3&&errNow<3)	errNow = 0;

	 errP = errNow;  				//��ȡ���ڵ�������kp����
	 errI[Ochannel-5] += errNow; 				//�����֣�����ki����
	 //--------------�����������----------------------
	 if(errILim != 0){	   	//�������޺�����
			if(errI[Ochannel-5] > errILim)  errI[Ochannel-5] = errILim;
			else if(errI[Ochannel-5] < -errILim)    errI[Ochannel-5] = -errILim;
	 }
	 errD = errNow - errOld[Ochannel-5];//���΢�֣�����kd����

	 //============PID����=================
	 //---���벻��ȫ΢�֣������ͨ�˲�������������΢�����ù�������,����ϵͳ���ƹ�����
	 Ud = ab_kd*(1-a)*errD + a*UdOld[Ochannel-5];		//aС��1
	 
	 //---������ַ��룬����������Ŀ����ƫ��ϴ�ȡ����������---
	 if(errNow<-20||errNow>20){			//���ַ���ֵ����ʵ�����ȷ��
			ctrOut = ab_kp * errP + Ud;
	 }else{
			ctrOut = ab_kp * errP + ab_ki * errI[Ochannel-5] + Ud;
	 }
	 
	 if(ctrOut>100)	
		 ctrOut = 100;
	 else if(ctrOut<=21)	
		 ctrOut = 21;
	
	 //===============��������======================
	 errOld[Ochannel-5] = errNow;	//�������ڵ����
	 UdOld[Ochannel-5] = Ud;
	 
	 return ctrOut;

}

//-----------------λ��ʽPID2�㷨�Ż�-------------------------------------

float ab_kp2 = 0.5;
float ab_ki2 = 0.6;
float ab_kd2 = 0;

float PID_AbsoluteMode2(u8 DeviceID, u8 Ochannel,float ValueWant,float ValueNow)//����޸�12.12
{
	 float errP, errD, errILim=2;
	 static float errI[4], errOld[4]={0,0,0,0}, UdOld[4]={0,0,0,0};
	 float errNow, ctrOut, Ud, a=0.3;
	 float Comp_ctrOut = 0;			//�������
	 
		switch(DeviceID)	//���ò���
		{
			case 0x0a:      //���͵��
				ab_kp2 = 0.5;
				ab_ki2 = 0.6;
				ab_kd2 = 0;
				break;
			case 0x04:			//���͵��
				ab_kp2 = 0.4;
				ab_ki2 = 1;
				ab_kd2 = 0;				
				break;
		}	 
	 
	 ValueNow = ValueNow;//����޸�12.12
	 
	 //==============����׼��=================
	 errNow = ValueWant - ValueNow;
	 //--------------��������----------------------
	 if(errNow>-2&&errNow<2)	errNow = 0;

	 errP = errNow;  				//��ȡ���ڵ�������kp����
	 errI[Ochannel-5] += errNow; 				//�����֣�����ki����
	 //--------------�����������----------------------
	 if(errILim != 0){	   	//�������޺�����
			if(errI[Ochannel-5] > errILim)  errI[Ochannel-5] = errILim;
			else if(errI[Ochannel-5] < -errILim)    errI[Ochannel-5] = -errILim;
	 }
	 errD = errNow - errOld[Ochannel-5];//���΢�֣�����kd����

	 //============PID����=================
	 //---���벻��ȫ΢�֣������ͨ�˲�������������΢�����ù�������,����ϵͳ���ƹ�����
	 Ud = ab_kd2*(1-a)*errD + a*UdOld[Ochannel-5];		//aС��1
	 
	 //---������ַ��룬����������Ŀ����ƫ��ϴ�ȡ����������---
	 if(errNow<-5||errNow>5){			//���ַ���ֵ����ʵ�����ȷ��
			ctrOut = ab_kp2 * errP + Ud;
	 }else{
			ctrOut = ab_kp2 * errP + ab_ki2 * errI[Ochannel-5] + Ud;
	 }
	 
	 if(ValueWant>80||ValueWant<-80)	
		 Comp_ctrOut = 80;
	 else if(ValueWant>70||ValueWant<-70)	
		 Comp_ctrOut = 70;
	 else if(ValueWant>50||ValueWant<-50)	
		 Comp_ctrOut = 65;
	 else if(ValueWant>30||ValueWant<-30)	
		 Comp_ctrOut = 55;
	 else if(ValueWant>=10||ValueWant<-10)		
		 Comp_ctrOut = 50;
	 else if(ValueWant>0||ValueWant<0)		
		 Comp_ctrOut = 40;
	 
	 if(ValueWant>0)	
		 ctrOut += Comp_ctrOut;
	 else if(ValueWant<0)	
		 ctrOut -= Comp_ctrOut;
	
	 //===============��������======================
	 errOld[Ochannel-5] = errNow;	//�������ڵ����
	 UdOld[Ochannel-5] = Ud;
	 
	 return ctrOut;

}




