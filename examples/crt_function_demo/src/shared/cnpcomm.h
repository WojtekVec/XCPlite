/*----------------------------------------------------------------------------
| File: cnpcomm.h
|
| Project: Vector MATLAB/Simulink MC Add-on CANape/vSignalyzer Target
|          and Vector CANape/vSignalyzer C-RT DLL Project
|
| Description:
|
|    CANape/vSignalyzer communication layer header file
|
|-----------------------------------------------------------------------------
| $Author: vislmk $    $Locker: $   $Revision: 8039 $
|-----------------------------------------------------------------------------
| Copyright (c) by Vector Informatik GmbH.  All rights reserved.
 ----------------------------------------------------------------------------*/
#pragma once

#include <windows.h> /* needed for BITMAPINFOHEADER */

#include "via/mbd_types.h"

#if defined __cplusplus
extern "C" {
#endif

const void* cnCreateEnvironmentInputVariable( const char* name, const void* group);
const void* cnCreateEnvironmentOutputVariable( const char* name, const void* group);
const void* cnCreateEnvironmentEventVariable( const char* name, void* pfn, void* s);
const void* cnCreateMeasurementStartEvent( void* pfn, void* s);
const void* cnCreateMeasurementStopEvent( void* pfn, void* s);
void        cnReleaseEnvironmentVariable( const void* var);
double      cnGetEnvironmentValue( const void* var);
void        cnSetEnvironmentValue( const void* var, double val);
void        cnGetEnvironmentValueString( const void* var, char* val, int32_t len);
void        cnSetEnvironmentValueString( const void* var, const char* val);
void        cnGetEnvironmentValues( const void* group, int32_t count, double* values);
void        cnSetEnvironmentValues( const void* group, int32_t count, double* values);
const void* cnCreateEnvironmentGroup();
void        cnReleaseEnvironmentGroup( const void* group);
const void* cnCreateEnvironmentTrigger( void* pfn, void* s);
void        cnReleaseEnvironmentTrigger( const void* trigger);

void        cnSetEnable( int32_t timer, int32_t events);
void        cnGetEnable( int32_t* timer, int32_t* events);

const void* cnCreateSimulation();
void        cnReleaseSimulation( const void* sim);
void        cnIncreaseTime( const void* sim, double time);

const void* cnCreateCAPLHandler( const char* name);
void        cnReleaseCAPLHandler( const void* handler);
void        cnCallCAPLHandler( const void* handler);

void        cnFatal( const char* msg);

void        cnSetBaseTimer( void ( *function)(), double timeout);
void        cnCreateTimer( void ( *function)(), double timeout);

double      cnGetCurrentTime();

const void* cnCreateEnvironmentInputVariable2( const char* name, int rows, int cols, const void* group);
const void* cnCreateEnvironmentOutputVariable2( const char* name, int rows, int cols, const void* group);
double      cnGetEnvironmentValue2( const void* var, int row, int col);
void        cnSetEnvironmentValue2( const void* var, double val, int row, int col);
void        cnGetEnvironmentDimension( const void* var, int* row, int* col);
void        cnGetEnvironmentData(const void* var, int8_t* data, BITMAPINFOHEADER* bmi);
void        cnSetEnvironmentData(const void* var, int8_t* data, BITMAPINFOHEADER* bmi);

void        cnGetEnvironmentBinaryData(const void* var, uint8_t* data, int size);
void        cnSetEnvironmentBinaryData(const void* var, const uint8_t* data, int size);

/* VIA interface extension */
double      cnGetEnvironmentValueEx(const void* var, int row, int col, int64_t* pTimeStamp);
void        cnGetEnvironmentBinaryDataEx(const void* var, uint8_t* pData, int32_t dataSize, int32_t* pNumberOfBytesWritten, int64_t* pTimeStamp);
void        cnSetEnvironmentBinaryDataEx(const void* var, const uint8_t* pData, int32_t dataSize, int32_t numberOfBytesWritten);

void        cnSetDescription(const void* var, const char* pDescription, int32_t descriptionLength, int32_t descriptionType);


#if defined __cplusplus
}
#endif
