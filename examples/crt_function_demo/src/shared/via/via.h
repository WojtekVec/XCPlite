/*----------------------------------------------------------------------------
| File: VIA.h
| Project: Vector MATLAB/Simulink MC Add-on CANape/vSignalyzer Target
|          and Vector CANape/vSignalyzer C-RT DLL Project
|
| Description: New external interface for CANoe/CANape nodes (Nodelayer-DLLs)
|
|-----------------------------------------------------------------------------
| $Author: vislmk $    $Locker: $   $Revision: 8042 $
|-----------------------------------------------------------------------------
| Copyright (c) by Vector Informatik GmbH.  All rights reserved.
----------------------------------------------------------------------------*/
#pragma once

#include "mbd_types.h"

// ============================================================================
// Version of the interfaces, which are declared in this header
// See  REVISION HISTORY for a description of the versions
// ============================================================================

// Version 2.0 contains the new VIA Interface Extension
const int32_t VIAMajorVersion = 2;
const int32_t VIAMinorVersion = 0;

// ============================================================================
// Dependencies
// ============================================================================

// We need only the type HWND from <windows.h>.
// If you don't need windows, the following dummy declaration is all what you
// need. Otherwise include <windows.h> before you process this header.
#ifndef _WINDOWS_
typedef void*   HWND;
#endif // _WINDOWS_

// some forward declarations
class VIANode;
class VIANodeLayerApi;
class VIAModuleApi;
class VIARequest;
class VIAEnvVar;
class VIAUtil;


#if defined ( _BUILDNODELAYERDLL )
#define VIACLIENT(type)  extern "C" __declspec (dllexport) type __stdcall
#else
#define VIACLIENT(type)  extern "C" __declspec (dllimport) type __stdcall
#endif



// ============================================================================
// Standard return type and return values for the services
// ============================================================================



// 'VIAResult' is the standard result type of all methods declared below.
typedef int32_t VIAResult;


// Standard declaration and definition macro for methods with an result of
// type 'VIAResult'.
#define VIASTDDECL  virtual VIAResult __stdcall
#define VIASTDDEF   VIAResult __stdcall


// The following are return values of the services
enum VIAResultValue {
  kVIA_OK = 0,              // normal result value used if a call happens well
  kVIA_WrongVersion = 1,
  kVIA_MissingInterface = 2,
  kVIA_ObjectCreationFailed = 3,
  kVIA_ServiceNotRunning = 4,
  kVIA_ObjectNotFound = 5,
  kVIA_BufferToSmall = 6,
  kVIA_ObjectInvalid = 7,
  kVIA_FunctionNotImplemented = 8,

  kVIA_DBUnspecificError = 100,
  kVIA_DBAttributeNameNotFound = 101,
  kVIA_DBObjectNameNotFound = 102,
  kVIA_DBParameterInvalid = 103
}; // VIAResult


// ============================================================================
// VIATime
//
// For time information we are using a 64 bit integer.
// VIATime(1) is one nanosecond
// ============================================================================

typedef int64_t VIATime;


inline VIATime VIATimeMilliSec(int ms)
{
  return VIATime(1000000) * VIATime(ms);
}


// ============================================================================
// Request blocks
// ============================================================================


typedef VIARequest* VIARequestHandle;

//
// VIARequest is an empty class, thats used to build a generic handle for
// Requests.
//

class VIARequest
{
};  // class VIARequest



// ============================================================================
// Bus interface (common part)
// ============================================================================


// Channel of a bus interface.
// Attention: the numbering starts with one. The value zero is used as a
// defined undefined value.
typedef uint16_t   VIAChannel;
const VIAChannel kVIA_WildcardChannel = 0xFFFF; // any channel
const VIAChannel kVIA_InvalidChannel = 0x0000; // no channel



enum VIADir             // Direction of a message
{
  kVIA_Rx = 0,    // receive
  kVIA_Tx = 1,    // transmit
  kVIA_TxRq = 2     // transmit request
};



enum VIARequestType    // used for creating request about messages on an bus
{
  kVIA_OneId = 0,   // Receive only one Id
  kVIA_AllId = 1,   // Receive all Ids
  kVIA_MskId = 2    // Receive all Ids specified by Mask
};



enum VIAOnlineOffline // Constants for method SetLine, which switches parts
{                      // of the bus interface on or off
  kVIA_Online = 1,   // switch bus interface on
  kVIA_Offline = 0,   // switch bus interface off
  kVIA_Model = 2,   // switching impacts CAPL program
  kVIA_DLL = 4    // switching impacts nodelayer dll
};



enum VIABusInterfaceType
{
  kVIA_CAN = 1, // CAN bus at level 2
  kVIA_1939 = 2, // CAN bus at J1939 view
  kVIA_VAN = 3, // VAN
  kVIA_TTP = 4, // TTP
  kVIA_LIN = 5, // LIN
  kVIA_MOST = 6  // MOST
};



class VIABus
{
public:
  // Get the type (see enum VIABusInterfaceType) and the version of this
  // bus interface object.
  VIASTDDECL GetVersion(uint32_t* busInterfaceType,
                        int32_t* majorversion, int32_t* minorversion) = 0;

  // Get the number of available channels for this bus interface
  VIASTDDECL GetNumberOfChannels(VIAChannel* maxChannelNumber) = 0;

  // SetLine(...) is used to switch the bus interface of a node on or off.
  // Parameter channel describes the channel, that should be turned on or off.
  //   At the moment, kVIA_WildcardChannel is the only supported value for
  //   the parameter channel
  // Parameter mode is kVIA_Online or kVIA_Offline
  // Parameter part is a combination of the Flags kVIA_Model and kVIA_DLL,
  //   which describes, which part of a node is switched on or off.
  VIASTDDECL SetLine(VIAChannel channel, uint32_t mode, uint32_t part32) = 0;

  // General method to release a request for messages on the bus.
  // The creation of the events is specific to the concrete  bus interface
  VIASTDDECL ReleaseRequest(VIARequestHandle handle) = 0;
}; // class VIABus


// ============================================================================
// Interface to timer
// ============================================================================


//
// callback object for a ringing timer
//
class VIAOnTimerSink
{
public:
  // The time provided by the callback method 'OnTimer' is the time, when
  // the timer should have done a ring. (This is like an ideal timer, that
  // never has a delay even when the CPU is busy.)
  VIASTDDECL OnTimer(VIATime nanoseconds) = 0;
}; // class VIAOnTimer


//
// The timer object itself
//
class VIATimer
{
public:

  // Set the callback object of the timer. The sink-object will be referenced
  // and MUST NOT be deleted until the timer is released or the sink is set
  // otherwise.
  VIASTDDECL SetSink(VIAOnTimerSink* sink) = 0; // sink will be referenced

  // Set the name of the timer. The name is used for debugging purpose
  // and for warnings, which appear in the "Write Window" of CANalyzer/CANoe
  // The name will be copied into the timer object, so you are allowed to
  // delete the string directly after the call of method.
  VIASTDDECL SetName(const char* name) = 0;

  VIASTDDECL SetTimer(VIATime nanoseconds) = 0;
  VIASTDDECL CancelTimer() = 0;
}; // class VIATimer



// ============================================================================
// Access to CAPL
// ============================================================================



class VIACaplFunction
{
public:
  // Total number of bytes used for all parameters of this CAPL function
  VIASTDDECL ParamSize(int32_t* size) = 0;

  // Number of parameters in the signature of this CAPL function
  VIASTDDECL ParamCount(int32_t* size) = 0;

  // The type of the nth (0, 1, 2 ...) parameter in the signature of this
  // CAPL function. See file 'cdll.h' for the meaning of the types.
  VIASTDDECL ParamType(char* type, int32_t nth) = 0;

  // The type of the return value. See file 'cdll.h' for the meaning of
  // parameter type..
  VIASTDDECL ResultType(char* type) = 0;

  // Call the CAPL function with the given parameters.
  // Params is a pointer to the parameter list using the standard call calling
  // convention.
  VIASTDDECL Call(uint32_t* result, void* params) = 0;
}; // class VIACaplFunction


// ============================================================================
// Access to environment variables
// ============================================================================


enum VIAEnvVarType   // the type of an environment variable
{
  kVIA_EVInteger = 0,
  kVIA_EVFloat = 1,
  kVIA_EVString = 2,
  kVIA_EVData = 3  // binary
};


//
// callback object for changes of the environment variable
//
class VIAOnEnvVar
{
public:
  VIASTDDECL OnEnvVar(VIATime nanoseconds, VIAEnvVar* ev) = 0;
};

// andre
class VIAOnMeasurementEvent
{
public:
  VIASTDDECL OnMeasurementEvent() = 0;
};


class VIAEnvVar
{
public:

  /**
  * Gets port type.
  *
  * @param pType on return, this will be the port type
  */
  VIASTDDECL GetType(uint32_t* pType) = 0;   // see enum VIAEnvVarType for type

  /**
  * Gets port name.
  *
  * @param pName on return, this will be the port name
  * @param bufferLength maximum number of bytes to be written
  */
  VIASTDDECL GetName(char* pName, int32_t bufferLength) = 0;

  /**
  * Sets port sink, a callback object for changes of the environment variable.
  *
  * @param pSink on return, this will be the port sink
  */
  VIASTDDECL SetSink(VIAOnEnvVar* pSink) = 0;

  /**
  * Writes numeric integer data to output port.
  *
  * @param x numeric data to be written
  */
  VIASTDDECL SetInteger(int32_t  x) = 0;

  /**
  * Gets numeric integer data from input port.
  *
  * @param pX on return, this will be the numeric input port value
  */
  VIASTDDECL GetInteger(int32_t* pX) = 0;

  /**
  * Writes numeric floating point data to output port.
  *
  * @param x numeric data to be written
  */
  VIASTDDECL SetFloat(double  x) = 0;

  /**
  * Gets numeric floating point data from input port.
  *
  * @param pX on return, this will be the numeric input port value
  */
  VIASTDDECL GetFloat(double* pX) = 0;

  /**
  * Sets output port string data.
  *
  * @param pText text data with objects text to be read
  * @param bufferLength maximum number of bytes to be read
  */
  VIASTDDECL SetString(const char* pText) = 0;

  /**
  * Gets input port string data by writing its bytes into text buffer of the max. specified length.
  *
  * @param pText on return, this will be the object's string
  * @param bufferLength maximum number of bytes to be written
  */
  VIASTDDECL GetString(char* pText, int32_t bufferLength) = 0;

  /**
  * Gets input port data by writing its bytes into buffer of the max. specified length.
  *
  * @param pData on return, this will be the object's raw data bytes
  * @param bufferLength maximum number of bytes to be written
  */
  VIASTDDECL GetData(uint8_t* pBuffer, int32_t bufferLength) = 0;

  /**
  * Sets output port data.
  *
  * @param pData binary data with objects raw byte to be read
  * @param bufferLength maximum number of bytes to be read
  */
  VIASTDDECL SetData(const uint8_t* pBuffer, int32_t bufferLength) = 0;

  // new functions to support arrays
  /**
  * Writes numeric integer data to output port.
  *
  * @param x numeric data to be written
  * @param row x-dimension of the element to be written
  * @param col y-dimension of the element to be written
  */
  VIASTDDECL SetInteger2(int32_t  x, int row, int col) = 0;

  /**
  * Gets numeric integer data from input port.
  *
  * @param pX on return, this will be the numeric input port value
  * @param row x-dimension of the element to be written
  * @param col y-dimension of the element to be written
  */
  VIASTDDECL GetInteger2(int32_t* x, int row, int col) = 0;

  /**
  * Writes numeric floating point data to output port.
  *
  * @param x numeric data to be written
  * @param row x-dimension of the element to be written
  * @param col y-dimension of the element to be written
  */
  VIASTDDECL SetFloat2(double x, int row, int col) = 0;

  /**
  * Gets numeric floating point data from input port.
  *
  * @param pX on return, this will be the numeric input port value
  * @param row x-dimension of the element to be written
  * @param col y-dimension of the element to be written
  */
  VIASTDDECL GetFloat2(double* pX, int row, int col) = 0;

  /**
  * Gets port dimensions.
  *
  * @param pRows number of rows
  * @param pCols number of columns
  */
  VIASTDDECL GetDimension(int* pRows, int* pCols) = 0;

  /**
  * Gets image data from an input port.
  *
  * @param pBuffer buffer where the image data is written
  * @param header info contains information about the dimensions and color format of a device-independent bitmap (DIB)
  */
  VIASTDDECL GetImageData(uint8_t* pBuffer, BITMAPINFOHEADER* bmi) = 0;

  /**
  * Sets image data to an output port.
  *
  * @param pBuffer image data buffer holds the image pixels
  * @param header info contains information about the dimensions and color format of a device-independent bitmap (DIB)
  */
  VIASTDDECL SetImageData(const uint8_t* pBuffer, BITMAPINFOHEADER* bmi) = 0;

  // VIA version 2.0 interface extension
  /**
  * Gets numeric data from input port.
  *
  * @param pX on return, this will be the numeric input port value
  * @param row x-dimension of the element to be written
  * @param col y-dimension of the element to be written
  * @param pTimeStamp time-stamp of the sample as sampled by the host (e.g. CANape)
  */
  VIASTDDECL GetFloatEx(double* pX, int row, int col, VIATime* pTimeStamp) = 0;

  /**
  * Gets input port data by writing its bytes into buffer of the max. specified length.
  *
  * @param pData on return, this will be the object's raw data bytes
  * @param bufferLength maximum number of bytes to be written
  * @param pNumberOfBytesWritten on return, this will be the actual size written into pData
  * @param pTimeStamp time-stamp of the object as sampled by the host (e.g. CANape)
  */
  VIASTDDECL GetDataEx(uint8_t* pBuffer, int32_t bufferLength, int32_t* pNumberOfBytesWritten, VIATime* pTimeStamp) = 0;

  /**
  * Sets output port data.
  *
  * @param pData binary data with objects raw byte to be read
  * @param bufferLength maximum number of bytes to be read
  * @param numberOfBytesWritten actual payload size for the current sample
  */
  VIASTDDECL SetDataEx(const uint8_t* pBuffer, int32_t bufferLength, int32_t numberOfBytesWritten) = 0;

  /**
   * Sets the corresponding port description to host port implementation.
   *
   * @param pDescription the port description as string, NULL in case a port does not provide any description
   * @param descriptionLength maximum number of bytes to be written
   * @param descriptionType type of the port description (0 defines a standard string description)
   */
  VIASTDDECL SetDescription(const char* pDescription, int32_t descriptionLength, int32_t descriptionType) = 0;

}; // class VIAEnvVar



// ============================================================================
// Interface to a node
// ============================================================================



class VIANode
{
public:
  // Get the name of the network node. The length of the name is limited to
  // 32 characters (without the termination null byte).
  VIASTDDECL GetName(char* buffer, int32_t bufferLength) = 0;

  // Get a CAPL function handle. The handle stays valid until end of
  // measurement or a call of ReleaseCaplFunction. This method is best
  // called at InitMeasurement.
  VIASTDDECL GetCaplFunction(VIACaplFunction** caplfct,     // the created handle
    const char* functionName) = 0;  // name of the function

  // Release the CAPL function handle received by 'GetCaplFunction' (see above)
  // Call this method at EndMeasurement for every CAPL function handle, that
  // you have asked for at InitMeasurement.
  VIASTDDECL ReleaseCaplFunction(VIACaplFunction* caplfct) = 0;
}; // class VIANodeHandle



// ============================================================================
// class VIAService
// ============================================================================



class VIAService
{
public:

  // =============== Configuration items =====================================


  // Version of this implementation of VIAService
  VIASTDDECL GetVersion(int32_t* major, int32_t* minor, int32_t* patchlevel) = 0;

  // Get the windows handle for the main window of canoe. You can use
  // this as the parent handle for windows message boxes.
  VIASTDDECL GetClientWindow(HWND* handle) = 0;


  // With 'GetConfigItem(...)' you can ask CANoe much about its configuration
  // like used databases, filename of the configuration, etc. You specify
  // the item by a main topic and a sub topic. The topics are enumerated below
  // (see enum VIA_ConfigTopic). The meaning of sub topic depends on the main
  // topic. But often the sub topic has no special meaning and should be
  // set to zero in this case.
  enum VIA_ConfigTopic {
    kVIA_ConfigurationPath = 1, // CANoe's configuration file (no subtopic)
    kVIA_IniFile = 2, // the used init file (no subtopic)
    kVIA_SysPath = 3, // (?)
    kVIA_UsrPath = 4, // (?)
    kVIA_DatabaseName = 5 // name used as qualifiers of database or an
    // empty string, if not available
    // Subtopic enumerates the databases [0,n-1].
  };

  VIASTDDECL GetConfigItem(uint32_t topic,             // main topic
                           uint32_t subtopic,          // sub topic
                           char*    buffer,            // buffer for the result
                           int32_t  bufferLength) = 0; // size of buffer


  // =============== Interface to database ===================================


  enum VIA_DBObject {           // object type for a ...
    kVIA_DBDatabase = 151,    // ...  database attribute
    kVIA_DBNode = 152,    // ...  node attribute
    kVIA_DBMessage = 153,    // ...  message attribute
    kVIA_DBSignal = 154,    // ...  signal attribute
    kVIA_DBEnvVar = 155     // ...  environment variable attribute
  };

  enum VIA_DBType {
    kVIA_DBFloat = 160,    // attribute type for attributes of type Float or Integer
    kVIA_DBString = 165     // attribute type for attributes of type String
  };

  // Get the Type (kVIA_DBFloat or kVIA_DBString) of a attribute
  VIASTDDECL GetDBAttributeType(uint32_t*     attributeType, // the requested type (enum VIA_DBType)
    uint32_t      objectType,    // see enum VIA_DBObject
    const char* objectName,    //
    const char* attrName,      // name of attribute
    const char* dbName = NULL) = 0;// database name

  // Get the value of an attribute of type kVIA_DBFloat
  VIASTDDECL GetDBAttributeValue(double*     attributeValue,  // the requested value
    uint32_t      objectType,      // see enum VIA_DBObject
    const char* objectName,      //
    const char* attrName,        // name of attribute
    const char* dbName = NULL) = 0; // database name

  // Get the value of an attribute of type kVIA_DBString
  VIASTDDECL GetDBAttributeString(char*       buffer,          // buffer to copy the requested string
    int32_t       bufferLength,    // size of buffer
    uint32_t      objectType,      // see enum VIA_DBObject
    const char* objectName,      //
    const char* attrName,        // name of attribute
    const char* dbName = NULL) = 0; // database name


  // =============== Interface to timer ======================================


  // Create a timer object. If a node is specified, the timer is
  // affected by the drift and jitter used by this node.
  VIASTDDECL CreateTimer(VIATimer**      timer,    // the created object
    VIANode*        node,     // network node
    VIAOnTimerSink* sink,     // callback object (NULL allowed)
    const char*     name) = 0; // name of the timer

  // release the timer, so that VIAService can delete the object
  VIASTDDECL ReleaseTimer(VIATimer* timer) = 0;


  // =============== Interface to environmet variables  ======================


  // Access to environment variables, GetEnvVar(...) creates an interface object
  // for a single environment variable
  VIASTDDECL GetEnvVar(VIAEnvVar**  ev,      // the created object
    VIANode*     node,    // node
    const char*  name,    // name of the variable
    VIAOnEnvVar* sink) = 0; // callback object (NULL allowed)

  // Release the object created by GetEnvVar(...)
  VIASTDDECL ReleaseEnvVar(VIAEnvVar* ev) = 0;


  // =============== Interface to bus ========================================


  // Get an interface object to a bus system. The type of the interface is
  // specified by the parameter interfaceType. At the moment, only kVIA_CAN
  // is supported.
  VIASTDDECL GetBusInterface(VIABus** busInterface,    // the interface object
    VIANode* node,            // node, which will use the interface
    uint32_t   interfaceType,   // see enum VIABusInterfaceType
    int32_t    majorversion,    // the required version of
    int32_t    minorversion) = 0; //   the bus interface

  // Release the object created by 'GetBusInterface(...)'
  VIASTDDECL ReleaseBusInterface(VIABus* busInterface) = 0;


  // =============== utility services=========================================

  VIASTDDECL GetUtilService(VIAUtil** service,        // the service object
    int32_t     majorversion,      // the required version
    int32_t     minorversion) = 0; // the util service

  VIASTDDECL ReleaseUtilService(VIAUtil* service) = 0;


  // =============== misc ====================================================


  // Output text to the "Write Window"
  VIASTDDECL WriteString(const char* text) = 0;

  // Output text to a logging file
  VIASTDDECL WriteToLog(const char* text) = 0;

  // Popup the well known assertion box of CANalyzer/CANoe
  VIASTDDECL Assertion(char* message, char* condition, char* file, int32_t line) = 0;

  // Popup a Message Box
  VIASTDDECL MsgBox(char* message) = 0;

  // Status of the current Measurement,
  // returns kVIA_OK or kVIA_ServiceNotRunning
  VIASTDDECL RtKernelIsRunning() = 0;


  // Use GetCurrentNode() within a call from a capl function to get the
  // the node, which called the function.
  VIASTDDECL GetCurrentNode(VIANode** node) = 0;


  // Use GetCurrentNodeLayer() within a call from a capl function to get the
  // nodelayer object of this given module, belonging to the current node
  VIASTDDECL GetCurrentNodeLayer(VIANodeLayerApi** nodelayer,  // current nodelayer
    VIAModuleApi* module) = 0;     // module

  // Timestamp of the event, that is processed at the moment (ringing timer,
  // bus event, environment variable was set, ...)
  VIASTDDECL GetCurrentSimTime(VIATime* time) = 0;

  // Stop the current measurement
  VIASTDDECL Stop() = 0;

  // Something very important for the OSEK-Emulation
  VIASTDDECL GetSystemFiber(LPVOID* fiber) = 0;

  // new function to support arrays
  VIASTDDECL GetEnvVar2(VIAEnvVar**  ev,      // the created object
    VIANode*     node,    // node
    const char*  name,    // name of the variable
    int          rows,
    int          cols,
    VIAOnEnvVar* sink) = 0; // callback object (NULL allowed)

}; // class VIAService




// ==========================================================================
// class VIANodeLayerApi
//
// Interface of the nodelayer, which is called by the runtime kernel.
// This class must be implemented by the nodelayer-DLL.
// ==========================================================================



class VIANodeLayerApi
{
public:
  // Get the Node, to which this Layer belongs. This should be the handle,
  // that was provided by the call of VIAModuleApi::CreateObject(...)
  VIASTDDECL GetNode(VIANode** node) = 0;

  VIASTDDECL InitMeasurement() = 0;
  VIASTDDECL StartMeasurement() = 0;
  VIASTDDECL StopMeasurement() = 0;
  VIASTDDECL EndMeasurement() = 0;
}; // class VIANodeLayerApi



// ==========================================================================
// class VIAModuleApi
//
// Interface of the module, which is called by the runtime kernel.
//
// This class, together with the functions VIAGetModuleApi and
// VIAReleaseModuleApi, must be implemented by the nodelayer-DLL.
// ==========================================================================



class VIAModuleApi
{
public:
  // Initialize the nodelayer. Here you can do additional setup of the DLL.
  // CANoe calls this method after it ask for the object (function
  // 'VIAGetModuleApi') and before it calls any other method of this object.
  VIASTDDECL Init() = 0;

  // The text provided by GetVersion(...) is displayed in the info window
  // This is normally the name and version of this DLL.
  VIASTDDECL GetVersion(char* buffer, int32_t bufferLength) = 0;

  // With a Shift-DoubleClick in the Info-Window, the following information
  // is displayed in the Write-Window.
  // pathBuff: filename and path of this nodelayer
  // versionBuff: name and versionnumber of this nodelayer
  VIASTDDECL GetModuleParameters(char* pathBuff, int32_t pathBuffLength,
                                 char* versionBuff, int32_t versionBuffLength) = 0;

  // The information provided by GetNodeInfo(...) appears within the node
  // at the simulation setup window (shortName) and the status indication
  // line of CANoe (longName) .
  VIASTDDECL GetNodeInfo(const char* nodename,      // name of the node
                         char* shortNameBuf, int32_t shortBufLength,
                         char* longNameBuf, int32_t longBufLength) = 0;


  // Create a nodelayer object for the given node
  VIASTDDECL CreateObject(VIANodeLayerApi** object,    // the created instance
                          VIANode* node,               // the node
                          int32_t argc, char* argv[]) = 0;  // additional parameters

  // Release the given nodelayer object
  VIASTDDECL ReleaseObject(VIANodeLayerApi*  object) = 0;


  VIASTDDECL InitMeasurement() = 0;
  VIASTDDECL EndMeasurement() = 0;
}; // class VIAModuleApi


// ==========================================================================
// setup functions
//
// The following C-functions are for loading, unloading and initializing
// of the DLL. 
// They are called just after loading or just before unloading of the DLL.
// ==========================================================================

// CANoe calls the function 'VIARequiredVersion' to ask the nodelayer DLL,
// with which version of the VIA-interface it was compiled.
// This is the first function, that CANoe calls after loading the DLL.
VIACLIENT(void) VIARequiredVersion(int32_t* majorversion, int32_t* minorversion);


// After CANoe has asked, which version of the via-service the DLL expects,
// it assigns a compatible version of the service to the DLL.
// This service object stays valid until the DLL is unloaded or this function
// is called again. This is the second function, that CANoe calls after loading
// the DLL.
VIACLIENT(void) VIASetService(VIAService* service);



// VIAGetNodeLayerApi is called by the runtime kernel to get the
// interface of the module.
// Parameter argc, argv: Additional parameters for multiservice modules
//                       This parameters are ignored by most nodelayers
// Note: The interface-object must not be destroyed by the module
// before VIAReleaseNodeLayerApi() is called. This is the third function
// that CANoe calls on the nodelayer DLL. It is called after CANoe has
// initialized the DLL with a VIA-Service object.
VIACLIENT(VIAModuleApi*) VIAGetModuleApi(int32_t argc, char* argv[]);



// VIAReleaseNodeLayerApi is called by the runtime kernel, when the
// interface of the nodelayer isn't needed any longer.
// This function is called just before the DLL will be unloaded.
VIACLIENT(void) VIAReleaseModuleApi(VIAModuleApi* api);



// ==========================================================================
// struct VIAMetaInfo
//
// Function Meta-Information.
// Plain data without functionality.
// ==========================================================================
struct VIAMetaInfo
{
  const char* Name;                     /* model name                 */
  const char* OriginalName;             /* original model name        */
  const char* ModelVersion;             /* model version              */
  const char* Description;              /* model description          */
  const char* Creator;                  /* initial user/creator       */
  const char* LastModifiedBy;           /* last user                  */
  const char* Compiler;                 /* MSVC or MinGW64            */
  uint16_t    CompilerVersion;          /* MSVC or MinGW64 version    */
  const char* GeneratedOn;              /* creation time              */
  bool        IsMultiRate;              /* multi sample rate          */
  bool        IsDebugBuild;             /* debug build                */
  // Matlab/Simulink specific meta information
  struct VIASimulinkMetaInfo
  {
    const char* MatlabVersion;          /* Matlab version             */
    const char* SimulinkVersion;        /* Simulink version           */
    const char* Target;                 /* MSVC or MinGW STF          */
    const char* TargetVersion;          /* Target version             */
  } SimulinkMetaInfo;
};

// VIAGetMetaInfo is called by the host in order to retrieve function meta-information.
VIACLIENT(VIAMetaInfo*) VIAGetMetaInfo();