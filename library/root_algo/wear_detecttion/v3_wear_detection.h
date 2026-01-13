/*****************************************************************************************************************
版权所有：Shenzhen Root Inonvation
项目名称：

项目编号：
文 件 名：
生成日期：2025.12.19
作    者：sahar.hu
功能说明：V3 wear detection functions
其他说明：
修改记录：
修改日期：
修 改 者：
修改内容：
******************************************************************************************************************/

#ifndef V3_WEAR_DETECTION_H
#define V3_WEAR_DETECTION_H
#ifdef __cplusplus
extern "C" {
#endif
// define modes
#define MODE_STIMULATION	(0)
#define MODE_EXPRESSION		(1)
#define MODE_MIXED			(2)

//define levels
#define LEVEL_1				(1)
#define LEVEL_2				(2)
#define LEVEL_3				(3)
#define LEVEL_4				(4)
#define LEVEL_5				(5)
#define LEVEL_6				(6)
#define LEVEL_7				(7)
#define LEVEL_8				(8)
#define LEVEL_9				(9)
#define LEVEL_10			(10)
#define LEVEL_11			(11)
#define LEVEL_12			(12)
#define LEVEL_13			(13)
#define LEVEL_14			(14)
#define LEVEL_15			(15)

// hardware parameters
#define SAMPLE_RATE			(50)
#define WIN_SIZE			(50*3)  //(in second)

// functions
void V3WearDetectionInit(unsigned int level, unsigned int mode);
unsigned int BufferCircleNextIndex(unsigned int uCurrPos, unsigned int uIndent, unsigned int uBufferLen);
unsigned int BufferCircleRelativeIndex(int uCurrPos, unsigned int uIdx, unsigned int uBufferLen, unsigned short uFirstTime);
float FilterPreprocess(float fInputData);
void PressureDataPreprocess(float fInputData);
void PeakValleyAnalysis();
void PressureDataFeature();
void WearDetectionDetermination();

void V3WearDetectionMain(float fInputData);
unsigned int GetWearDetectionResult();

#ifdef __cplusplus
}
#endif

#endif