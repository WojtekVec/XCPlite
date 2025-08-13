/*----------------------------------------------------------------------------
|     Project: Vector CANape/vSignalyzer C-RT DLL Project
|
|        File: crt_function.h
|
| Description:
|
|-----------------------------------------------------------------------------
| $Rev:: 8276        $:  Revision of last commit
| $Author:: viswp    $:  Author of last commit
| $Date:: 2025-05-16#$:  Date of last commit
|-----------------------------------------------------------------------------
| Copyright (c) by Vector Informatik GmbH.  All rights reserved.
-----------------------------------------------------------------------------*/
#pragma once

#include <windows.h> //needed for BITMAPINFOHEADER

#include "crt.h"

#include <map>
#include <vector>
#include <string>

#include <cstdint>

// Start: external time source variables ==============================
extern "C" uint64_t gTimerTicks;
extern "C" double gExtTimeValue;
extern "C" double gExtTimeFactor;
extern "C" uint64_t gFixedStepTicks;

extern "C" double gStepSize = 0.1; // Fixed Step Size (Fundamental Sample Time): 0.1s (100ms)
// End: external time source variables ================================

extern sUserVar gUserInputVar[];
extern sUserVar gUserOutputVar[];
extern uint32_t gSizeUserInputVar;
extern uint32_t gSizeUserOutputVar;
extern const char* gModelName;

/**
 * Main C-RT class for algorithm implementation and input/output port specification.
 */
class VCrtFunction : public VCrtBase
{
public:
  VCrtFunction(VIAService * pService)
    : VCrtBase(pService)
  {
    // Constructor: all required resources are acquired here
  }

  virtual ~VCrtFunction()
  {
    // Destructor: all allocated resources are released here
  }

  // C-RT interface methods overrides
  virtual void InitMeasurement() override;
  virtual void InitData() override;
  virtual void StartMeasurement() override;
  virtual void Calculate() override;
  virtual void StopMeasurement() override;
  virtual void EndMeasurement() override;

private:
  void calcArray(const double);
};
