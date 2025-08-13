/*----------------------------------------------------------------------------
|     Project: Vector CANape/vSignalyzer C-RT DLL Project
|
|        File: crt.h
|
| Description: Base C-RT function class.
|
|-----------------------------------------------------------------------------
| $Rev:: 7229        $:  Revision of last commit
| $Author:: viswp    $:  Author of last commit
| $Date:: 2023-12-08#$:  Date of last commit
|-----------------------------------------------------------------------------
| Copyright (c) by Vector Informatik GmbH.  All rights reserved.
-----------------------------------------------------------------------------*/
#pragma once

#include <windows.h> //needed for BITMAPINFOHEADER

#include "variable.h"

#include <map>
#include <string>

#include <cstdint>

extern sUserVar gUserInputVar[];
extern sUserVar gUserOutputVar[];
extern uint32_t gSizeUserInputVar;
extern uint32_t gSizeUserOutputVar;
extern double gStepSize;
extern const char* gModelName;

/**
 * Base C-RT function/model abstract class for algorithm implementation and input/output port specification.
 */
class VCrtBase
{
public:
  // C-RT function/model factory method: must be used to create an instance of the derived class
  static VCrtBase *CreateCrtFunction(VIAService *);

  VCrtBase(VIAService *);
  virtual ~VCrtBase();

  // abstract C-RT interface methods
  // must be overridden in inherited class
  virtual void InitMeasurement() = 0;
  virtual void InitData() = 0;
  virtual void StartMeasurement() = 0;
  virtual void Calculate() = 0;
  virtual void StopMeasurement() = 0;
  virtual void EndMeasurement() = 0;

  // Input/output variables for communication with CANape/vSignalyzer
  std::vector<VariableBase*> mInputVariables;
  std::vector<VariableBase*> mOutputVariables;

protected:
  // service methods
  void WriteString(const char* text);

  // VIA service
  VIAService * mpService;

  // Port variables for data exchange with CANape/vSignalyzer
  std::map<const std::string, VariableBase*> mPorts;

private:
  void CreateVariables();
  void DestroyVariables();
};

