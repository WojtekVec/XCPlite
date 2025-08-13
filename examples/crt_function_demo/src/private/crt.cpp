/*----------------------------------------------------------------------------
| Project: Vector CANape/vSignalyzer C-RT DLL Project
|
| File: crt.cpp
|
| Description: Base C-RT function class implementation.
|
|-----------------------------------------------------------------------------
| $Author: viswp $    $Locker: $   $Revision: 7230 $
|-----------------------------------------------------------------------------
| Copyright (c) by Vector Informatik GmbH.  All rights reserved.
 ----------------------------------------------------------------------------*/
#pragma warning( disable:4786)

#include "crt.h"

#ifndef _WIN64
// Vector CANape/vSignalyzer C-RT 32-bit (x86) build
#pragma message( __FILE__ ": Vector CANape/vSignalyzer C-RT DLL 32-bit (x86) builds are deprecated and will be completely removed in future versions." )
#endif

#pragma region VCrtBase

 /**
  * \brief C-RT base class constructor.
  */
VCrtBase::VCrtBase(VIAService * pService)
  : mpService(pService)
{
  CreateVariables();
}

/**
 * \brief C-RT base class destructor.
 */
VCrtBase::~VCrtBase()
{
  DestroyVariables();
}

/**
 * \brief Destroys port variables.
 */
void VCrtBase::DestroyVariables()
{
  uint32_t i = 0;

  for (i = 0; i < mInputVariables.size(); i++)
  {
    delete mInputVariables[i];
  }
  mInputVariables.clear();

  for (i = 0; i < mOutputVariables.size(); i++)
  {
    delete mOutputVariables[i];
  }
  mOutputVariables.clear();

  mPorts.clear();
}

/**
 * \brief Creates port variables for communication with CANape/vSignalyzer host application.
 */
void VCrtBase::CreateVariables()
{ 
  uint32_t i = 0;

  //take the user entered input variables and put into Vector with full information  
  for (i = 0; i < gSizeUserInputVar; i++)
  {
    VariableBase* pVar = 0;
    switch(gUserInputVar[i].portType)
    {
      case kNumeric: 
        pVar = new Variable(gUserInputVar[i].name, gUserInputVar[i].rows, gUserInputVar[i].cols);
        break;
      case kImageProcessing:
        pVar = new VariableIP(gUserInputVar[i].name, gUserInputVar[i].rows, gUserInputVar[i].cols);
        break;
      case kBinary:
        pVar = new BinaryVariable(gUserInputVar[i].name, gUserInputVar[i].rows);
        break;
    }

    mInputVariables.push_back(pVar);
  }
  
  //create a map to enable the user easier addressing the variables during calculation
  for(i = 0; i < mInputVariables.size(); i++)
  {
    mPorts.insert(std::pair<const std::string, VariableBase*>(mInputVariables[i]->GetName(), mInputVariables[i]));
  }
  
  //take the user entered output variables and put into Vector with full information
  for (i = 0; i < gSizeUserOutputVar; i++)  
  {
    VariableBase* pVar = 0;
    switch(gUserOutputVar[i].portType)
    {
      case kNumeric: 
        pVar = new Variable(gUserOutputVar[i].name, gUserOutputVar[i].rows, gUserOutputVar[i].cols);
        break;
      case kImageProcessing:
        pVar = new VariableIP(gUserOutputVar[i].name, gUserOutputVar[i].rows, gUserOutputVar[i].cols);
        break;
      case kBinary:
        pVar = new BinaryVariable(gUserOutputVar[i].name, gUserOutputVar[i].rows);
        break;
    }

    mOutputVariables.push_back(pVar);    
  }

  //create a map to enable the user easier addressing the variables during calculation
  for (i = 0; i < mOutputVariables.size(); i++)
  {
    mPorts.insert(std::pair<const std::string, VariableBase*>(mOutputVariables[i]->GetName(), mOutputVariables[i]));
  }  
}


/**
 * \brief WriteString makes use of the host service
 *        and writes the specified string to CANape/vSignalyzer (host) Write-Window.
 */
void VCrtBase::WriteString(const char* text)
{
  if (mpService)
  {
    mpService->WriteString(text);
  }
}

#pragma endregion // VCrtBase