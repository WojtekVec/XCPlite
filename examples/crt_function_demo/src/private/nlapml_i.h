/*----------------------------------------------------------------------------
| File: nlapml_i.h
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
#pragma warning(disable : 4100)

#include "crt.h"

#include <cstdint>
#include <list>

using namespace std;

class VAPMLLayer;

class VNLAPMLOnTimerSink : public VIAOnTimerSink 
{
  public:
    VNLAPMLOnTimerSink(VAPMLLayer *layer, void (*function)(), double timeout);
    ~VNLAPMLOnTimerSink();

    VIASTDDECL SetTimer(VIATimer *timer);
    VIASTDDECL StartTimer();

    VIASTDDECL OnTimer(VIATime nanoseconds);

  private:
    VAPMLLayer *mLayer;
    void (*mFunction)();
    double mTimeout;
    VIATimer *mTimer;
};

class VNLAPMLOnEventSink : public VIAOnEnvVar 
{
  public:
    VNLAPMLOnEventSink(VIAOnEnvVar *layer, VIAOnEnvVar *sink);
    ~VNLAPMLOnEventSink();

    VIASTDDECL OnEnvVar(VIATime nanoseconds, VIAEnvVar *ev);

  private:
    VIAOnEnvVar *mLayer;
    VIAOnEnvVar *mSink;
};

class VNLAPMLModule : public VIAModuleApi 
{
  public:
    VNLAPMLModule(VIAService *service) : mService(service) {}

    // Initialize the nodelayer. Here you can do additional setup of the DLL.
    // CANoe/CANape calls this method after it ask for the object (function
    // 'VIAGetModuleApi') and before it calls any other method of this object.
    VIASTDDECL Init();

    // The text provided by GetVersion(...) is displayed in the info window
    // This is normally the name and version of this DLL.
    VIASTDDECL GetVersion(char *buffer, int32_t bufferLength);

    // With a Shift-DoubleClick in the Info-Window, the following information
    // is displayed in the Write-Window.
    // pathBuff: filename und path of this nodelayer
    // versionBuff: name und versionnumber of this nodelayer
    VIASTDDECL GetModuleParameters(char *pathBuff, int32_t pathBuffLength, char *versionBuff, int32_t versionBuffLength);

    // The information provided by GetNodeInfo(...) appears within the node
    // at the simulation setup window (shortName) and the status indication
    // line of CANoe/CANape (longName) .
    VIASTDDECL GetNodeInfo(const char *nodename, // name of the node
                           char *shortNameBuf, int32_t shortBufLength, char *longNameBuf, int32_t longBufLength);

    // Create a nodelayer object for the given node
    VIASTDDECL CreateObject(VIANodeLayerApi **object,    // the created instance
                            VIANode *node,               // the node
                            int32_t argc, char *argv[]); // additional parameters

    // Release the given nodelayer object
    VIASTDDECL ReleaseObject(VIANodeLayerApi *object);

    VIASTDDECL InitMeasurement();
    VIASTDDECL EndMeasurement();

  private:
    VIAService *mService;
};

class VNLAPMLLayer : public VIANodeLayerApi, public VIAOnTimerSink, public VIAOnEnvVar, public VAPMLLayer 
{
  public:
    VNLAPMLLayer(VIAService *service, VIANode *node);
    ~VNLAPMLLayer();

    // VIANodeLayerApi
    VIASTDDECL GetNode(VIANode **node);
    VIASTDDECL InitMeasurement();
    VIASTDDECL StartMeasurement();
    VIASTDDECL StopMeasurement();
    VIASTDDECL EndMeasurement();

    // VIAOnTimerSink
    VIASTDDECL OnTimer(VIATime nanoseconds);

    // VIAOnEnvVar
    VIASTDDECL OnEnvVar(VIATime nanoseconds, VIAEnvVar *ev);

    // VAPMLLayer
    VIASTDDECL SetMeasurementStartEvent(VIAOnMeasurementEvent *sink);
    VIASTDDECL SetMeasurementStopEvent(VIAOnMeasurementEvent *sink);

    VIASTDDECL GetEnvVar(VIAEnvVar **ev, const char *name);
    VIASTDDECL GetEventEnvVar(VIAEnvVar **ev, const char *name, VIAOnEnvVar *sink);
    VIASTDDECL GetSunkenEnvVar(VIAEnvVar **ev, const char *name);
    VIASTDDECL ReleaseEnvVar(VIAEnvVar *ev);
    VIASTDDECL AddEnvVarTrigger(VIAOnEnvVar *sink);
    VIASTDDECL RemoveEnvVarTrigger(VIAOnEnvVar *sink);

    VIASTDDECL GetTimerEnable(int *enable);
    VIASTDDECL PutTimerEnable(int enable);

    VIASTDDECL GetEnvironmentEventsEnable(int *enable);
    VIASTDDECL PutEnvironmentEventsEnable(int enable);

    VIASTDDECL GetCaplFunction(VIACaplFunction **caplfct, const char *functionName);
    VIASTDDECL ReleaseCaplFunction(VIACaplFunction *caplfct);

    VIASTDDECL PerformSimulationStep();

    VIASTDDECL Fatal(const char *msg);

    VIASTDDECL SetBaseTimer(void (*function)(), double timeout);
    VIASTDDECL CreateTimer(void (*function)(), double timeout);
    VIASTDDECL ReleaseTimer(VIATimer *timer);

    VIASTDDECL GetCurrentSimTime(VIATime *time);

    // new functions to support arrays
    VIASTDDECL GetEnvVar2(VIAEnvVar **ev, const char *name, int rows, int cols);
    VIASTDDECL GetSunkenEnvVar2(VIAEnvVar **ev, const char *name, int rows, int cols);

  private:
    /** Init the XCP protocol/server for the C-RT function. */
    void InitXcp();

    /** Terminate the XCP protocol/server. */
    void ExitXcp();

    VIAService *mService;
    VIANode *mNode;
    VIATimer *mTimer;
    VIATime mTimeout;
    int mTimerEnable;
    int mEventsEnable;
    VIATime mLastEventTime;
    list<VNLAPMLOnTimerSink *> mTimerSinks;
    list<VNLAPMLOnEventSink *> mEventSinks;
    list<VIAOnEnvVar *> mTriggerSinks;
    list<VIAOnMeasurementEvent *> mMeasurementStartSinks;
    list<VIAOnMeasurementEvent *> mMeasurementStopSinks;
    void (*mFunction)();

    VCrtBase *mAlgorithm;
};
