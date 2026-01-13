/*****************************************************************************************************************
版权所有：Shenzhen Root Inonvation
项目名称：V3
项目编号：
文 件 名：
生成日期：2026.01.07
作    者：sahar.hu
功能说明：V3 wear detection functions
其他说明：this version is only for stimulation-level 4. 
修改记录：
修改日期：
修 改 者：
修改内容：
******************************************************************************************************************/
#include "v3_wear_detection.h"
#include <stdlib.h>


// structures

typedef struct _DEVICE_INFO
{
	unsigned short	uMode;
	unsigned short	uLevel;
}SDEVICE_INFO;

typedef struct _PV_INFO
{
	unsigned short	uType;
	unsigned int	uPos;
	float			fValue;
}SPV_INFO;

typedef struct _WAVE_INFO
{
	unsigned short	uType;
	unsigned int	uPos;
	float			fValue;
	float			fLeftAmp; 
	float			fRightAmp;
	unsigned int	uwidth;
}SWAVE_INFO;


typedef struct _FEATURE_INFO_
{
	unsigned int	uPVIndex; 
	unsigned int	uWaveSpikes;
	float			fMinData;
	unsigned int	uNoiseLevel;
	unsigned int	uDataIdx;		// data index
	unsigned short	uUpdateFlag;
	float			fThreshold;
	unsigned short	uWearStatus;	// result
	unsigned short	uPrevWearStatus;
	unsigned int	uNormalWaveform;
}SFEATURE_INFO;

typedef struct _DATA_BUFFER_
{
	float			fDataBuff[WIN_SIZE];
	float			fDataMax;
	float			fDataMin;
	unsigned int	uCurrPos;		// current pos
	unsigned short  uFirstTime;
}SDATA_BUFFER;


typedef struct _FILTER_INFO_
{
	float			fFc;
	float			fDT;
	float			fRC;
	float			fAlpha;
	float			fYpre;
	unsigned short	uInitState;
}SFILTER_INFO;


// global data
SDATA_BUFFER		tDataBuffer;
SPV_INFO			tPVInfo[100];
SWAVE_INFO			tWaveInfo[100];
SFEATURE_INFO		tFeatureInfo;
SDEVICE_INFO		tDeviceInfo;
SFILTER_INFO		tFilterInfo;
// Thresholds for each mode and level
float EXPRESSION_THRESHOLD[] = {-12.06, -12.465, -13.655, -15.77, -16.815, -18.39, -22.165, -26.885, -32.46, -36.585, -39.84, -42.305, -44.645, -46.74, -48.83};
float STIMULATION_THRESHOLD[] = { -3, -4, -4, -8, -10, -5, -5, -5, -6, -10, -15, -16, -18, -20, -23 };
float MIXED_THRESHOLD[] = {-12.01, -12.45, -13.785, -15.75, -17.04, -18.375, -22.52, -27.19, -32.69,-36.795, -40.145, -42.725, -45.32, -47.04, -48.93};

/*************************************************************************************************************************************************
* 函数名称：V3WearDetectionInit
* 作    者：sahar.hu
* 创建日期：2025.12.19
* 功能说明：初始化
* 输入参数：无
* 输出参数：无
* 返 回 值：无
*************************************************************************************************************************************************
*/
void V3WearDetectionInit(unsigned int level, unsigned int mode)
{
	
	for (int i = 0; i < 100; i++)
	{
		// PVInfo initiation
		tPVInfo[i].uPos			= 0;
		tPVInfo[i].fValue		= 0;
		tPVInfo[i].uType		= 0;
		// WaveInfo initiation
		tWaveInfo[i].fLeftAmp	= 0;
		tWaveInfo[i].fRightAmp	= 0;
		tWaveInfo[i].fValue		= 0;
		tWaveInfo[i].uPos		= 0;
		tWaveInfo[i].uType		= 0;
		tWaveInfo[i].uwidth		= 0;

	}
	// data buffer
	for (int i = 0; i < WIN_SIZE; i++)
	{
		tDataBuffer.fDataBuff[i]= 0;
		tDataBuffer.uCurrPos	= 0;
	}
	tDataBuffer.fDataMax = -1;
	tDataBuffer.fDataMin = 0;
	tDataBuffer.uFirstTime = 1;

	// features
	tFeatureInfo.uUpdateFlag	= 0;
	tFeatureInfo.fMinData		= 0;
	tFeatureInfo.uNoiseLevel	= 0;
	tFeatureInfo.uWaveSpikes	= 0;
	tFeatureInfo.uPVIndex		= 0;
	tFeatureInfo.uWearStatus	= 0;
	tFeatureInfo.uNormalWaveform = 0;
	// Device info
	tDeviceInfo.uMode = mode;
	tDeviceInfo.uLevel = level;
	switch (mode)
	{
		case MODE_EXPRESSION:
			tFeatureInfo.fThreshold = EXPRESSION_THRESHOLD[level-1];
		case MODE_MIXED:
			tFeatureInfo.fThreshold = MIXED_THRESHOLD[level-1];
		case MODE_STIMULATION:
			tFeatureInfo.fThreshold = STIMULATION_THRESHOLD[level-1];
	}
	
	// filter info
	tFilterInfo.fFc = 3.0;
	tFilterInfo.fDT = 1.0/ SAMPLE_RATE;
	tFilterInfo.fRC = 1.0/(2.0*3.1415926*tFilterInfo.fFc);
	tFilterInfo.fAlpha = tFilterInfo.fDT / (tFilterInfo.fRC + tFilterInfo.fDT);
	tFilterInfo.fYpre = 0.0;
	tFilterInfo.uInitState = 0;
}


/*************************************************************************************************************************************************
* 函数名称：CircleIndex
* 作    者：sahar.hu
* 创建日期：2025.12.19
* 功能说明：数据当前定位
* 输入参数：无
* 输出参数：无
* 返 回 值：无
*************************************************************************************************************************************************
*/
unsigned int BufferCircleNextIndex(unsigned int uPos, unsigned int uIndent, unsigned int uBufferLen)
{
	unsigned int uTempPos = 0;
	if (uPos == uBufferLen - uIndent)
	{
		uTempPos = 0;
	}
	else if (uPos + uIndent >= uBufferLen)
		uTempPos = uPos + uIndent - uBufferLen;
	else
		uTempPos = uPos + uIndent;
	return uTempPos;
}


/*************************************************************************************************************************************************
* 函数名称：BufferCircleRelativeIndex
* 作    者：sahar.hu
* 创建日期：2025.12.19
* 功能说明：数据当前定位
* 输入参数：无
* 输出参数：无
* 返 回 值：无
*************************************************************************************************************************************************
*/
unsigned int BufferCircleRelativeIndexrCircle(unsigned int uPos, unsigned int uIdx, unsigned int uBufferLen, unsigned int uFirstTime)
{
	unsigned int uTempPos = 0;
	if (uFirstTime == 1)
	{
		unsigned int uStartPos = 0;
		uTempPos = uIdx;
	}
	else
	{
		unsigned int uStartPos = uPos + 1;
		uTempPos = BufferCircleNextIndex(uStartPos, uIdx, uBufferLen);
	}

	return uTempPos;
}


/*************************************************************************************************************************************************
* 函数名称：V3WearDetectionMain
* 作    者：sahar.hu
* 创建日期：2025.12.19
* 功能说明：佩戴检测主函数
* 输入参数：无
* 输出参数：无
* 返 回 值：无
*************************************************************************************************************************************************
*/
float FilterPreprocess(float fInputData)
{
	// low pass filter
	float filtered_data = 0.0;
	if (tFilterInfo.uInitState == 0)
	{
		tFilterInfo.fYpre = fInputData;
		tFilterInfo.uInitState = 1;
		return fInputData;
	}
	filtered_data = tFilterInfo.fYpre + tFilterInfo.fAlpha * (fInputData - tFilterInfo.fYpre);
	tFilterInfo.fYpre = filtered_data;
	return filtered_data;
}

/*************************************************************************************************************************************************
* 函数名称：PressureDataPreprocess
* 作    者：sahar.hu
* 创建日期：2025.12.19
* 功能说明：数据缓存处理, 滤波
* 输入参数：无
* 输出参数：无
* 返 回 值：无
*************************************************************************************************************************************************
*/
void PressureDataPreprocess(float fInputData)
{
	float filtered_data = 0.0;
	filtered_data = FilterPreprocess(fInputData);
	tDataBuffer.fDataBuff[tDataBuffer.uCurrPos] = filtered_data;
	if (tFeatureInfo.uDataIdx == SAMPLE_RATE-1)
	{
		tDataBuffer.fDataMax = -1;
		tDataBuffer.fDataMin = 0;

		tFeatureInfo.fMinData = 0;
		tFeatureInfo.uNoiseLevel = 0;
		tFeatureInfo.uWaveSpikes = 0;
		tFeatureInfo.uDataIdx = 0;
		tFeatureInfo.uUpdateFlag = 1;

		tFeatureInfo.uNormalWaveform = 0;
	}
	else
	{
		tFeatureInfo.uDataIdx ++;
		tFeatureInfo.uUpdateFlag = 0;	
	}

}

/*************************************************************************************************************************************************
* 函数名称：PeakValleyAnalysis
* 作    者：sahar.hu
* 创建日期：2025.12.19
* 功能说明：波峰、波谷检测
* 输入参数：无
* 输出参数：无
* 返 回 值：无
*************************************************************************************************************************************************
*/
void PeakValleyAnalysis()
{ 
	if (tFeatureInfo.uUpdateFlag == 1)
	{
		for (int j = 0; j < 100; j++)
		{
			tPVInfo[j].fValue = 0;
			tPVInfo[j].uPos = 0;
			tPVInfo[j].uType = 0;
			tWaveInfo[j].fLeftAmp = 0;
			tWaveInfo[j].fRightAmp = 0;
			tWaveInfo[j].fValue = 0;
			tWaveInfo[j].uPos = 0;
			tWaveInfo[j].uType = 0;
			tWaveInfo[j].uwidth = 0;

		}
		tFeatureInfo.uPVIndex = 0;
		if (tDataBuffer.fDataMax < tDataBuffer.fDataBuff[0])
		{
			tDataBuffer.fDataMax = tDataBuffer.fDataBuff[0];
		}
		if (tDataBuffer.fDataMin > tDataBuffer.fDataBuff[0])
		{
			tDataBuffer.fDataMin = tDataBuffer.fDataBuff[0];
		}
		for (int i = 1; i < WIN_SIZE - 1; i++)
		{
			if (tDataBuffer.fDataMax < tDataBuffer.fDataBuff[i])
			{
				tDataBuffer.fDataMax = tDataBuffer.fDataBuff[i];
			}
			if (tDataBuffer.fDataMin > tDataBuffer.fDataBuff[i])
			{
				tDataBuffer.fDataMin = tDataBuffer.fDataBuff[i];
			}
			unsigned int uTempCurrPos = BufferCircleRelativeIndexrCircle(tDataBuffer.uCurrPos, i, WIN_SIZE, tDataBuffer.uFirstTime);
			unsigned int uTempNextPos = BufferCircleRelativeIndexrCircle(tDataBuffer.uCurrPos, i + 1, WIN_SIZE, tDataBuffer.uFirstTime);
			unsigned int uTempPrevPos = BufferCircleRelativeIndexrCircle(tDataBuffer.uCurrPos, i - 1, WIN_SIZE, tDataBuffer.uFirstTime);

			//printf("i: %d, currPos: %d, prev: %d, curr: %d, next: %d\n", i, tDataBuffer.uCurrPos, uTempPrevPos, uTempCurrPos, uTempNextPos);
			//printf("i: %d: %f\n", i, tDataBuffer.fDataBuff[uTempCurrPos]);
		
			// peak
			if ((tDataBuffer.fDataBuff[uTempPrevPos] <= tDataBuffer.fDataBuff[uTempCurrPos] && tDataBuffer.fDataBuff[uTempCurrPos] > tDataBuffer.fDataBuff[uTempNextPos]) || 
			(tDataBuffer.fDataBuff[uTempPrevPos] < tDataBuffer.fDataBuff[uTempCurrPos] && tDataBuffer.fDataBuff[uTempCurrPos] >= tDataBuffer.fDataBuff[uTempNextPos]))
			{
				// combine with the previous PV if it's flat
				if (tFeatureInfo.uPVIndex >= 1)
				{
					if (tPVInfo[tFeatureInfo.uPVIndex - 1].fValue == tDataBuffer.fDataBuff[uTempCurrPos] && tPVInfo[tFeatureInfo.uPVIndex - 1].uType == 1)
					{
						tFeatureInfo.uPVIndex -= 1;
					}
				}
				// update PVInfo
				tPVInfo[tFeatureInfo.uPVIndex].uPos = i;
				tPVInfo[tFeatureInfo.uPVIndex].fValue = tDataBuffer.fDataBuff[uTempCurrPos];
				tPVInfo[tFeatureInfo.uPVIndex].uType = 1;
				tFeatureInfo.uPVIndex ++;
			}

			// valley
			if ((tDataBuffer.fDataBuff[uTempPrevPos] >= tDataBuffer.fDataBuff[uTempCurrPos] && tDataBuffer.fDataBuff[uTempCurrPos] < tDataBuffer.fDataBuff[uTempNextPos]) ||
				(tDataBuffer.fDataBuff[uTempPrevPos] > tDataBuffer.fDataBuff[uTempCurrPos] && tDataBuffer.fDataBuff[uTempCurrPos] <= tDataBuffer.fDataBuff[uTempNextPos]))
			{
				// combine with the previous PV if it's flat
				if (tFeatureInfo.uPVIndex >= 1)
				{
					if (tPVInfo[tFeatureInfo.uPVIndex - 1].fValue == tDataBuffer.fDataBuff[uTempCurrPos] && tPVInfo[tFeatureInfo.uPVIndex - 1].uType == 0)
					{
						tFeatureInfo.uPVIndex -= 1;
					}
				}
				// update PVInfo
				tPVInfo[tFeatureInfo.uPVIndex].uPos = i;
				tPVInfo[tFeatureInfo.uPVIndex].fValue = tDataBuffer.fDataBuff[uTempCurrPos];
				tPVInfo[tFeatureInfo.uPVIndex].uType = 0;
				tFeatureInfo.uPVIndex++;
			}
		}
		// update the buffer status
		if (tDataBuffer.uFirstTime == 1 && tDataBuffer.uCurrPos == 89)
		{
			tDataBuffer.uFirstTime = 0;
		}
		// update the max and min data
		if ((tDataBuffer.fDataMax - tDataBuffer.fDataMin) < 5)
			tFeatureInfo.uNoiseLevel += 1;
	}
}
/*************************************************************************************************************************************************
* 函数名称：PressureDataFeature
* 作    者：sahar.hu
* 创建日期：2025.12.19
* 功能说明：波形特征分析
* 输入参数：无
* 输出参数：无
* 返 回 值：无
*************************************************************************************************************************************************
*/
void PressureDataFeature()
{
	// peak and valley
	PeakValleyAnalysis();
	// wave analysis
	unsigned int uWaveIndex = 0;
	if (tFeatureInfo.uUpdateFlag == 1) 
	{
		if (tFeatureInfo.uPVIndex >= 2)
		{
			// check the peak/valley starting at the first PV
			unsigned int i = 0;
			if (tPVInfo[i].uType == 0 && tPVInfo[i + 1].uType == 1)
			{
				tWaveInfo[uWaveIndex].uPos = tPVInfo[i].uPos;
				tWaveInfo[uWaveIndex].fValue = tPVInfo[i].fValue;
				tWaveInfo[uWaveIndex].uwidth = tPVInfo[i+1].uPos;		// it starts at "0"
				tWaveInfo[uWaveIndex].uType = 0;
				tWaveInfo[uWaveIndex].fRightAmp = tPVInfo[i + 1].fValue - tPVInfo[i].fValue;
				tWaveInfo[uWaveIndex].fLeftAmp = fabs(tPVInfo[i].fValue);
				if (tFeatureInfo.fMinData > tWaveInfo[uWaveIndex].fValue)
				{
					tFeatureInfo.fMinData = tWaveInfo[uWaveIndex].fValue;
				}
				uWaveIndex += 1;
			}
			else if (tPVInfo[i].uType == 1 && tPVInfo[i + 1].uType == 0)
			{
				tWaveInfo[uWaveIndex].uPos = tPVInfo[i].uPos;
				tWaveInfo[uWaveIndex].fValue = tPVInfo[i].fValue;
				tWaveInfo[uWaveIndex].uwidth = tPVInfo[i + 1].uPos;		// it starts at "0"
				tWaveInfo[uWaveIndex].uType = 1;
				tWaveInfo[uWaveIndex].fRightAmp = tPVInfo[i].fValue - tPVInfo[i + 1].fValue;
				tWaveInfo[uWaveIndex].fLeftAmp = fabs(tPVInfo[i].fValue);
				uWaveIndex += 1;
			}
			if (tFeatureInfo.uPVIndex >= 3)
			{
				for (i = 1; i < tFeatureInfo.uPVIndex - 1; i++)
				{
					// check a valley wave
					if (tPVInfo[i - 1].uType == 1 && tPVInfo[i].uType == 0 && tPVInfo[i + 1].uType == 1)
					{
						tWaveInfo[uWaveIndex].uPos = tPVInfo[i].uPos;
						tWaveInfo[uWaveIndex].fValue = tPVInfo[i].fValue;
						tWaveInfo[uWaveIndex].uwidth = tPVInfo[i + 1].uPos - tPVInfo[i - 1].uPos;
						tWaveInfo[uWaveIndex].uType = 0;
						tWaveInfo[uWaveIndex].fRightAmp = tPVInfo[i + 1].fValue - tPVInfo[i].fValue;
						tWaveInfo[uWaveIndex].fLeftAmp = tPVInfo[i - 1].fValue - tPVInfo[i].fValue;
						if (tFeatureInfo.fMinData > tWaveInfo[uWaveIndex].fValue)
						{
							tFeatureInfo.fMinData = tWaveInfo[uWaveIndex].fValue;
						}
						uWaveIndex += 1;
					}
					// check a peak wave
					if (tPVInfo[i - 1].uType == 0 && tPVInfo[i].uType == 1 && tPVInfo[i + 1].uType == 0)
					{
						tWaveInfo[uWaveIndex].uPos = tPVInfo[i].uPos;
						tWaveInfo[uWaveIndex].fValue = tPVInfo[i].fValue;
						tWaveInfo[uWaveIndex].uwidth = tPVInfo[i + 1].uPos - tPVInfo[i - 1].uPos;
						tWaveInfo[uWaveIndex].uType = 1;
						tWaveInfo[uWaveIndex].fRightAmp = tPVInfo[i].fValue - tPVInfo[i + 1].fValue;
						tWaveInfo[uWaveIndex].fLeftAmp = tPVInfo[i].fValue - tPVInfo[i - 1].fValue;
						uWaveIndex += 1;
					}
				}
			}
		}
		// check wave spikes
		for (unsigned int i = 0; i < uWaveIndex; i++)
		{
			if (tWaveInfo[i].uType == 1 && fabs(tWaveInfo[i].fLeftAmp - tWaveInfo[i].fRightAmp) > 2 && fabs(tWaveInfo[i].fLeftAmp) > .1 && fabs(tWaveInfo[i].fRightAmp) > .1 && 
			( (2 <= tWaveInfo[i].uwidth && tWaveInfo[i].uwidth <= 15 && tFeatureInfo.fThreshold < tWaveInfo[i].fValue && tWaveInfo[i].fValue < -.1) || 
			(15 <= tWaveInfo[i].uwidth && tWaveInfo[i].uwidth <= 50 && tWaveInfo[i].fValue < -.1)))
			{
				tFeatureInfo.uWaveSpikes ++;
			}
			else if (tWaveInfo[i].uType == 0 && 
			(tWaveInfo[i].fValue < tFeatureInfo.fThreshold && tDeviceInfo.uMode == MODE_STIMULATION || (tDeviceInfo.uMode == MODE_EXPRESSION && tDeviceInfo.uLevel <= 7)) || 
				(tWaveInfo[i].fValue < -1 && abs(tFeatureInfo.fThreshold - tWaveInfo[i].fValue) < 10 && (tDeviceInfo.uMode == MODE_EXPRESSION && tDeviceInfo.uLevel > 7 && tDeviceInfo.uLevel < 10)) ||
				(tWaveInfo[i].fValue < -1 && abs(tFeatureInfo.fThreshold - tWaveInfo[i].fValue) > 2 && (tDeviceInfo.uMode == MODE_EXPRESSION && tDeviceInfo.uLevel >=11))
				)
			{
				tFeatureInfo.uNormalWaveform ++;
			}

		}
	}
	//printf("%d \n", tFeatureInfo.uWaveSpikes);
}

/*************************************************************************************************************************************************
* 函数名称：WearDetectionDetermination
* 作    者：sahar.hu
* 创建日期：2025.12.19
* 功能说明：佩戴检测
* 输入参数：无
* 输出参数：无
* 返 回 值：无
*************************************************************************************************************************************************
*/
void WearDetectionDetermination()
{
	tFeatureInfo.uWearStatus = tFeatureInfo.uPrevWearStatus;
	if (tFeatureInfo.uWearStatus == 0)
	{
		//
		if (tFeatureInfo.uWaveSpikes <= 1 && tFeatureInfo.uNoiseLevel == 0 && tFeatureInfo.uNormalWaveform >=1)
		{
			tFeatureInfo.uWearStatus = 1;
		}
	}
	else if (tFeatureInfo.uWearStatus == 1)
	{
		if (((tFeatureInfo.fThreshold > tFeatureInfo.fMinData && tDeviceInfo.uLevel > LEVEL_12 && tDeviceInfo.uMode == MODE_EXPRESSION)
			|| (tFeatureInfo.fThreshold < tFeatureInfo.fMinData && tFeatureInfo.fMinData < -.1 && tDeviceInfo.uMode == MODE_STIMULATION)
			|| (STIMULATION_THRESHOLD[tDeviceInfo.uLevel] < tFeatureInfo.fMinData && tFeatureInfo.fMinData < -.1 && tDeviceInfo.uMode == MODE_MIXED && tDeviceInfo.uLevel <= 7)) ||
			(tFeatureInfo.fThreshold > tFeatureInfo.fMinData && tDeviceInfo.uLevel > 12 && tDeviceInfo.uMode == MODE_EXPRESSION))
		{
			tFeatureInfo.uWearStatus = 0;		// off
		}
		else if (tFeatureInfo.fMinData > -.1 || \
			(tFeatureInfo.fThreshold > tFeatureInfo.fMinData && tDeviceInfo.uLevel > 12 && tDeviceInfo.uMode == MODE_EXPRESSION || tDeviceInfo.uMode == MODE_MIXED))
		{
			tFeatureInfo.uWearStatus = tFeatureInfo.uPrevWearStatus;
		}
	}
	
	
}


/*************************************************************************************************************************************************
* 函数名称：V3WearDetectionMain
* 作    者：sahar.hu
* 创建日期：2025.12.19
* 功能说明：佩戴检测主函数
* 输入参数：无
* 输出参数：无
* 返 回 值：无
*************************************************************************************************************************************************
*/
void V3WearDetectionMain(float fInputData)
{
	PressureDataPreprocess(fInputData);
	PressureDataFeature();
	WearDetectionDetermination();
	// update current index 
	tDataBuffer.uCurrPos = BufferCircleNextIndex(tDataBuffer.uCurrPos, 1, WIN_SIZE);
}


unsigned int GetWearDetectionResult()
{
	return tFeatureInfo.uWearStatus;
}