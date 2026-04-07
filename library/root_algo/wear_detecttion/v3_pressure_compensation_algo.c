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
#include "v3_pressure_compensation_algo.h"
#define DEBUG		0

// global data
SDEVICE_INFO		tPumpDeviceInfo[2];
SDATA_BUFF			tPumpDataBuffer[2];
SPUMPFEATURE_INFO	tPumpFeatureInfo[2];
SGLOBAL_INFO		tGlobalInfo;
/*************************************************************************************************************************************************
* 函数名称：V3PressureCompensation Initialization
* 作    者：sahar.hu
* 创建日期：2026.03.26
* 功能说明：初始化
* 输入参数：无
* 输出参数：无
* 返 回 值：无
*************************************************************************************************************************************************
*/
void V3ProAlgoInit(aglo_air_pressure_data_t sPressureData)
{
	// 把sPressureData的数据拆解出来
	tGlobalInfo.currtimestamp = 0;
	tGlobalInfo.prevtimestamp = 0;
	tGlobalInfo.do_work = 0;

	for (unsigned int j = 0; j < CHANNEL_NUM; j++)
	{
		tPumpDeviceInfo[j].uMode = sPressureData.pump_state[j].mode;
		tPumpDeviceInfo[j].uLevel = sPressureData.pump_state[j].level - 1;
		if (tPumpDeviceInfo[j].uMode == MODE_EXPRESSION)
			tPumpDeviceInfo[j].run_time = EXPRESSION_WORK_TIME[tPumpDeviceInfo[j].uLevel];
		else if (tPumpDeviceInfo[j].uMode == MODE_STIMULATION)
			tPumpDeviceInfo[j].run_time = STIMULATION_WORK_TIME[tPumpDeviceInfo[j].uLevel];
		tPumpDeviceInfo[j].uTargetPressure = sPressureData.pump_state[j].target_pressure;
		tPumpFeatureInfo[j].remain_time = 0;
		tPumpFeatureInfo[j].uState = 0;
		tPumpFeatureInfo[j].uCompsStatus = 0;
		tPumpFeatureInfo[j].iDelta_pwm = 0;
		if (tPumpDeviceInfo[j].uMode == MODE_EXPRESSION)
		{
			tPumpFeatureInfo[j].fSlope = expression_K_slope[tPumpDeviceInfo[j].uLevel];
			tPumpFeatureInfo[j].fIntercept = expression_intercep[tPumpDeviceInfo[j].uLevel];
		}
	}
}

/*************************************************************************************************************************************************
* 函数名称：V3PressureCompensation Mode reset
* 作    者：sahar.hu
* 创建日期：2026.03.26
* 功能说明：重置
* 输入参数：无
* 输出参数：无
* 返 回 值：无
*************************************************************************************************************************************************
*/
void V3ProModeReset(algo_pump_state_t pump_state, unsigned char channel_idx)
{
	// 把sPressureData的数据拆解出来
	tPumpDeviceInfo[channel_idx].uMode = pump_state.mode;
	tPumpDeviceInfo[channel_idx].uLevel = pump_state.level - 1;
	if (tPumpDeviceInfo[channel_idx].uMode == MODE_EXPRESSION)
		tPumpDeviceInfo[channel_idx].run_time = EXPRESSION_WORK_TIME[tPumpDeviceInfo[channel_idx].uLevel];
	else if (tPumpDeviceInfo[channel_idx].uMode == MODE_STIMULATION)
		tPumpDeviceInfo[channel_idx].run_time = STIMULATION_WORK_TIME[tPumpDeviceInfo[channel_idx].uLevel];
	tPumpDeviceInfo[channel_idx].uTargetPressure = pump_state.target_pressure;
	tPumpFeatureInfo[channel_idx].pumping_time = 0;
	tPumpFeatureInfo[channel_idx].remain_time = 0;
	tPumpFeatureInfo[channel_idx].uState = 0;
	tPumpFeatureInfo[channel_idx].uCompsStatus = 0;
	if (tPumpDeviceInfo[channel_idx].uMode == MODE_EXPRESSION)
	{
		tPumpFeatureInfo[channel_idx].fSlope = expression_K_slope[tPumpDeviceInfo[channel_idx].uLevel];
		tPumpFeatureInfo[channel_idx].fIntercept = expression_intercep[tPumpDeviceInfo[channel_idx].uLevel];
	}
}

/*************************************************************************************************************************************************
* 函数名称：V3ProBufferCircleNextIndex -- buffer index
* 作    者：sahar.hu
* 创建日期：2026.03.26
* 功能说明：下一个索引号
* 输入参数：无
* 输出参数：无
* 返 回 值：无
*************************************************************************************************************************************************
*/
unsigned int V3ProBufferCircleNextIndex(unsigned int uPos, unsigned int uIndent, unsigned int uBufferLen)
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
* 函数名称：V3ProPressureDataPreprocess
* 作    者：sahar.hu
* 创建日期：2026.03.26
* 功能说明：to buffer
* 输入参数：无
* 输出参数：无
* 返 回 值：无
*************************************************************************************************************************************************
*/
void V3ProPressureDataPreprocess(aglo_air_pressure_data_t sPressureData, unsigned char uchannelIdx)
{
	// 更新时间戳
	tGlobalInfo.currtimestamp = sPressureData.timestamp;

	// 如果发生模式/档位变更，重置
	if (sPressureData.pump_state[uchannelIdx].level - 1 != tPumpDeviceInfo[uchannelIdx].uLevel ||
		sPressureData.pump_state[uchannelIdx].mode != tPumpDeviceInfo[uchannelIdx].uMode)
	{
		V3ProModeReset(sPressureData.pump_state[uchannelIdx], uchannelIdx);
	}
	tPumpDataBuffer[uchannelIdx].fDataBuff[tPumpDataBuffer[uchannelIdx].uCurrPos] = sPressureData.air_pressure[uchannelIdx];
	//tPumpFeatureInfo->remain_time[uchannelIdx] = sPressureData.timestamp;
}

/*************************************************************************************************************************************************
* 函数名称：PressureCompensationAnalysis
* 作    者：sahar.hu
* 创建日期：2026.03.26
* 功能说明：重置
* 输入参数：无
* 输出参数：无
* 返 回 值：无
*************************************************************************************************************************************************
*/
void PressureCompensationAnalysis(float fDataBuff, SPUMPFEATURE_INFO*tPumpFeatureInfoTemp, SDEVICE_INFO *tPumpDeviceInfoTemp)
{
	ALGO_LOG_INFO("run_time: %d, prev: %d, curr: %d", tPumpDeviceInfoTemp->run_time, tGlobalInfo.prevtimestamp, tGlobalInfo.currtimestamp);
	if (tPumpFeatureInfoTemp->uCompsStatus != 0)
	{
		tPumpFeatureInfoTemp->uCompsStatus == 0;
		tPumpFeatureInfoTemp->iDelta_pwm = 0;
	}
	// 判断当前是否接近结束泵工作ms级运算
	else if (tPumpDeviceInfoTemp->run_time - (tGlobalInfo.currtimestamp - tGlobalInfo.prevtimestamp) <  100 && tGlobalInfo.currtimestamp - tGlobalInfo.prevtimestamp > 0)	{
		// float fPredPressure = tPumpFeatureInfoTemp->fSlope * fDataBuff + tPumpFeatureInfoTemp->fIntercept;
		float fPredPressure = fDataBuff + 2;
#if DEBUG == 1
		//printf("%d, %f\n", tPumpFeatureInfoTemp->pumping_time, fPredPressure);
#endif
		// 预测值有问题时，不进行处理
		// float fPredPressure = fDataBuff + 2;
		// 当前已超过目标负压最大值，则直接反馈补偿
		// if ((fPredPressure + tPumpDeviceInfoTemp->uTargetPressure) < (tPumpDeviceInfoTemp->run_time - (tGlobalInfo.currtimestamp - tGlobalInfo.prevtimestamp))*.05)
		if (fDataBuff < -tPumpDeviceInfoTemp->uTargetPressure)
		{
			tPumpFeatureInfoTemp->uCompsStatus = 1;
			// 需要反馈iDelta_pwm (超出目标负压，反馈减小pwm）
			tPumpFeatureInfoTemp->iDelta_pwm = -100;		//  - (fPredPressure - tPumpDeviceInfoTemp->uTargetPressure) * 10
		}	
		//else if ((tPumpDeviceInfoTemp->uTargetPressure + fPredPressure) > (tPumpDeviceInfoTemp->run_time - (tGlobalInfo.currtimestamp - tGlobalInfo.prevtimestamp))*.1)
		else if (fDataBuff > -tPumpDeviceInfoTemp->uTargetPressure)
		{
			tPumpFeatureInfoTemp->uCompsStatus = 2;
			// 需要反馈iDelta_pwm (低于目标负压，反馈减小pwm）
			tPumpFeatureInfoTemp->iDelta_pwm = 100;			//  + (fPredPressure - tPumpDeviceInfoTemp->uTargetPressure) * 10
		}
		else
		{
			tPumpFeatureInfoTemp->uCompsStatus = 0;
			tPumpFeatureInfoTemp->iDelta_pwm = 0;
		}
	}
}

/*************************************************************************************************************************************************
* 函数名称：Pressure Compensation Main
* 作    者：sahar.hu
* 创建日期：2026.03.26
* 功能说明：重置
* 输入参数：无
* 输出参数：无
* 返 回 值：无
*************************************************************************************************************************************************
*/
void V3ProAlgoMain(aglo_air_pressure_data_t sPressureData)
{
#if DEBUG == 1
	// processing left and right data
	// for debug, process one channel
	unsigned int i = 0;
	if ((sPressureData.pump_state[i].state == SUCTION_STATUS_GENERATE) || (sPressureData.pump_state[i].state == SUCTION_STATUS_HOLD))	//  
	{
		tPumpFeatureInfo[i].pumping_time += 1;
		if (tPumpFeatureInfo[i].pumping_time == 1)
		{
			tGlobalInfo.prevtimestamp = sPressureData.timestamp;
		}
		V3ProPressureDataPreprocess(sPressureData, i);
		PressureCompensationAnalysis(tPumpDataBuffer[i].fDataBuff[tPumpDataBuffer[i].uCurrPos], &tPumpFeatureInfo[i], &tPumpDeviceInfo[i]);
		// update current index
		tPumpDataBuffer[i].uCurrPos = V3ProBufferCircleNextIndex(tPumpDataBuffer[i].uCurrPos, 1, WIN_SIZE);
	}
	else
	{
		// 如果不在泵奶期间，则不进行算法处理
		tPumpFeatureInfo[i].uCompsStatus = 0;
		tPumpFeatureInfo[i].iDelta_pwm = 0;
		tPumpFeatureInfo[i].pumping_time = 0;
	}

#else

	for (unsigned int i = 0; i < CHANNEL_NUM; i++)
	{
		if (((sPressureData.pump_state[i].state == SUCTION_STATUS_GENERATE) || (sPressureData.pump_state[i].state == SUCTION_STATUS_HOLD)))
		{
			tPumpFeatureInfo[i].pumping_time += 1;
			if (tPumpFeatureInfo[i].pumping_time == 1)
			{
				tGlobalInfo.prevtimestamp = sPressureData.timestamp;
			}
			V3ProPressureDataPreprocess(sPressureData, i);
			PressureCompensationAnalysis(tPumpDataBuffer[i].fDataBuff[tPumpDataBuffer[i].uCurrPos], &tPumpFeatureInfo[i], &tPumpDeviceInfo[i]);
			// update current index
			tPumpDataBuffer[i].uCurrPos = V3ProBufferCircleNextIndex(tPumpDataBuffer[i].uCurrPos, 1, WIN_SIZE);
		}
		else
		{
			// 如果不在泵奶期间，则不进行算法处理
			tPumpFeatureInfo[i].uCompsStatus = 0;
			tPumpFeatureInfo[i].iDelta_pwm = 0;
			tPumpFeatureInfo[i].pumping_time = 0;
		}
	}
#endif /* DEBUG == 1 */
}

/*************************************************************************************************************************************************
* 函数名称：GetCompensationStatus
* 作    者：sahar.hu
* 创建日期：2026.03.26
* 功能说明：重置
* 输入参数：无
* 输出参数：无
* 返 回 值：无
*************************************************************************************************************************************************
*/
void GetCompensationResult(unsigned char* uCompsStatus, int* iDeltaPWM)
{
	uCompsStatus[0] = tPumpFeatureInfo[0].uCompsStatus;
	uCompsStatus[1] = tPumpFeatureInfo[1].uCompsStatus;
	iDeltaPWM[0] = tPumpFeatureInfo[0].iDelta_pwm;
	iDeltaPWM[1] = tPumpFeatureInfo[1].iDelta_pwm;

}