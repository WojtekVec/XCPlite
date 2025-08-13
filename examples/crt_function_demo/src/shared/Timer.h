/*----------------------------------------------------------------------------
|        File: Timer.cpp

|     Project: Vector MATLAB/Simulink MC Add-on CANape/vSignalyzer Target
|              and Vector CANape/vSignalyzer C-RT DLL Project
|
| Description:
|
|-----------------------------------------------------------------------------
| $Author: vislmk $    $Locker: $   $Revision: 5798 $
|-----------------------------------------------------------------------------
| Copyright (c) by Vector Informatik GmbH.  All rights reserved.
|---------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

namespace Utilities
{
  class Timer
  {
  public:
    Timer(bool start = false);
    ~Timer(void);
    void Start(void);
    void Stop(void);
    void Reset(void);
    uint64_t Elapsed(void);

    static Timer* GetInstance(void); // global access to local static instance

  private:
    uint64_t mStartTime;
    uint64_t mTicksPerSecond;
    uint64_t mElapsed;
    uint32_t a, b;
    bool mIsRunning;
  };
}
