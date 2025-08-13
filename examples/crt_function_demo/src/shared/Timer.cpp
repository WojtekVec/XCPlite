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

#include "Timer.h"
#include <windows.h>

using namespace Utilities;

/**
 * Creates an object of thie Timer class.
 */
Timer::Timer(bool start)
  : mIsRunning(false)
  , mTicksPerSecond(0)
  , mStartTime(0)
  , mElapsed(0)
  , a(1000000000)
  , b(1)
{
  LARGE_INTEGER ticksPerSecond;

  // Set factors a and b dependent on how fast the performance counter is running.
  // This lengthy code takes care of all possibilities that may occur within 64-bit limits.
  if (::QueryPerformanceFrequency(&ticksPerSecond))
  {
    if (ticksPerSecond.QuadPart < 10000000000ull /* 10^10 */)
    {
      a = 1000000000; b = 1;
    }
    else if (ticksPerSecond.QuadPart < 100000000000ull /* 10^11 */)
    {
      a = 100000000; b = 10;
    }
    else if (ticksPerSecond.QuadPart < 1000000000000ull /* 10^12 */)
    {
      a = 10000000; b = 100;
    }
    else if (ticksPerSecond.QuadPart < 10000000000000ull /* 10^13 */)
    {
      a = 1000000; b = 1000;
    }
    else if (ticksPerSecond.QuadPart < 100000000000000ull /* 10^14 */)
    {
      a = 100000; b = 10000;
    }
    else if (ticksPerSecond.QuadPart < 1000000000000000ull /* 10^15 */)
    {
      a = 10000; b = 100000;
    }
    else if (ticksPerSecond.QuadPart < 10000000000000000ull /* 10^16 */)
    {
      a = 1000; b = 1000000;
    }
    else if (ticksPerSecond.QuadPart < 100000000000000000ull /* 10^17 */)
    {
      a = 100; b = 10000000;
    }
    else if (ticksPerSecond.QuadPart < 1000000000000000000ull /* 10^18 */)
    {
      a = 10; b = 100000000;
    }
    else if (ticksPerSecond.QuadPart < 10000000000000000000ull /* 10^19 */)
    {
      a = 1; b = 1000000000;
    }
    mTicksPerSecond = ticksPerSecond.QuadPart;
  }

  if (start)
  {
    Start();
  }
}

Timer::~Timer(void)
{
}

/**
 * Starts the timer.
 */
void Timer::Start(void)
{
  Reset();
  mIsRunning = true;
}

/**
 * Stops the timer.
 */
void Timer::Stop(void)
{
  if (!mIsRunning)
  {
    return;
  }

  LARGE_INTEGER elapsed;

  ::QueryPerformanceCounter(&elapsed);
  mElapsed = elapsed.QuadPart;

  mIsRunning = false;
}

/**
 * Resets this timer by setting both start/elapsed time to current time.
 */
void Timer::Reset(void)
{
  LARGE_INTEGER startTime;
  ::QueryPerformanceCounter(&startTime);
  mStartTime = startTime.QuadPart;

  if (!mIsRunning)
  {
    mElapsed = mStartTime;
  }
}

/**
 * Returns the elapsed time since last call.
 */
uint64_t Timer::Elapsed(void)
{
  uint64_t seconds, fractionalTicks, elapsedTicks;

  if (!mIsRunning)
  {
    elapsedTicks = mElapsed - mStartTime;
  }
  else
  {
    LARGE_INTEGER elapsed;
    ::QueryPerformanceCounter(&elapsed);
    elapsedTicks = elapsed.QuadPart - mStartTime;
  }

  // split time into full seconds and fractions of a second (see below)
  seconds = (uint32_t)(elapsedTicks / mTicksPerSecond);
  fractionalTicks = elapsedTicks - mTicksPerSecond * seconds;

  // This computes (performance counter ticks * 10^9)/(ticks per second) that gives
  // the time in nanoseconds. To avoid integer overflow in the multiplication (performance counter ticks * 10^9),
  // we do two things:
  // 1. split time into full seconds and fractions of a second;
  // 2. scale the operands to avoid 64-bit integer overflow AND to guarantee 1ns timer accuracy.
  //    Note that a * b = 10^9 and hence (ticks*10^9)/(ticks per second) = (ticks*a)/((ticks per second)/b).
  // The expression below will never overflows (as long as the performance counter stays within 64 bits),
  // it yields time in nanoseconds, and the result differs from the correct time by at most 1ns.
  if (b == 1)
  {
    return 1000000000 * seconds + (fractionalTicks * a) / mTicksPerSecond;
  }
  else
  {
    return 1000000000 * seconds + (fractionalTicks * a) / (mTicksPerSecond / b);
  }
}

/**
 * Gets instance of this Timer class.
 */
Timer* Timer::GetInstance(void)
{
  static Timer sInstance;
  return &sInstance;
}
