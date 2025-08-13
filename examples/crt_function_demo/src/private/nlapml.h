/*----------------------------------------------------------------------------
| File: nlapml.h
| Project: Vector CANape/vSignalyzer C-RT DLL Project
|
| Description: CANape/vSignalyzer NodeLayer DLL
|
|-----------------------------------------------------------------------------
| $Author: viswp $    $Locker: $   $Revision: 6295 $
|-----------------------------------------------------------------------------
| Copyright (c) by Vector Informatik GmbH.  All rights reserved.
 ----------------------------------------------------------------------------*/
#pragma once

#include "via\via.h"

class VAPMLLayer
{
public:
   // start/stop measurement event
   VIASTDDECL SetMeasurementStartEvent(VIAOnMeasurementEvent* sink) = 0;
   VIASTDDECL SetMeasurementStopEvent(VIAOnMeasurementEvent* sink) = 0;

   VIASTDDECL GetEnvVar( VIAEnvVar** ev, const char* name) = 0;
   VIASTDDECL GetEventEnvVar( VIAEnvVar** ev, const char* name,
                VIAOnEnvVar* sink) = 0;
   VIASTDDECL GetSunkenEnvVar( VIAEnvVar** ev, const char* name) = 0;
   VIASTDDECL ReleaseEnvVar( VIAEnvVar* ev) = 0;
   VIASTDDECL AddEnvVarTrigger( VIAOnEnvVar* sink) = 0;
   VIASTDDECL RemoveEnvVarTrigger( VIAOnEnvVar* sink) = 0;

   VIASTDDECL GetTimerEnable( int* enable) = 0;
   VIASTDDECL PutTimerEnable( int enable) = 0;

   VIASTDDECL GetEnvironmentEventsEnable( int* enable) = 0;
   VIASTDDECL PutEnvironmentEventsEnable( int enable) = 0;

   VIASTDDECL GetCaplFunction( VIACaplFunction** caplfct,
                const char* functionName) = 0;
   VIASTDDECL ReleaseCaplFunction( VIACaplFunction* caplfct) = 0;

   VIASTDDECL PerformSimulationStep() = 0;

   VIASTDDECL Fatal( const char* msg) = 0;

   VIASTDDECL SetBaseTimer( void ( *function)(), double timeout) = 0;
   VIASTDDECL CreateTimer( void ( *function)(), double timeout) = 0;
   VIASTDDECL ReleaseTimer( VIATimer* timer) = 0;

   VIASTDDECL GetCurrentSimTime( VIATime* time) = 0;

   // new functions to support arrays
   VIASTDDECL GetEnvVar2( VIAEnvVar** ev, const char* name, int rows, int cols) = 0;
   VIASTDDECL GetSunkenEnvVar2( VIAEnvVar** ev, const char* name, int rows, int cols) = 0;

};
