/*----------------------------------------------------------------------------
|     Project: Vector CANape/vSignalyzer C-RT DLL Project
|
|        File: metainfo.h
|
| Description: CANape/vSignalyzer Function Meta Information.
|              The descriptive information about this function.
|              It is used for detailed description and identification.
|
|-----------------------------------------------------------------------------
| $Rev:: 6250        $:  Revision of last commit
| $Author:: viswp    $:  Author of last commit
| $Date:: 2023-01-19#$:  Date of last commit
|-----------------------------------------------------------------------------
| Copyright (c) by Vector Informatik GmbH.  All rights reserved.
 ----------------------------------------------------------------------------*/
#pragma once

#include "via/via.h"

/**
 * TODO: customize the information to suit your needs.
 */
VIAMetaInfo gFunctionMetaInfo =
{
  gModelName,                                 /* model name */
  "CANape/vSignalyzer Function DLL",          /* original model name */
  "1.0",                                      /* model version */
  "C-RT DLL Project Example",                 /* model description */
  "Vector Informatik (PMC21)",                /* initial user/creator */
  "",                                         /* last user name */
#if defined(_MSC_VER)
  "MSVC",                                     /* compiler */
  _MSC_VER,                                   /* compiler version */
#elif defined(__MINGW64__)
  "MinGW64",                                  /* compiler */
  __GNUC__,                                   /* compiler version */
#elif defined(__GNUC__)
  "GCC",                                      /* compiler */
  __GNUC__,                                   /* compiler version */
#else
  #error Compiler not supported
#endif
  __TIMESTAMP__,                              /* creation time */
  false,                                      /* multi sample rate */
#if defined(_DEBUG)
  true,                                       /* debug build */
#else
  false,                                      /* release build */
#endif
  /* Simulink Coder data */
  {
    "",                                       /* Matlab version             */
    "",                                       /* Simulink version           */
    "",                                       /* MSVC or MinGW STF          */
    "",                                       /* Target version             */
  }
};
