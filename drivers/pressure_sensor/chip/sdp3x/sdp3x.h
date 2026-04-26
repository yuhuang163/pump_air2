#ifndef __SDP3X_H__
#define __SDP3X_H__

#ifdef __cplusplus
extern "C" {
#endif


/*******************************************************************************
* Include files
******************************************************************************/
#include "bsp_common.h" // IWYU pragma: keep


// Sensor Commands
typedef enum {
  /// Undefined dummy command.
  COMMAND_UNDEFINED                       = 0x0000,
  /// Start continous measurement                     \n
  /// Temperature compensation: Mass flow             \n
  /// Averaging: Average till read
  COMMAND_START_MEASURMENT_MF_AVERAGE     = 0x3603,
  /// Start continous measurement                     \n
  /// Temperature compensation: Mass flow             \n
  /// Averaging: None - Update rate 1ms
  COMMAND_START_MEASURMENT_MF_NONE        = 0x3608,
  /// Start continous measurement                     \n
  /// Temperature compensation: Differential pressure \n
  /// Averaging: Average till read
  COMMAND_START_MEASURMENT_DP_AVERAGE     = 0x3615,
  /// Start continous measurement                     \n
  /// Temperature compensation: Differential pressure \n
  /// Averaging: None - Update rate 1ms
  COMMAND_START_MEASURMENT_DP_NONE        = 0x361E,
  // Stop continuous measurement.
  COMMAND_STOP_CONTINOUS_MEASUREMENT      = 0x3FF9
} Command;


//-----------------------------------------------------------------------------
/// \brief Enumeration to configure the temperature compensation for
///        measurement.
typedef enum {
  SDP3X_TEMPCOMP_MASS_FLOW,
  SDP3X_TEMPCOMP_DIFFERNTIAL_PRESSURE
} Sdp3xTempComp;
  
//-----------------------------------------------------------------------------
/// \brief Enumeration to configure the averaging for measurement.
typedef enum {
  SDP3X_AVERAGING_NONE,
  SDP3X_AVERAGING_TILL_READ
} Sdp3xAveraging;

//-----------------------------------------------------------------------------
/// \brief  Initializes the I2C bus for communication with the sensor.
/// \param  i2cAddress  Sensors I2C address.
void Sdp3x_Init(uint8_t i2cAddress);

//-----------------------------------------------------------------------------
/// \brief  Starts the continous mesurement with the specified settings.
/// \param  tempComp  Temperature compensation: Mass flow or diff. pressure.
/// \param  averaging Averaging: None or average till read.
/// \return ERROR_NONE              = No error.
///         ERROR_ACK               = No acknowledgment from sensor.
///         ERROR_INVALID_PARAMETER = At least one of the specified parameter
///                                   are invalid.
int Sdp3x_StartContinousMeasurement(Sdp3xTempComp  tempComp,
                                      Sdp3xAveraging averaging);

//-----------------------------------------------------------------------------
/// \brief  Stops the continous mesurement.
/// \return ERROR_NONE              = No error.
///         ERROR_ACK               = No acknowledgment from sensor.
int Sdp3x_StopContinousMeasurement(void);

//-----------------------------------------------------------------------------
/// \brief  Reads the measurment result from the continous measurment.
/// \param  diffPressure Pointer to return the measured diverential pressur.
///         temperature  Pointer to return the measured temperature.
/// \return ERROR_NONE              = No error.
///         ERROR_ACK               = No acknowledgment from sensor.
///         ERROR_CHECKSUM          = Checksum does not match.
int Sdp3x_ReadMeasurementResults(float* diffPressure, float* temperature);

//-----------------------------------------------------------------------------
/// \brief  Calls the soft reset mechanism that forces the sensor into a
///         well-defined state without removing the power supply.
/// \return ERROR_NONE              = No error.
///         ERROR_ACK               = No acknowledgment from sensor.
int Sdp3x_SoftReset(void);
  


/*
* GLOBAL FUNCTION DECLARATION
*****************************************************************************************
*/

#ifdef __cplusplus
}
#endif

#endif 
