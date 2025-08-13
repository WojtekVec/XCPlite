/*----------------------------------------------------------------------------
|     Project: Vector CANape/vSignalyzer C-RT DLL Project
|
|        File: calculate.h
|
| Description:
|
|-----------------------------------------------------------------------------
| $Rev:: 6712        $:  Revision of last commit
| $Author:: viswp    $:  Author of last commit
| $Date:: 2023-06-21#$:  Date of last commit
|-----------------------------------------------------------------------------
| Copyright (c) by Vector Informatik GmbH.  All rights reserved.
-----------------------------------------------------------------------------*/
#pragma once

#include <windows.h> //needed for BITMAPINFOHEADER

#include "via/via.h"

#include <map>
#include <vector>
#include <string>

#include <cstdint>

// forward declarations
class VIAService;

enum ePortType
{
  kNumeric = 0,     // standard numeric port
  kImageProcessing, // image processing port
  kBinary,          // binary data port
};

enum eSampleTimeType
{
  kFundamental = 0,
  kCustom, // custom sample-time if port is written from the calculate function
};

struct sUserVar
{
  ePortType   portType;
  const char* name;
  int         rows;
  int         cols;
};


/**
  * Base class for the three different types of variables: Variable, BinaryVariable (binary data) and IPVariable (image processing)
  */
class VariableBase
{

public:
  VariableBase();
  VariableBase(const char* name, int rows, int cols);
  virtual ~VariableBase();

  /**
   * Returns 1 for a scalar variable, the number of elements for arrays, 
   * number of pixels*3 (RGB) for image arrays and buffer size for binary ports.
   */
  virtual int    GetSize() const { return mSize; };

  /**
   * Returns the unique name of the variable as specified by programmer.
   */
  const char*    GetName() const { return mName; };

  /**
   * Returns 1 for scalar variable and the number of rows for arrays.
   */
  int            GetRows() const { return mRows; };

  // Returns 1 for scalar variable and the number of cols for arrays.
  int            GetCols() const { return mCols; };

  /**
   * Returns the corresponding sample rate type.
   */
  eSampleTimeType GetSampleTimeType() const { return mSampleTimeType; }

  /**
   * Sets the specified sample rate type.
   */
  void           SetSampleTimeType(eSampleTimeType sampleRate) { mSampleTimeType = sampleRate; }

  /**
   * Returns the value for the specified position. For scalar variables position is not needed.
   * In case of row/col selection during image processing three bytes RGB value is returned.
   */
  virtual double GetValue(unsigned int index = 0) { return 0; };

  /**
   * Returns the value for the specified position. For scalar variables position is not needed.
   * In case of row/col selection during image processing three bytes RGB value is returned.
   */
  virtual double GetValue(int row, int col) { return 0; };

  /**
   * Sets the value at the specified position. Call without position is same like index 0.
   * In case of row/col selection during image processing value must be three bytes RGB value
   */
  virtual void   SetValue(double value) { SetValue(0, value); };

  /**
   * Sets the value at the specified position. Call without position is same like index 0.
   * In case of row/col selection during image processing value must be three byte RGB value
   */
  virtual void   SetValue(unsigned int index, double value) {};

  /**
   * Sets the value at the specified position. Call without position is same like index 0.
   * In case of row/col selection during image processing value must be three byte RGB value
   */
  virtual void   SetValue(int row, int col, double value) {};

  /**
   * Returns a pointer to the array variable. Don't use with scalar variables.
   */
  virtual int8_t* GetValueBuffer() = 0;

  /**
   * Gets binary data with the specified length.
   */
  virtual void   GetData(uint8_t* data, int32_t dataLength) {}

  /**
   * Sets binary data with the specified length.
   */
  virtual void   SetData(const uint8_t* buffer, int32_t bufferLength) {}

  /**
   * Sets a port scalar value directly to the mapped host (CANape/vSignalyzer) object.
   */
  virtual void   SetPortValue(double value) {};

  /**
   * Returns a pointer to the binary data buffer.
   * Only for binary ports.
   */
  virtual uint8_t* GetDataBuffer() = 0;

  /**
   * Clears the underlying port buffer.
   */
  virtual void   ClearBuffer() {};

  // enhanced VIA interface functions
  virtual double GetValueEx(VIATime* pTimeStamp) { return 0.0; }
  virtual double GetValueEx(int row, int col, VIATime* pTimeStamp) { return 0.0; }
  virtual void   GetDataEx(uint8_t* pBuffer, int32_t bufferLength, int32_t* pNumberOfBytesWritten, VIATime* pTimeStamp) {}
  virtual void   SetDataEx(const uint8_t* buffer, int32_t bufferLength, int32_t numberOfBytesWritten) {}
  virtual void   SetDescription(const char* pDescription, int32_t descriptionLength, int32_t descriptionType) {}

protected:
  const void*    GetEnvVar() { return mpEnvVar; };
  void           SetEnvVar(const void* envVar) { mpEnvVar = envVar; };
  virtual void   GetEnvValues(){};
  virtual void   SetEnvValues(){};

  friend class VNLAPMLLayer;

protected:
  // name of the port/variable
  const char*     mName;
  // number of rows
  int             mRows;
  // number of columns
  int             mCols;
  // total port size
  int             mSize;
  // raw point to the corresponding host environment port/variable
  const void*     mpEnvVar;
  // port/variable type
  ePortType       mPortType;
  // sample rate: fundamental or custom
  eSampleTimeType mSampleTimeType;
};

/**
* Numeric variable type for processing numeric scalar or multi dimensional in-/output values.
*/
class Variable : public VariableBase
{

public:
  Variable();
  Variable(const char* name, int rows, int cols);
  virtual ~Variable();

  virtual double  GetValue(unsigned int index = 0);
  virtual double  GetValue(int row, int col);
  virtual void    SetValue(unsigned int index, double value);
  virtual void    SetValue(int row, int col, double value);
  virtual void    SetValues(const double *pValue, int size);
  virtual int8_t* GetValueBuffer();
  virtual void    SetPortValue(double value);
  virtual uint8_t* GetDataBuffer() { return nullptr; }

  // enhanced VIA interface function, which provides the corresponding time-stamp
  virtual double  GetValueEx(VIATime* pTimeStamp);
  virtual double  GetValueEx(int row, int col, VIATime* pTimeStamp);

private:
  virtual void    GetEnvValues();
  virtual void    SetEnvValues();

private:
  double              mValue;
  std::vector<double> mVValues;
};

/**
* Binary variable type for processing raw bytes/data streams.
*/
class BinaryVariable : public VariableBase
{
public:
  static const int kDefaulSize = 16384;
  static const int kZeroSize = 0;
  BinaryVariable(const char* name, int bufferSize);
  virtual ~BinaryVariable();

  virtual void    GetData(uint8_t* pBuffer, int32_t bufferLength);
  virtual void    SetData(const uint8_t* pBuffer, int32_t bufferLength);
  virtual int8_t* GetValueBuffer() { return nullptr; }
  virtual uint8_t* GetDataBuffer() { return mpBuffer; }
  virtual int     GetSize() const { return mBufferSize; };
  virtual void    ClearBuffer();

  virtual void    GetDataEx(uint8_t* pBuffer, int32_t bufferLength, int32_t* pNumberOfBytesWritten, VIATime* pTimeStamp);
  virtual void    SetDataEx(const uint8_t* pBuffer, int32_t bufferLength, int32_t numberOfBytesWritten);
  virtual void    SetDescription(const char* pDescription, int32_t descriptionLength, int32_t descriptionType);

  /**
   * Returns true if this variable value cache is dirty and must be written to host's port.
   * False otherwise.
   */
  inline  bool    IsDirty() const { return mDirty; }

private:
  virtual void    GetEnvValues();
  virtual void    SetEnvValues();
private:
  // buffer
  uint8_t* mpBuffer;
  const int mBufferSize;
  // data dirty?
  bool mDirty;
};


/**
* "ImageProcessing" variable type whose array in-/output values shall be interpreted as images
*/
class VariableIP : public VariableBase
{
public:
  VariableIP();
  VariableIP(const char* name, int rows, int cols);
  virtual ~VariableIP();

  virtual double  GetValue(unsigned int index = 0);
  virtual double  GetValue(int row, int col);
  virtual void    SetValue(unsigned int index, double value);
  virtual void    SetValue(int row, int col, double rgbValue);
  virtual int8_t* GetValueBuffer() { return (int8_t*)&mVValues[0]; };
  virtual uint8_t* GetDataBuffer() { return nullptr; }

private:
  virtual void    GetEnvValues();
  virtual void    SetEnvValues();

private:
  std::vector<uint8_t> mVValues;
  BITMAPINFOHEADER     mBmi;
};
