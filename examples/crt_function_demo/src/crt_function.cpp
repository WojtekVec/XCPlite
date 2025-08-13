/*----------------------------------------------------------------------------
|     Project: Vector CANape/vSignalyzer C-RT DLL Project
|
|        File: crt_function.cpp
|
| Description: VCrtFunction implementation file.
|
|-----------------------------------------------------------------------------
| $Rev:: 8310        $:  Revision of last commit
| $Author:: viswp    $:  Author of last commit
| $Date:: 2025-07-16#$:  Date of last commit
|-----------------------------------------------------------------------------
| Copyright (c) by Vector Informatik GmbH.  All rights reserved.
 ----------------------------------------------------------------------------*/
#pragma warning( disable:4786)

#include "crt_function.h"
#include "crt_function_data.h"

#include "metainfo.h"

#define _USE_MATH_DEFINES

#include <math.h>

// TODO: define the name of the CANape/vSignalyzer function
const char* gModelName = "CRT_Function";

// TODO: define input ports
// Please note: all input port names must be unique
struct sUserVar gUserInputVar[] =
{
  //------------------------------------------------------------------|
  //| 1.                | 2.                | 3.      and     4.      |
  //| PortType          | Name of variable  | Dimensions of variable  |
  //| (use kNumeric     | (must be unique!) | Rows     | Columns      |
  //|  or kBinary or    |                   |          |              |
  //|  kImageProcessing)|                   |          |              |
  //------------------------------------------------------------------|
  //{kNumeric,           "input1",           1,         1},    // example of a scalar input port (variable)
  //{kNumeric,           "input2",           1,         3},    // example of a multi-dim input port (variable)
  //{kBinary,            "binInput",         16384,     BinaryVariable::kZeroSize } // example of binary data input port

  // inputs to be mapped with the example functions
  { kNumeric, "input1", 1, 1 },   // example of a scalar input port
  { kNumeric, "input2", 1, 1 },   // example of another scalar input port
};

uint32_t gSizeUserInputVar = sizeof(gUserInputVar) / sizeof(sUserVar);

// TODO: define output ports
// Please note: all output port names must be unique
struct sUserVar gUserOutputVar[] =
{
  //-----------------------------------------------------------------|
  //| 1.              | 2.                 | 3.      and     4.      |
  //| PortType        | Name of variable   | Dimensions of variable  |
  //| (see ePortType) | (must be unique!)  | Rows     | Columns      |
  //-----------------------------------------------------------------|
  //{kNumeric,         "output1",            1,          1},   // example of a scalar output port
  //{kNumeric,         "output2",            1,          3},   // example of an multi-dim output port
  //{kBinary,          "binOutput",          16384,      BinaryVariable::kZeroSize } // example of binary data output port

  // outputs to be mapped with the example functions
  { kNumeric, "output1", 1, 1 },
  { kNumeric, "output2", 1, 1 },
};

uint32_t gSizeUserOutputVar = sizeof(gUserOutputVar) / sizeof(sUserVar);

/**
 * C-RT factory method for the C-RT Function class.
 */
VCrtBase *VCrtBase::CreateCrtFunction(VIAService *pService)
{
  // return a new instance of the C-RT Function class
  return new VCrtFunction(pService);
}

/**
 * \brief InitMeasurement is called when the host application's e.g. CANape/vSignalyzer measurement is initialized.
 * 
 * This method can be used to initialize data and allocate the resources required for a single measurement run.
 *
 * See "doc\DLL_measurement_flow_chart.html" for more details.
 */
void VCrtFunction::InitMeasurement()
{  
  // initialize simulation time
  simTime = 0.0;
}

/**
 * \brief InitData is called when the CANape/vSignalyzer measurement is initialized after the InitMeasurement method
 *
 * With this method, port descriptions can be specified.
 */
void VCrtFunction::InitData()
{
  for (uint16_t x = 0; x < Const::DimX; ++x)
  {
    for (uint16_t y = 0; y < Const::DimY; ++y)
    {
      // set current element to 0.0
      arrayParam[x][y] = 0.0;
    }
  }
  // initialze the internal array measurement object
  const double phaseShift = 0.0;
  calcArray(phaseShift);
}

/**
 * \brief StartMeasurement is called when the CANape/vSignalyzer measurement is started.
 * 
 * This method can be used to initialize variable values that are required for a single measurement run.
 */
void VCrtFunction::StartMeasurement()
{
  // Write the following string to CANape/vSignalyzer (host) Write-Window.
  WriteString("VCrtFunction START Measurement");

  // initialize parameter values
  fcnCounter1 = 0;

  // initialize input port data values
  inputData.input1 = 0.0;
  inputData.input2 = 0.0;

  // initialize output port data values
  outputData.output1 = 0.0;
  outputData.output2 = 0.0;
}

/**
 * \brief Calculates the internal array measurement object.
 */
void VCrtFunction::calcArray(const double phaseShift)
{
  const double phaseStepX = 1.0 / static_cast<double>(Const::DimX - 1);
  const double phaseStepY = 1.0 / static_cast<double>(Const::DimY);
  double phaseX = 0.0;
  double phaseY = 0.0;

  for (uint16_t x = 0; x < Const::DimX; ++x)
  {
    for (uint16_t y = 0; y < Const::DimY; ++y)
    {
      arrayObj[x][y] = sin(phaseX * M_PI) * sin(phaseShift + 2.0 * phaseY * M_PI);
      // update Y phase
      phaseY += phaseStepY;
    }
    // update X phase
    phaseX += phaseStepX;
  }
}

/**
 * \brief The Calculate method is called each time CANape/vSignalyzer triggers the function to calculate a new step.
 */
void VCrtFunction::Calculate()
{
  //////
  // User defined function example
  ///////////////
  // perform input port value mapping
  inputData.input1 = mPorts["input1"]->GetValue();
  inputData.input2 = mPorts["input2"]->GetValue();

  // process internal data
  fcnCounter1 = fcnCounter1 + 1;
  fcnCounter2 = fcnCounter2 + 1;

  // perform output port value mapping
  mPorts["output1"]->SetValue(outputData.output1);
  mPorts["output2"]->SetValue(outputData.output2);

  // calculate the internal array measurement object
  calcArray(simTime);

  // move to next time step, increase simulation time
  simTime += gStepSize;
}

/**
 * \brief StopMeasurement is called when the CANape/vSignalyzer measurement is stopped.
 */
void VCrtFunction::StopMeasurement()
{
  // Write the following string to CANape (host) Write-Window.
  WriteString("VCrtFunction STOP Measurement");
}

/**
 * \brief EndMeasurement is called when the CANape/vSignalyzer measurement is terminated.
 *
 * This method can be used to release resources previously allocated in the InitMeasurement method that are no longer used.
 */
void VCrtFunction::EndMeasurement()
{

}
