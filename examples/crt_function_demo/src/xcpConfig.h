/*----------------------------------------------------------------------------
| File:
|   xcpConfig.h
|
| Description:
|   All the constant options and settings for the C_RT Project
|   are contained in this XCP configuration file.
|
| Copyright (c) Vector Informatik GmbH. All rights reserved.
| See LICENSE file in the project root for details.
|
 ----------------------------------------------------------------------------*/
#pragma once

#include <cstdint>

/* Uncomment the following line for Vector-Simulink transport layer */
#define XCP_TRANSPORT_LAYER_PL

/* Uncomment the following line for XCP-on-Ethernet usage. */
//#define XCP_TRANSPORT_LAYER_ETH

namespace Xcp 
{  
  // XCP params
  static constexpr const char *OptionProjectName = "crt_function";
  static constexpr bool OptionUseTcp = false;
  static constexpr uint16_t OptionServerPort = 5555;
  static constexpr uint8_t OptionServerAddress[] = {0, 0, 0, 0};
  static constexpr uint32_t OptionQueueSize = 1024 * 64;
  static constexpr uint8_t OptionLogLevel = 3;
} // namespace Xcp
