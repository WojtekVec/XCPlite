/*----------------------------------------------------------------------------
| File: calculateHidden.cpp
| Project: Vector CANape/vSignalyzer C-RT DLL Project
|
| Description:
|
|-----------------------------------------------------------------------------
| $Author: viswp $    $Locker: $   $Revision: 6757 $
|-----------------------------------------------------------------------------
| Copyright (c) by Vector Informatik GmbH.  All rights reserved.
----------------------------------------------------------------------------*/
#pragma warning( disable:4786)

#include "variable.h"
#include "cnpcomm.h"
#include "via/via.h"

#pragma region VariableBase

/**
* Creates an object of the VariableBase class.
*/
VariableBase::VariableBase()
  : mPortType(kNumeric)
  , mName("")
  , mRows(1)
  , mCols(1)
  , mSize(1)
  , mpEnvVar(NULL)
  , mSampleTimeType(kFundamental)
{
}

/**
* Creates an object of the VariableBase class using the specified name and dimensions.
*/
VariableBase::VariableBase(const char* name, int rows, int cols)
  : mPortType(kNumeric)
  , mName(name)
  , mRows(rows)
  , mCols(cols)
  , mSize(rows*cols)
  , mpEnvVar(NULL)
  , mSampleTimeType(kFundamental)
{
}

/**
* Deletes this object.
*/
VariableBase::~VariableBase()
{
}
#pragma endregion // VariableBase

#pragma region Variable

/**
* The VariableBase class represents a scalar or multi-dimensional numeric input/output CANape port.
*/
Variable::Variable()
  : VariableBase()
  , mValue(0)
{
}

Variable::Variable(const char* name, int rows, int cols)
  : VariableBase(name, rows, cols)
  , mValue(0)
{
  //allocate memory for non-scalar values
  if (mSize > 1)
  {
    mVValues.resize(GetSize());
  }
}

Variable::~Variable()
{
  mVValues.clear();
}

/**
* Sets the value specified by index.
*/
void Variable::SetValue(unsigned int index, double value)
{
  if (mSize > 1)
  {
    if (index > mVValues.size())
    {
      index = (unsigned int)mVValues.size();
    }
    mVValues[index] = value;
  }
  else
  {
    mValue = value;
  }
}

/**
* Sets the value specified by row and col.
*/
void Variable::SetValue(int row, int col, double value)
{
  int index = col * mRows + row;
  SetValue(index, value);
}

/**
* Sets the specified number of values.
*/
void Variable::SetValues(const double *pValue, int size)
{
  for (int i = 0; i < size; i++)
  {
    mVValues[i] = pValue[i];
  }
}

/**
* Gets double value specified by index.
*/
double Variable::GetValue(unsigned int index)
{
  if (mSize > 1)
  {
    if (index > mVValues.size())
    {
      return mVValues.back();
    }
    return mVValues[index];
  }
  return mValue;
}

/**
* Gets double value specified by row and column.
*/
double Variable::GetValue(int row, int col)
{
  int index = col * mRows + row;
  return GetValue(index);
}

/**
* Gets the corresponding values.
*/
void Variable::GetEnvValues()
{
  if (mCols > 1 || mRows > 1)
  {
    unsigned int row, col;
    int num = mCols * mRows;
    for (int n = 0; n < num; n++)
    {
      col = n / mRows;
      row = n - (mRows*col);
      SetValue(n, cnGetEnvironmentValue2(mpEnvVar, row, col));
    }
  }
  else
  {
    SetValue(0, cnGetEnvironmentValue2(mpEnvVar, 0, 0));
  }
}

/**
* Gets the corresponding value for the specified object element and sets the corresponding time stamp.
*/
double Variable::GetValueEx(VIATime* pTimeStamp)
{
  return GetValueEx(0, 0, pTimeStamp);
}


/**
* Gets the corresponding value for object element specified by row and col and sets the corresponding time stamp.
*/
double Variable::GetValueEx(int row, int col, VIATime* pTimeStamp)
{
  int index = col * mRows + row;
  SetValue(index, cnGetEnvironmentValueEx(mpEnvVar, row, col, pTimeStamp));
  return GetValue(index);
}


/**
* Sets the corresponding values of the mapped object.
*/
void Variable::SetEnvValues()
{
  if (mCols > 1 || mRows > 1)
  {
    unsigned int row, col;
    int num = mCols * mRows;
    for (int n = 0; n < num; n++)
    {
      //row = i/cols;
      //col = i-(cols*row);
      col = n / mRows;
      row = n - (mRows*col);
      cnSetEnvironmentValue2(mpEnvVar, GetValue(n), row, col);
    }
  }
  else
  {
    cnSetEnvironmentValue2(mpEnvVar, GetValue(), 0, 0);
  }
}


/**
* Gets value buffer as byte.
*/
int8_t* Variable::GetValueBuffer()
{
  if (mSize > 1)
  {
    return (int8_t*)&mVValues[0];
  }
  return 0;
}

/// SetPortValue writes the specified value directly to the mapped CANape variable.
/// The port's sample-rate must be set to 'kCustom'.
void Variable::SetPortValue(double value)
{
  mValue = value;
  if (GetSampleTimeType() == kCustom)
  {
    cnSetEnvironmentValue2(mpEnvVar, GetValue(), 0, 0);
  }
}

#pragma endregion // Variable

#pragma region BinaryVariable

/**
* The BinaryVariable class represents a binary input/output CANape port specialized in raw/uninterpreted data bytes.
*/
BinaryVariable::BinaryVariable(const char* name, int bufferSize)
  : VariableBase(name, bufferSize, -bufferSize)
  , mBufferSize(bufferSize)
  , mpBuffer(NULL)
  , mDirty(true)
{
  mPortType = kBinary;
  mpBuffer = new uint8_t[bufferSize];
  memset(mpBuffer, 0, bufferSize);
  // overwrite generic base class size with buffer size
  mSize = bufferSize;
}

/**
* The destructor of the BinaryVariable class deletes the previously allocated buffer memory.
*/
BinaryVariable::~BinaryVariable()
{
  delete[] mpBuffer;
  mpBuffer = NULL;
}

/**
* Copies this port's data buffer into the specified destination buffer by checking/copying maximum bufferSize number of bytes (read access).
*/
void BinaryVariable::GetData(uint8_t* pBuffer, int32_t bufferLength)
{
  if (mpBuffer != NULL)
  {
    const int32_t size = bufferLength > mBufferSize ? mBufferSize : bufferLength;
    memcpy(pBuffer, mpBuffer, size);
  }
}

/**
* Copies the specified data into this port's data buffer (write access).
*/
void BinaryVariable::SetData(const uint8_t* pBuffer, int32_t bufferLength)
{
  if (mpBuffer != NULL)
  {
    const int32_t size = bufferLength > mBufferSize ? mBufferSize : bufferLength;
    memcpy(mpBuffer, pBuffer, size);
  }
}

/**
* Clears the corresponding buffer.
*/
void BinaryVariable::ClearBuffer()
{
  if (mpBuffer != NULL)
  {
    memset(mpBuffer, 0, mBufferSize);
  }
}

/**
* Gets input port data via the corresponding mapping from CANape.
*/
void BinaryVariable::GetEnvValues()
{
  if (IsDirty())
  {
    cnGetEnvironmentBinaryData(mpEnvVar, mpBuffer, mBufferSize);
  }
}

/**
* Sets output port data via the corresponding mapping to CANape.
*/
void BinaryVariable::SetEnvValues()
{
  if (IsDirty())
  {
    cnSetEnvironmentBinaryData(mpEnvVar, mpBuffer, mBufferSize);
  }
}

/**
* Gets input port data, the number of mapped objects bytes and the associated time-stamp via the corresponding mapping from CANape.
*/
void BinaryVariable::GetDataEx(uint8_t* pBuffer, int32_t bufferLength, int32_t* pNumberOfBytesWritten, VIATime* pTimeStamp)
{
  cnGetEnvironmentBinaryDataEx(mpEnvVar, pBuffer, bufferLength, pNumberOfBytesWritten, pTimeStamp);
  // cache buffer
  if (mpBuffer != NULL)
  {
    const int32_t size = bufferLength > mBufferSize ? mBufferSize : bufferLength;
    memcpy(mpBuffer, pBuffer, size);
  }
  // data already written to host's port
  mDirty = false;
}

/**
* Sets output port data via the corresponding mapping to CANape.
*/
void BinaryVariable::SetDataEx(const uint8_t* pBuffer, int32_t bufferLength, int32_t numberOfBytesWritten)
{
  cnSetEnvironmentBinaryDataEx(mpEnvVar, pBuffer, bufferLength, numberOfBytesWritten);
  // cache buffer
  if (mpBuffer != NULL)
  {
    const int32_t size = bufferLength > mBufferSize ? mBufferSize : bufferLength;
    memcpy(mpBuffer, pBuffer, size);
  }
  // data already written to host's port
  mDirty = false;
}

/**
* Sets the specified description to CANape port variable implementation counterpart.
*/
void BinaryVariable::SetDescription(const char* pDescription, int32_t descriptionLength, int32_t descriptionType)
{
  cnSetDescription(mpEnvVar, pDescription, descriptionLength, descriptionType);
}


#pragma endregion // BinaryVariable

#pragma region VariableIP

/**
* The VariableIP class represents an image-processing input/output CANape port.
*/
VariableIP::VariableIP()
  : VariableBase()
{
  mPortType = kImageProcessing;
}

VariableIP::VariableIP(const char* name, int rows, int cols)
  : VariableBase(name, rows, cols)
{
  mPortType = kImageProcessing;

  memset(&mBmi, 0, sizeof(mBmi));
  mBmi.biSize = sizeof(mBmi);
  mBmi.biPlanes = 1;      // must be set to 1
  mBmi.biBitCount = 24;
  mBmi.biHeight = rows;   // If biHeight is positive, the bitmap is a bottom-up DIB and its origin is the lower-left corner (standard).
  mBmi.biWidth = cols;
  mBmi.biCompression = BI_RGB;
  mBmi.biSizeImage = 0;      //for BI_RGB

  mSize *= 3;
  mVValues.resize(mSize);
}

VariableIP::~VariableIP()
{
  mVValues.clear();
}

double VariableIP::GetValue(unsigned int index)
{
  if (index > mVValues.size())
  {
    return mVValues.back();
  }
  return mVValues[index];
}

double VariableIP::GetValue(int row, int col)
{
  int index = row * mCols * 3 + col * 3;
  unsigned char blue = (unsigned char)GetValue(index);
  unsigned char green = (unsigned char)GetValue(index + 1);
  unsigned char red = (unsigned char)GetValue(index + 2);
  double rgbVal = (red << 16) | (green << 8) | blue;
  return rgbVal;
}

void VariableIP::SetValue(unsigned int index, double value)
{
  if (index > mVValues.size())
  {
    index = (unsigned int)mVValues.size();
  }
  mVValues[index] = (unsigned char)value;
}

void VariableIP::SetValue(int row, int col, double rgbValue)
{
  unsigned char red = ((int)rgbValue & 0xFF0000) >> 16;
  unsigned char green = ((int)rgbValue & 0x00FF00) >> 8;
  unsigned char blue = ((int)rgbValue & 0x0000FF);
  int index = row * mCols * 3 + col * 3;
  mVValues[index + 0] = blue;
  mVValues[index + 1] = green;
  mVValues[index + 2] = red;
}

void VariableIP::GetEnvValues()
{
  int8_t* ptr = GetValueBuffer();
  cnGetEnvironmentData(mpEnvVar, ptr, &mBmi);
}

void VariableIP::SetEnvValues()
{
  int8_t* ptr = GetValueBuffer();
  cnSetEnvironmentData(mpEnvVar, ptr, &mBmi);
}
#pragma endregion // VariableIP
