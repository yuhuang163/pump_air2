/*****************************************************************************************************************
版权所有：Shenzhen Root Inonvation
项目名称：V3pro
项目编号：
文 件 名：
生成日期：2026.03.26
作    者：sahar.hu
功能说明：V3pro pressure compensation functions
其他说明：
修改记录：
修改日期：
修 改 者：
修改内容：
******************************************************************************************************************/

#pragma once
#ifndef V3_PRESSURE_COMPENSATION_H
#define V3_PRESSURE_COMPENSATION_H

#include "v3_Defined.h"

// functions
void V3ProModeReset(unsigned char uLevel, unsigned char uMode, unsigned int uChannelIdx);
void V3ProPressureDataPreprocess(algo_air_pressure_data_t sPressureData, unsigned char uchannelIdx);
void PressureCompensationAnalysis(float fDataBuff, SPUMPFEATURE_INFO tFeatureInfoTemp, SDEVICE_INFO tDeviceInfoTemp);
// 
// interface
void V3ProAlgoInit(algo_air_pressure_data_t sPressureData);
void V3ProAlgoMain(algo_air_pressure_data_t sPressureData);
void GetCompensationStatus(unsigned char* uCompsStatus, unsigned char *uDeltaPWM);

#endif