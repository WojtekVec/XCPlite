/*----------------------------------------------------------------------------
| File: cnpcomm.cpp
|
| Project: Vector MATLAB/Simulink MC Add-on CANape/vSignalyzer Target
|          and Vector CANape/vSignalyzer C-RT DLL Project
|
| Description:
|
|    CANape/vSignalyzer communication layer implementation
|
|-----------------------------------------------------------------------------
| $Author: viswp $    $Locker: $   $Revision: 6022 $
|-----------------------------------------------------------------------------
| Copyright (c) by Vector Informatik GmbH.  All rights reserved.
 ----------------------------------------------------------------------------*/
#pragma warning( disable:4786)

#include "cnpcomm.h"

#if defined ( _LIB )
typedef void (*pfnOutputs)(void* o, const double& d);
#else
typedef void (*pfnOutputs)(void* s, int32_t tid);
#endif

#if defined ( _BUILDNODELAYERDLL )

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//                           CANape NodeLayer DLL                            //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#pragma warning( disable : 4514)

typedef void* LPVOID;

#include "..\private\nlapml.h"

extern VAPMLLayer* gLayer;

class COnMeasurementEvent : public VIAOnMeasurementEvent
{
public:
  COnMeasurementEvent(pfnOutputs pfn, void* s) { mPfn = pfn; mS = s; }

  VIASTDDECL OnMeasurementEvent();

private:
  pfnOutputs mPfn;
  void* mS;
};

class COnEnvVar : public VIAOnEnvVar
{
public:
  COnEnvVar(pfnOutputs pfn, void* s) { mPfn = pfn; mS = s; }

  VIASTDDECL OnEnvVar(VIATime nanoseconds, VIAEnvVar* ev);

private:
  pfnOutputs mPfn;
  void* mS;
};

class CnEnvVar
{
public:
  CnEnvVar(VIAEnvVar* var, COnEnvVar* change) 
  { 
    mVar = var; mChange = change; 
  }
  virtual ~CnEnvVar();

  virtual double GetValue() = 0;
  virtual void   SetValue(double val) = 0;
  virtual void   GetValueString(char* str, int32_t len) = 0;
  virtual void   SetValueString(const char* val) = 0;
  virtual double GetValue2(int row, int col) = 0;
  virtual void   SetValue2(double val, int row, int col) = 0;
  virtual void   GetDimension(int* rows, int* cols) = 0;
  virtual void   GetData(uint8_t* data, int dataLength) = 0;
  virtual void   SetData(const uint8_t* data, int dataLength) = 0;
  virtual void   GetImageData(int8_t* data, BITMAPINFOHEADER* bmi) = 0;
  virtual void   SetImageData(const int8_t* buffer, BITMAPINFOHEADER* bmi) = 0;

  // VIA version 2.0 interface extension
  virtual double GetValueEx(int row, int col, VIATime* pTimeStamp) { double d(0.0);  mVar->GetFloatEx(&d, row, col, pTimeStamp); return d; }
  virtual void   GetDataEx(uint8_t* pBuffer, int32_t bufferLength, int32_t* pNumberOfBytesWritten, VIATime* pTimeStamp) = 0;
  virtual void   SetDataEx(const uint8_t* pBuffer, int32_t bufferLength, int32_t numberOfBytesWritten) = 0;
  virtual void   SetDescription(const char* pDescription, int32_t descriptionLength, int32_t descriptionType) = 0;

  VIAEnvVar* mVar;
  COnEnvVar* mChange;
};

class CnIntEnvVar : public CnEnvVar
{
public:
  CnIntEnvVar(VIAEnvVar* var, COnEnvVar* change = NULL)
    :CnEnvVar(var, change) {}

  virtual double GetValue() { int32_t i; mVar->GetInteger(&i); return i; }
  virtual void   SetValue(double val) { mVar->SetInteger((int32_t)val); }
  virtual void   GetValueString(char* str, int32_t len) { mVar->GetString(str, len); }
  virtual void   SetValueString(const char* val) { mVar->SetString(val); }
  virtual double GetValue2(int row, int col) { int32_t i; mVar->GetInteger2(&i, row, col); return i; }
  virtual void   SetValue2(double val, int row, int col) { mVar->SetInteger2((int32_t)val, row, col); }
  virtual void   GetDimension(int* rows, int* cols) { mVar->GetDimension(rows, cols); }
  virtual void   GetData(uint8_t* data, int dataLength) { mVar->GetData(data, dataLength); }
  virtual void   SetData(const uint8_t* data, int dataLength) { mVar->SetData(data, dataLength); }
  virtual void   GetImageData(int8_t* data, BITMAPINFOHEADER* bmi) { mVar->GetImageData((uint8_t*)data, bmi); }
  virtual void   SetImageData(const int8_t* buffer, BITMAPINFOHEADER* bmi) { mVar->SetImageData((const uint8_t*)buffer, bmi); }

  // VIA version 2.0 interface extension
  virtual void   GetDataEx(uint8_t* pBuffer, int32_t bufferLength, int32_t* pNumberOfBytesWritten, VIATime* pTimeStamp) {}
  virtual void   SetDataEx(const uint8_t* pBuffer, int32_t bufferLength, int32_t numberOfBytesWritten) {}
  virtual void   SetDescription(const char* pDescription, int32_t descriptionLength, int32_t descriptionType) {}
};

class CnFloatEnvVar : public CnEnvVar
{
public:
  CnFloatEnvVar(VIAEnvVar* var, COnEnvVar* change = NULL)
    :CnEnvVar(var, change) {}

  virtual double GetValue() { double d(0.0); mVar->GetFloat(&d); return d; }
  virtual void   SetValue(double val) { mVar->SetFloat((double)val); }
  virtual void   GetValueString(char* str, int32_t len) { mVar->GetString(str, len); }
  virtual void   SetValueString(const char* val) { mVar->SetString(val); }
  virtual double GetValue2(int row, int col) { double d(0.0); mVar->GetFloat2(&d, row, col); return d; }
  virtual void   SetValue2(double val, int row, int col) { mVar->SetFloat2((double)val, row, col); }
  virtual void   GetDimension(int* rows, int* cols) { mVar->GetDimension(rows, cols); }
  virtual void   GetData(uint8_t* data, int dataLength) { mVar->GetData(data, dataLength); }
  virtual void   SetData(const uint8_t* data, int dataLength) { mVar->SetData(data, dataLength); }
  virtual void   GetImageData(int8_t* data, BITMAPINFOHEADER* bmi) { mVar->GetImageData((uint8_t*)data, bmi); }
  virtual void   SetImageData(const int8_t* buffer, BITMAPINFOHEADER* bmi) { mVar->SetImageData((const uint8_t*)buffer, bmi); }

  // VIA version 2.0 interface extension
  virtual double GetValueEx(int row, int col, VIATime* pTimeStamp)
  {
    double d;
    mVar->GetFloatEx(&d, row, col, pTimeStamp);
    return d;
  }

  virtual void GetDataEx(uint8_t* pBuffer, int32_t bufferLength, int32_t* pNumberOfBytesWritten, VIATime* pTimeStamp)
  {
    mVar->GetDataEx(pBuffer, bufferLength, pNumberOfBytesWritten, pTimeStamp);
  }

  virtual void SetDataEx(const uint8_t* pBuffer, int32_t bufferLength, int32_t numberOfBytesWritten)
  {
    mVar->SetDataEx(pBuffer, bufferLength, numberOfBytesWritten);
  }

  virtual void SetDescription(const char* pDescription, int32_t descriptionLength, int32_t descriptionType)
  {
    mVar->SetDescription(pDescription, descriptionLength, descriptionType);
  }
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

VIASTDDEF COnMeasurementEvent::OnMeasurementEvent()
{
  mPfn(mS, 0);

  return kVIA_OK;
}

VIASTDDEF COnEnvVar::OnEnvVar(VIATime /*nanoseconds*/, VIAEnvVar* /*ev*/)
{
  mPfn(mS, 0);

  return kVIA_OK;
}

CnEnvVar::~CnEnvVar()
{
  if (NULL != gLayer)
  {
    gLayer->ReleaseEnvVar(mVar);
  }

  if (NULL != mChange)
  {
    delete mChange;
  }
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

const void* cnCreateEnvironmentInputVariable(const char* name, const void* group)
{
  return cnCreateEnvironmentInputVariable2(name, 1, 1, group);
}


const void* cnCreateEnvironmentOutputVariable(const char* name, const void* group)
{
  return cnCreateEnvironmentOutputVariable2(name, 1, 1, group);
}


const void* cnCreateEnvironmentEventVariable(const char* name, void* pfn, void* s)
{
  CnEnvVar* env = NULL;
  VIAEnvVar* var = NULL;
  COnEnvVar* change;
  uint32_t   type;

  if (NULL == gLayer)
  {
    return NULL;
  }

  change = new COnEnvVar((pfnOutputs)pfn, s);

  if (gLayer->GetEventEnvVar(&var, name, change) != kVIA_OK)
  {
    delete change;

    return env;
  }

  var->GetType(&type);

  switch (type)
  {
  case kVIA_EVInteger:
    env = new CnIntEnvVar(var, change);
    break;
  case kVIA_EVFloat:
    env = new CnFloatEnvVar(var, change);
    break;
  default:
    delete change;
    break;
  }


  return env;
}

// create event for measurement start/stop
const void* cnCreateMeasurementStartEvent(void* pfn, void* s)
{
  VIAOnMeasurementEvent* change = NULL;

  if (NULL == gLayer)
  {
    return NULL;
  }

  change = new COnMeasurementEvent((pfnOutputs)pfn, s);

  if (gLayer->SetMeasurementStartEvent(change) != kVIA_OK)
  {
    //delete change;

  }

  return 0;
}

// create event for measurement start/stop
const void* cnCreateMeasurementStopEvent(void* pfn, void* s)
{
  VIAOnMeasurementEvent* change = NULL;

  if (NULL == gLayer)
  {
    return NULL;
  }

  change = new COnMeasurementEvent((pfnOutputs)pfn, s);

  if (gLayer->SetMeasurementStopEvent(change) != kVIA_OK)
  {
    //delete change;

  }

  return 0;
}

void cnReleaseEnvironmentVariable(const void* var)
{
  CnEnvVar* env = (CnEnvVar*)var;
  delete env;
  // mark as deleted by setting to NULL pointer
  env = NULL;
}

double cnGetEnvironmentValue(const void* var)
{
  return cnGetEnvironmentValue2(var, 0, 0);
}

void cnSetEnvironmentValue(const void* var, double val)
{
  cnSetEnvironmentValue2(var, val, 0, 0);
}

void cnGetEnvironmentValueString(const void* var, char* val, int32_t len)
{
  CnEnvVar* env = (CnEnvVar*)var;

  if (NULL != env)
  {
    env->GetValueString(val, len);
  }
}

void cnSetEnvironmentValueString(const void* var, const char* val)
{
  CnEnvVar* env = (CnEnvVar*)var;

  if (NULL != env)
  {
    env->SetValueString(val);
  }
}

const void* cnCreateEnvironmentTrigger(void* pfn, void* s)
{
  if (NULL == gLayer)
  {
    return NULL;
  }

  COnEnvVar* trigger = new COnEnvVar((pfnOutputs)pfn, s);

  gLayer->AddEnvVarTrigger(trigger);

  return trigger;
}

void cnReleaseEnvironmentTrigger(const void* trig)
{
  if (NULL == gLayer)
  {
    return;
  }

  COnEnvVar* trigger = (COnEnvVar*)trig;

  if (NULL != trigger)
  {
    gLayer->RemoveEnvVarTrigger(trigger);

    delete trigger;
  }
}

void cnSetEnable(int32_t timer, int32_t events)
{
  if (NULL == gLayer)
  {
    return;
  }

  gLayer->PutTimerEnable(timer);
  gLayer->PutEnvironmentEventsEnable(events);
}

void cnGetEnable(int32_t* timer, int32_t* events)
{
  if (NULL == gLayer)
  {
    return;
  }

  gLayer->GetTimerEnable(timer);
  gLayer->GetEnvironmentEventsEnable(events);
}

const void* cnCreateSimulation()
{
  return NULL;
}

void cnReleaseSimulation(const void* /*sim*/)
{
}

void cnIncreaseTime(const void* /*sim*/, double /*time*/)
{
}


const void* cnCreateCAPLHandler(const char* name)
{
  VIACaplFunction* caplfct;

  gLayer->GetCaplFunction(&caplfct, name);

  return caplfct;
}

void cnReleaseCAPLHandler(const void* fcn)
{
  VIACaplFunction* caplfct = (VIACaplFunction*)fcn;

  gLayer->ReleaseCaplFunction(caplfct);
}

void cnCallCAPLHandler(const void* handler)
{
  VIACaplFunction* caplfct = (VIACaplFunction*)handler;
  uint32_t         result;

  caplfct->Call(&result, NULL);
}

void cnFatal(const char* msg)
{
  if (NULL == gLayer)
  {
    return;
  }

  gLayer->Fatal(msg);
}

void cnSetBaseTimer(void (*function)(), double timeout)
{
  gLayer->SetBaseTimer(function, timeout);
}

void cnCreateTimer(void (*function)(), double timeout)
{
  gLayer->CreateTimer(function, timeout);
}

double cnGetCurrentTime()
{
  if (NULL == gLayer)
  {
    return 0.0;

  }

  VIATime time;

  gLayer->GetCurrentSimTime(&time);

  return (double)time / 1000000.0;
}

const void* cnCreateEnvironmentInputVariable2(const char* name, int rows, int cols, const void*)
{
  CnEnvVar*  env = NULL;
  VIAEnvVar* var = NULL;
  uint32_t   type;

  if (NULL == gLayer)
  {
    return NULL;
  }

  if (gLayer->GetSunkenEnvVar2(&var, name, rows, cols) == kVIA_OK)
  {
    var->GetType(&type);

    switch (type)
    {
    case kVIA_EVInteger:
      env = new CnIntEnvVar(var);
      break;
    case kVIA_EVFloat:
      env = new CnFloatEnvVar(var);
      break;
    case kVIA_EVString:
      env = new CnIntEnvVar(var);
      break;
    default:
      break;
    }
  }

  return env;
}

const void* cnCreateEnvironmentOutputVariable2(const char* name, int rows, int cols, const void*)
{
  CnEnvVar*  env = NULL;
  VIAEnvVar* var = NULL;
  uint32_t   type;

  if (NULL == gLayer)
  {
    return NULL;
  }

  if (gLayer->GetEnvVar2(&var, name, rows, cols) == kVIA_OK)
  {
    var->GetType(&type);

    switch (type)
    {
    case kVIA_EVInteger:
      env = new CnIntEnvVar(var);
      break;
    case kVIA_EVFloat:
      env = new CnFloatEnvVar(var);
      break;
    case kVIA_EVString:
      env = new CnIntEnvVar(var);
      break;
    default:
      break;
    }
  }

  return env;
}

double cnGetEnvironmentValue2(const void* var, int row, int col)
{
  CnEnvVar* env = (CnEnvVar*)var;

  if (NULL != env)
  {
    return env->GetValue2(row, col);
  }

  return 0.0;
}

void cnSetEnvironmentValue2(const void* var, double val, int row, int col)
{
  CnEnvVar* env = (CnEnvVar*)var;

  if (NULL != env)
  {
    env->SetValue2(val, row, col);
  }
}

void cnGetEnvironmentDimension(const void* var, int* row, int* col)
{
  CnEnvVar* env = (CnEnvVar*)var;

  if (NULL != env)
  {
    env->GetDimension(row, col);
  }
}

void cnGetEnvironmentData(const void* var, int8_t* data, BITMAPINFOHEADER* bmi)
{
  CnEnvVar* env = (CnEnvVar*)var;

  if (NULL != env)
  {
    env->GetImageData(data, bmi);
  }
}

void cnSetEnvironmentData(const void* var, int8_t* data, BITMAPINFOHEADER* bmi)
{
  CnEnvVar* env = (CnEnvVar*)var;

  if (NULL != env)
  {
    env->SetImageData(data, bmi);
  }
}

void cnGetEnvironmentBinaryData(const void* var, uint8_t* data, int size)
{
  CnEnvVar* env = (CnEnvVar*)var;

  if (NULL != env)
  {
    env->GetData(data, size);
  }
}

void cnSetEnvironmentBinaryData(const void* var, const uint8_t* data, int size)
{
  CnEnvVar* env = (CnEnvVar*)var;

  if (NULL != env)
  {
    env->SetData(data, size);
  }
}

double cnGetEnvironmentValueEx(const void* var, int row, int col, int64_t* pTimeStamp)
{
  CnEnvVar* env = (CnEnvVar*)var;

  if (NULL != env)
  {
    return env->GetValueEx(row, col, pTimeStamp);
  }

  return 0.0;
}

void cnGetEnvironmentBinaryDataEx(const void* var, uint8_t* pData, int32_t dataSize, int32_t* pNumberOfBytesWritten, int64_t* pTimeStamp)
{
  CnEnvVar* env = (CnEnvVar*)var;

  if (NULL != env)
  {
    env->GetDataEx(pData, dataSize, pNumberOfBytesWritten, pTimeStamp);
  }
}

void cnSetEnvironmentBinaryDataEx(const void* var, const uint8_t* pData, int32_t dataSize, int32_t numberOfBytesWritten)
{
  CnEnvVar* env = (CnEnvVar*)var;

  if (NULL != env)
  {
    env->SetDataEx(pData, dataSize, numberOfBytesWritten);
  }
}

void cnSetDescription(const void* var, const char* pDescription, int32_t descriptionLength, int32_t descriptionType)
{
  CnEnvVar* env = (CnEnvVar*)var;

  if (NULL != env)
  {
    env->SetDescription(pDescription, descriptionLength, descriptionType);
  }
}


#endif // _BUILDNODELAYERDLL