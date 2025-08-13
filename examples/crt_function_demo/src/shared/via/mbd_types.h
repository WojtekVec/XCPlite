/* Copyright (c) Vector Informatik GmbH. All rights reserved. */
#pragma once 

#if defined ( _MSC_VER ) && ( _MSC_VER < 1600 )
  typedef char               int8_t;
  typedef short              int16_t;
  typedef long               int32_t;
  typedef long long          int64_t;
  typedef unsigned char      uint8_t;
  typedef unsigned short     uint16_t;
  typedef unsigned long      uint32_t;
  typedef unsigned long long uint64_t;
#else
  /* use standard fixed width integer types and part of C numeric limits interface (C++ 11 or newer) */
  #include <stdint.h>
#endif
