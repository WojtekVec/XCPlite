/*----------------------------------------------------------------------------
| File: nlapml_v.h
| Project: Vector CANape/vSignalyzer C-RT DLL Project
|
| Description: CANape/vSignalyzer Function DLL C-RT Interface
|
|-----------------------------------------------------------------------------
| $Author: vislmk $    $Locker: $   $Revision: 5798 $
|-----------------------------------------------------------------------------
| Copyright (c) by Vector Informatik GmbH.  All rights reserved.
 ----------------------------------------------------------------------------*/
#pragma once

#define NLAPML_BUILD_NUMBER             10
#define NLAPML_BUILD_NUMBER_STRING     "10"

#define NLAPML_FILE_VERSION             1,0,NLAPML_BUILD_NUMBER,0
#define NLAPML_FILE_VERSION_STRING     "1.0." NLAPML_BUILD_NUMBER_STRING "\000"

#define NLAPML_INTERNAL_NAME           "Vector CANape Function DLL C-RT Interface"
#define NLAPML_EXTERNAL_NAME            NLAPML_INTERNAL_NAME " " NLAPML_FILE_VERSION_STRING

#define NLAPML_PRODUCT_VERSION          9,0,0,0
#define NLAPML_PRODUCT_VERSION_STRING  "9.0\000"
