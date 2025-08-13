/*----------------------------------------------------------------------------
| File: nlapml.cpp
| Project: Vector CANape/vSignalyzer C-RT DLL Project
|
| Description: CANape/vSignalyzer NodeLayer DLL
|
|-----------------------------------------------------------------------------
| $Author: viswp $    $Locker: $   $Revision: 7229 $
|-----------------------------------------------------------------------------
| Copyright (c) by Vector Informatik GmbH.  All rights reserved.
----------------------------------------------------------------------------*/
#pragma warning( disable:4100)
#pragma warning( disable:4245)
#pragma warning( disable:4786)

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "cnpcomm.h"
#include "nlapml.h"
#include "nlapml_i.h"
#include "nlapml_v.h"
#include "Timer.h"
#include "xcp/xcp_cfg.h"
#include "xcp/xcp_par.h"

#include <cstdint>

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#define SHORT_PRODUCT_NAME "NL" // comes from node-layer
#define LONG_PRODUCT_NAME  "CANape Function DLL"

#define BCVC_QUOTE( name)   #name
#define BCVCQUOTE( name)    BCVC_QUOTE( name)
#define __FILE__LINE__      __FILE__ "(" BCVCQUOTE( __LINE__) ") : "

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/* Internal DLL execution time */
unsigned long _gRTTarget_ModelStepExecutionTime;
/* Internal DLL counter to proof algo execution */
unsigned long _gRTTarget_ModelExecutionCounter;
/* algorithm calculation time as provided by CANape */
extern "C" VIATime gCalcTime;

VAPMLLayer*        gLayer = NULL;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

static HINSTANCE   sInstance = NULL;

// Function execution timer
Utilities::Timer gExecTimer;

// Function meta information
extern VIAMetaInfo gFunctionMetaInfo;

VNLAPMLOnTimerSink::VNLAPMLOnTimerSink(VAPMLLayer* layer, void(*function)(),
  double timeout)
{
  mLayer = layer;
  mFunction = function;
  mTimeout = timeout * 1e9;
  mTimer = NULL;
}

VNLAPMLOnTimerSink::~VNLAPMLOnTimerSink()
{
  if (NULL != mTimer)
  {
    mLayer->ReleaseTimer(mTimer);

    mTimer = NULL;
  }
}

VIASTDDEF VNLAPMLOnTimerSink::SetTimer(VIATimer* timer)
{
  mTimer = timer;

  return kVIA_OK;
}

VIASTDDEF VNLAPMLOnTimerSink::StartTimer()
{
  return mTimer->SetTimer((VIATime)mTimeout);
}

VIASTDDEF VNLAPMLOnTimerSink::OnTimer(VIATime nanoseconds)
{
  // call function
  mFunction();

  if (NULL != mTimer)
  {
    mTimer->SetTimer((VIATime)mTimeout);
  }

  return kVIA_OK;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

VNLAPMLOnEventSink::VNLAPMLOnEventSink(VIAOnEnvVar* layer, VIAOnEnvVar* sink)
  :mLayer(layer)
  , mSink(sink)
{
}

VNLAPMLOnEventSink::~VNLAPMLOnEventSink()
{
}

VIASTDDEF VNLAPMLOnEventSink::OnEnvVar(VIATime nanoseconds, VIAEnvVar* ev)
{
  mSink->OnEnvVar(nanoseconds, ev);

  return mLayer->OnEnvVar(nanoseconds, ev);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

VIASTDDEF VNLAPMLModule::Init()
{
  return kVIA_OK;
}

VIASTDDEF VNLAPMLModule::GetVersion(char* buffer, int32_t bufferLength)
{
  strncpy(buffer, NLAPML_EXTERNAL_NAME, bufferLength - 1);

  return kVIA_OK;
}

VIASTDDEF VNLAPMLModule::GetModuleParameters(char* pathBuff, int32_t pathBuffLength,
                                             char* versionBuff, int32_t versionBuffLength)
{
  //  ::GetModuleFileName( sInstance, pathBuff, pathBuffLength);
  strncpy(versionBuff, NLAPML_EXTERNAL_NAME, versionBuffLength - 1);
  versionBuff[versionBuffLength - 1] = 0;

  return kVIA_OK;
}

VIASTDDEF VNLAPMLModule::GetNodeInfo(const char* nodename,
                                     char* shortNameBuf, int32_t shortBufLength,
                                     char* longNameBuf, int32_t longBufLength)
{
  strncpy(shortNameBuf, SHORT_PRODUCT_NAME, shortBufLength - 1);
  shortNameBuf[shortBufLength - 1] = 0;

  //create an object which tells the user entered name of the model
  string names = LONG_PRODUCT_NAME;
  names.append(": ");
  names.append(gModelName);

  //if ( strlen( LONG_PRODUCT_NAME ": " BCVCQUOTE( MODEL)) < ( size_t)longBufLength)
  if (strlen(names.c_str()) < (size_t)longBufLength)
  {
    //strncpy( longNameBuf, LONG_PRODUCT_NAME ": " BCVCQUOTE( MODEL), longBufLength - 1);
    strncpy(longNameBuf, names.c_str(), longBufLength - 1);
  }
  else
  {
    strncpy(longNameBuf, LONG_PRODUCT_NAME, longBufLength - 1);
  }
  longNameBuf[longBufLength - 1] = 0;

  return kVIA_OK;
}

VIASTDDEF VNLAPMLModule::CreateObject(VIANodeLayerApi** object,
                                      VIANode*          node,
                                      int32_t           argc,
                                      char*             argv[])
{
  double version = atof(NLAPML_PRODUCT_VERSION_STRING);
  double appVersion = 0;
  if (argc >= 1) {
    appVersion = atof(argv[0]);
  }

  if (appVersion >= version) {

    VNLAPMLLayer* layer;

    if (NULL == object)
    {
      return kVIA_ObjectCreationFailed;
    }

    *object = layer = new VNLAPMLLayer(mService, node);
    gLayer = layer;

    return NULL == layer ? kVIA_ObjectCreationFailed : kVIA_OK;

  }

  *object = NULL;
  return kVIA_ObjectCreationFailed;
}

VIASTDDEF VNLAPMLModule::ReleaseObject(VIANodeLayerApi* object)
{
  VNLAPMLLayer* layer = (VNLAPMLLayer*)object;

  delete layer;

  gLayer = NULL;

  return kVIA_OK;
}

VIASTDDEF VNLAPMLModule::InitMeasurement()
{
  return kVIA_OK;
}

VIASTDDEF VNLAPMLModule::EndMeasurement()
{
  return kVIA_OK;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
VNLAPMLLayer::VNLAPMLLayer(VIAService* service, VIANode* node)
  : mService(service)
  , mNode(node)
  , mTimer(NULL)
  , mTimeout(0)
  , mTimerEnable(1)
  , mEventsEnable(0)
  , mAlgorithm(NULL)
{

#ifdef XCP_TRANSPORT_LAYER_ETH
  // start XCP socket server for ETH communication
  const int kTcpProto = SOCK_STREAM;
  const int kUdpProto = SOCK_DGRAM;
  const unsigned short ethPort = 5555;
  xcpStartServer(kUdpProto, ethPort);
#endif

  // init global data
  _gRTTarget_ModelStepExecutionTime = 0;
  _gRTTarget_ModelExecutionCounter = 0;
  mAlgorithm = VCrtBase::CreateCrtFunction(service);
}

VNLAPMLLayer::~VNLAPMLLayer()
{
  unsigned int i;

  VariableBase* pVar = 0;
  for (i = 0; i < mAlgorithm->mInputVariables.size(); i++)
  {
    pVar = mAlgorithm->mInputVariables[i];
    if (pVar && pVar->GetEnvVar())
    {
      cnReleaseEnvironmentVariable(pVar->GetEnvVar());
    }
  }

  for (i = 0; i < mAlgorithm->mOutputVariables.size(); i++)
  {
    pVar = mAlgorithm->mOutputVariables[i];
    if (pVar && pVar->GetEnvVar())
    {
      cnReleaseEnvironmentVariable(pVar->GetEnvVar());
    }
  }

  delete mAlgorithm;
  mAlgorithm = NULL;

#ifdef XCP_TRANSPORT_LAYER_ETH
  // stop XCP socket server
  xcpStopServer();
#endif
}

// VIANodeLayerApi
VIASTDDEF VNLAPMLLayer::GetNode(VIANode** node)
{
  if (NULL == node)
  {
    return kVIA_ObjectNotFound;
  }

  *node = mNode;

  return kVIA_OK;
}

VIASTDDEF VNLAPMLLayer::InitMeasurement()
{
  // init counter
  _gRTTarget_ModelExecutionCounter = 0;
  // init measurement event
  mAlgorithm->InitMeasurement();

  VariableBase* pVar = 0;
  for (size_t i = 0; i < mAlgorithm->mInputVariables.size(); ++i)
  {
    pVar = mAlgorithm->mInputVariables[i];
    if (pVar)
    {
      if (pVar->GetEnvVar())
      {
        cnReleaseEnvironmentVariable(pVar->GetEnvVar());
      }
      pVar->SetEnvVar(cnCreateEnvironmentInputVariable2(pVar->GetName(),
        pVar->GetRows(),
        pVar->GetCols(),
        NULL));
    }
  }

  for (size_t i = 0; i < mAlgorithm->mOutputVariables.size(); ++i)
  {
    pVar = mAlgorithm->mOutputVariables[i];
    if (pVar)
    {
      if (pVar->GetEnvVar())
      {
        cnReleaseEnvironmentVariable(pVar->GetEnvVar());
      }
      pVar->SetEnvVar(cnCreateEnvironmentOutputVariable2(pVar->GetName(),
        pVar->GetRows(),
        pVar->GetCols(),
        NULL));
    }
  }

  // the counterpart port variables have already been created at this point
  // now the corresponding port data can be initialized e.g. port descriptions
  // call init data event
  mAlgorithm->InitData();


  if (gStepSize <= 0.0)
  {
    // default step size
    gStepSize = 0.1;
  }

  mTimeout = (VIATime)(gStepSize * 1000000000);

  return kVIA_OK;
}



VIASTDDEF VNLAPMLLayer::StartMeasurement()
{
  if (0 != mTimerEnable && 0 != mTimeout)
  {
    if (NULL == mTimer)
    {
      mService->CreateTimer(&mTimer, mNode, this, "VnlTestLayer");
    }

    if (NULL != mTimer)
    {
      mTimer->SetTimer(mTimeout);
    }
  }

  gExecTimer.Start();

  list<VNLAPMLOnTimerSink*>::iterator iter;

  for (iter = mTimerSinks.begin();
    iter != mTimerSinks.end();
    iter++)
  {
    (*iter)->StartTimer();
  }

  // start measurement event
  mAlgorithm->StartMeasurement();

  list<VIAOnMeasurementEvent*>::iterator miter;

  for (miter = mMeasurementStartSinks.begin();
    miter != mMeasurementStartSinks.end();
    miter++)
  {
    if (*miter) (*miter)->OnMeasurementEvent();
  }


#if defined ( PERFORM_SIMSTEP_AT_START )
  PerformSimulationStep();
#endif

  return kVIA_OK;
}

VIASTDDEF VNLAPMLLayer::StopMeasurement()
{
  // stop measurement event

  list<VIAOnMeasurementEvent*>::iterator miter;

  for (miter = mMeasurementStopSinks.begin();
    miter != mMeasurementStopSinks.end();
    miter++)
  {
    if (*miter) (*miter)->OnMeasurementEvent();
  }

  // clear timer sinks

  list<VNLAPMLOnTimerSink*>::iterator titer;

  for (titer = mTimerSinks.begin();
    titer != mTimerSinks.end();
    titer++)
  {
    delete (*titer);
  }

  mTimerSinks.clear();

  list<VNLAPMLOnEventSink*>::iterator eiter;

  for (eiter = mEventSinks.begin();
    eiter != mEventSinks.end();
    eiter++)
  {
    delete (*eiter);
  }

  mEventSinks.clear();
  mTriggerSinks.clear();

  // stop measurement event
  mAlgorithm->StopMeasurement();

  VIAResult result = kVIA_OK;

  if (0 != mTimer)
  {
    result = mService->ReleaseTimer(mTimer);

    mTimer = NULL;
  }

  return result;
}

VIASTDDEF VNLAPMLLayer::EndMeasurement()
{
  list<VIAOnMeasurementEvent*>::iterator miter;

  // clear measurement start sinks
  for (miter = mMeasurementStartSinks.begin();
    miter != mMeasurementStartSinks.end();
    miter++)
  {
    delete (*miter);
  }

  mMeasurementStartSinks.clear();

  // clear measurement stop sinks
  for (miter = mMeasurementStopSinks.begin();
    miter != mMeasurementStopSinks.end();
    miter++)
  {
    delete (*miter);
  }

  mMeasurementStopSinks.clear();

  // exit measurement event
  mAlgorithm->EndMeasurement();

  return kVIA_OK;
}

// VIAOnTimerSink
VIASTDDEF VNLAPMLLayer::OnTimer(VIATime nanoseconds)
{
  VIAResult result = kVIA_OK;
  gCalcTime = nanoseconds;
  gExecTimer.Reset();
  result = PerformSimulationStep();

  // XCP events
  // Base Sample Time
  xcpEvent((uint8_t)0);
  // slower XCP events
  if (0 == (_gRTTarget_ModelExecutionCounter % 10))
  {
    // Sample Time / 10 
    xcpEvent((uint8_t)1);
  }
  if (0 == (_gRTTarget_ModelExecutionCounter % 100))
  {
    // Sample Time / 100
    xcpEvent((uint8_t)2);
  }
  if (0 == (_gRTTarget_ModelExecutionCounter % 1000))
  {
    // Sample Time / 1000
    xcpEvent((uint8_t)3);
  }

  // check time at execution end
  _gRTTarget_ModelStepExecutionTime = (unsigned long)((gExecTimer.Elapsed() / 1000)); // us
  _gRTTarget_ModelExecutionCounter++;

  return kVIA_OK == result ? mTimer->SetTimer(mTimeout) : result;
}

// VIAOnEnvVar
VIASTDDEF VNLAPMLLayer::OnEnvVar(VIATime nanoseconds, VIAEnvVar* ev)
{
  if (nanoseconds == mLastEventTime)
  {
    return kVIA_OK;
  }

  mLastEventTime = nanoseconds;

  VIAResult result = 0 != mEventsEnable ?
    PerformSimulationStep() :
    kVIA_OK;

  list<VIAOnEnvVar*>::iterator titer;

  for (titer = mTriggerSinks.begin();
    titer != mTriggerSinks.end();
    titer++)
  {
    (*titer)->OnEnvVar(nanoseconds, ev);
  }

  return result;
}

// VAPMLLayer
VIASTDDEF VNLAPMLLayer::GetEnvVar(VIAEnvVar** ev, const char* name)
{
  return mService->GetEnvVar(ev, NULL, name, NULL);
}


VIASTDDEF VNLAPMLLayer::SetMeasurementStartEvent(VIAOnMeasurementEvent* sink)
{
  mMeasurementStartSinks.push_back(sink);

  return kVIA_OK;
}


VIASTDDEF VNLAPMLLayer::SetMeasurementStopEvent(VIAOnMeasurementEvent* sink)
{
  mMeasurementStopSinks.push_back(sink);

  return kVIA_OK;
}

VIASTDDEF VNLAPMLLayer::GetEventEnvVar(VIAEnvVar**  ev,
  const char*  name,
  VIAOnEnvVar* sink)
{
#if defined ( NO_ENVIRONMENT_EVENT_HOOK )
  return mService->GetEnvVar(ev, NULL, name, sink);
#else
  VNLAPMLOnEventSink* esink = new VNLAPMLOnEventSink(this, sink);

  mEventSinks.push_back(esink);

  return mService->GetEnvVar(ev, NULL, name, esink);
#endif
}

VIASTDDEF VNLAPMLLayer::GetSunkenEnvVar(VIAEnvVar** ev, const char* name)
{
  return mService->GetEnvVar(ev, NULL, name, this);
}

VIASTDDEF VNLAPMLLayer::ReleaseEnvVar(VIAEnvVar* ev)
{
  return mService->ReleaseEnvVar(ev);
}

VIASTDDEF VNLAPMLLayer::AddEnvVarTrigger(VIAOnEnvVar* sink)
{
  mTriggerSinks.push_back(sink);

  return kVIA_OK;
}

VIASTDDEF VNLAPMLLayer::RemoveEnvVarTrigger(VIAOnEnvVar* sink)
{
  mTriggerSinks.remove(sink);

  return kVIA_OK;
}

VIASTDDEF VNLAPMLLayer::GetTimerEnable(int* enable)
{
  *enable = mTimerEnable;

  return kVIA_OK;
}

VIASTDDEF VNLAPMLLayer::PutTimerEnable(int enable)
{
  mTimerEnable = enable;

  return kVIA_OK;
}

VIASTDDEF VNLAPMLLayer::GetEnvironmentEventsEnable(int* enable)
{
  *enable = mEventsEnable;

  return kVIA_OK;
}

VIASTDDEF VNLAPMLLayer::PutEnvironmentEventsEnable(int enable)
{
  mEventsEnable = enable;

  return kVIA_OK;
}

VIASTDDEF VNLAPMLLayer::GetCaplFunction(VIACaplFunction** caplfct,
  const char* functionName)
{
  return mNode->GetCaplFunction(caplfct, functionName);
}

VIASTDDEF VNLAPMLLayer::ReleaseCaplFunction(VIACaplFunction* caplfct)
{
  return mNode->ReleaseCaplFunction(caplfct);
}

VIASTDDEF VNLAPMLLayer::PerformSimulationStep()
{
  unsigned int i;

  for (i = 0; i < mAlgorithm->mInputVariables.size(); i++)
  {
    VariableBase* inVar = mAlgorithm->mInputVariables[i];
    inVar->GetEnvValues();
  }

  mAlgorithm->Calculate();

  for (i = 0; i < mAlgorithm->mOutputVariables.size(); i++)
  {
    VariableBase* outVar = mAlgorithm->mOutputVariables[i];
    // check the specified sample time type
    // prevent oversampling of output port, which are written slower than the base sample rate
    if (outVar->GetSampleTimeType() == kFundamental)
    {
      outVar->SetEnvValues();
    }
  }

  return kVIA_OK;
}

VIASTDDEF VNLAPMLLayer::Fatal(const char* msg)
{
  mService->WriteString(msg);
  mService->Stop();

  return kVIA_OK;
}

VIASTDDEF VNLAPMLLayer::SetBaseTimer(void(*function)(), double timeout)
{
  mFunction = function;
  mTimeout = (VIATime)(timeout * 1e9);

  return mService->CreateTimer(&mTimer, mNode, this, "ModelTimer");
}

VIASTDDEF VNLAPMLLayer::CreateTimer(void(*function)(), double timeout)
{
  VNLAPMLOnTimerSink* sink = new VNLAPMLOnTimerSink(this, function, timeout);

  mTimerSinks.push_back(sink);

  VIATimer* timer = NULL;
  VIAResult result = mService->CreateTimer(&timer, mNode, sink, "ModelTimer");

  if (NULL != timer)
  {
    sink->SetTimer(timer);
  }

  return result;
}

VIASTDDEF VNLAPMLLayer::ReleaseTimer(VIATimer* timer)
{
  return mService->ReleaseTimer(timer);
}

VIASTDDEF VNLAPMLLayer::GetCurrentSimTime(VIATime* time)
{
  return mService->GetCurrentSimTime(time);
}

VIASTDDEF VNLAPMLLayer::GetEnvVar2(VIAEnvVar** ev, const char* name, int rows, int cols)
{
  return mService->GetEnvVar2(ev, NULL, name, rows, cols, NULL);
}

VIASTDDEF VNLAPMLLayer::GetSunkenEnvVar2(VIAEnvVar** ev, const char* name, int rows, int cols)
{
  return mService->GetEnvVar2(ev, NULL, name, rows, cols, this);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

VIACLIENT(void) VIARequiredVersion(int32_t* majorversion, int32_t* minorversion)
{
  if (NULL != majorversion && NULL != minorversion)
  {
    *majorversion = VIAMajorVersion;
    *minorversion = VIAMinorVersion;
  }
}

static VIAService* sService = NULL;

VIACLIENT(void) VIASetService(VIAService* service)
{
  sService = service;
}

VIACLIENT(VIAModuleApi*) VIAGetModuleApi(int32_t argc, char* argv[])
{
  return NULL == sService ? NULL : new VNLAPMLModule(sService);
}


VIACLIENT(void) VIAReleaseModuleApi(VIAModuleApi* api)
{
  VNLAPMLModule* module = (VNLAPMLModule*)api;

  delete module;
}

/**
 * Returns CANape Function Meta Information.
 */
VIACLIENT(VIAMetaInfo*) VIAGetMetaInfo()
{
  return &gFunctionMetaInfo;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

extern "C" __declspec(dllexport) void* __stdcall NLGetModuleOptions(int option)
{
  if (0 == option)
  {
    return (void*)2;
  }

  return NULL;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

BOOL APIENTRY DllMain(HINSTANCE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
  if (DLL_PROCESS_ATTACH == ul_reason_for_call)
  {
    sInstance = hModule;
  }
  else if (DLL_PROCESS_DETACH == ul_reason_for_call)
  {
    VIASetService(NULL);
  }

  return TRUE;
}
