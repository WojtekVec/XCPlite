/*----------------------------------------------------------------------------
|     Project: Vector CANape/vSignalyzer C-RT DLL Project
|
|        File: crt_function_data.h
|
| Description: Global ASAP2 objects can be declared here.
|              This objects are visible to CANape/vSignalyzer
|              and can be manipulated using the VCrtFunction class.
|              ASAP2 measurement and parameter objects are defined in the
|              corresponding ASAP2 database file.
|
|     Remarks: For more information about ASAP2 database file creation,
|              please refer to the official ASAP2 Tool-Set user manual on
|              https://www.vector.com
|
|-----------------------------------------------------------------------------
| $Rev:: 8310        $:  Revision of last commit
| $Author:: viswp    $:  Author of last commit
| $Date:: 2025-07-16#$:  Date of last commit
|-----------------------------------------------------------------------------
| Copyright (c) by Vector Informatik GmbH.  All rights reserved.
 ----------------------------------------------------------------------------*/
#pragma once

namespace Const
{
  // Array dimensions
  static constexpr uint16_t DimX = 20;
  static constexpr uint16_t DimY = 20;
}


// Start: simulation time ===========================================
/*
@@ SYMBOL = simTime
@@ A2L_TYPE = MEASURE
@@ DATA_TYPE = DOUBLE
@@ DESCRIPTION = "Internal simulation time based on the specified fixed step size"
@@ EVENT XCP = VARIABLE 0 1 2 3
@@ END
*/
double simTime = 0.0;
// End: simulation time =============================================

// Start: function counter ==========================================
/*
@@ SYMBOL = fcnCounter1
@@ A2L_TYPE = MEASURE
@@ DATA_TYPE = ULONG
@@ DESCRIPTION = "Function counter #1, reset at measurement start"
@@ EVENT XCP = VARIABLE 0 1 2 3
@@ END
*/
uint32_t fcnCounter1 = 0;

/*
@@ SYMBOL = fcnCounter2
@@ A2L_TYPE = MEASURE
@@ DATA_TYPE = ULONG
@@ DESCRIPTION = "Function counter #2, free running"
@@ EVENT XCP = VARIABLE 0 1 2 3
@@ END
*/
uint32_t fcnCounter2 = 0;
// End: function counter ============================================

// Start: example of using structures for input data ================
struct InputData
{
  /*
  @@ ELEMENT = input1
  @@ STRUCTURE = InputData
  @@ A2L_TYPE = MEASURE
  @@ DATA_TYPE = DOUBLE
  @@ DESCRIPTION = "Function input port 1 data"
  @@ EVENT XCP = VARIABLE 0 1 2 3
  @@ END
  */
  double input1;
  
  /*
  @@ ELEMENT = input2
  @@ STRUCTURE = InputData
  @@ A2L_TYPE = MEASURE
  @@ DATA_TYPE = DOUBLE
  @@ DESCRIPTION = "Function input port 2 data"
  @@ EVENT XCP = VARIABLE 0 1 2 3
  @@ END
  */
  double input2;
  // uint8_t buffer[16384]; // example of binary data buffer for input
};

/*
@@ INSTANCE = inputData
@@ STRUCTURE = InputData
@@ END
*/
InputData inputData; // inputs are stored in a structure
// End: example of using structures for input data ==================

// Start: example of using structures for output data ===============
struct OutputData
{
  /*
  @@ ELEMENT = output1
  @@ STRUCTURE = OutputData
  @@ A2L_TYPE = MEASURE
  @@ DATA_TYPE = DOUBLE
  @@ DESCRIPTION = "Function output port 1 data"
  @@ EVENT XCP = VARIABLE 0 1 2 3
  @@ END
  */
  double output1;

  /*
  @@ ELEMENT = output2
  @@ STRUCTURE = OutputData
  @@ A2L_TYPE = MEASURE
  @@ DATA_TYPE = DOUBLE
  @@ DESCRIPTION = "Function output port 2 data"
  @@ EVENT XCP = VARIABLE 0 1 2 3
  @@ END
  */
  double output2;

  /*
  @@ ELEMENT = output3
  @@ STRUCTURE = OutputData
  @@ A2L_TYPE = MEASURE
  @@ DATA_TYPE = DOUBLE
  @@ DESCRIPTION = "Function output port 2 data"
  @@ EVENT XCP = VARIABLE 0 1 2 3
  @@ END
  */
  double output3;
};


/*
@@ INSTANCE = outputData
@@ STRUCTURE = OutputData
@@ END
*/
OutputData outputData; // outputs are stored in a structure
// End: example of using structures for output data =================

/*
@@ SUB_GROUP = table
@@ DESCRIPTION = "Look-Up table group"
@@ END
*/

// Start: LookUp ====================================================
/*
@@ SYMBOL = lookUpTable
@@ A2L_TYPE = PARAMETER
@@ DATA_TYPE = DOUBLE
@@ DIMENSION = 8
@@ DESCRIPTION = "Example of a Look-Up-Table multi-dim parameter"
@@ EVENT XCP = VARIABLE 0 1 2 3
@@ GROUP = table
@@ END
*/
double lookUpTable[] = { 0.0000000000, 0.7071067812, 1.0000000000, 0.7071067812, 0.0000000000, -0.7071067812, -1.0000000000, -0.7071067812 };

/*
@@ SYMBOL = lookUpTableSize
@@ A2L_TYPE = MEASURE
@@ DATA_TYPE = ULONG
@@ DESCRIPTION = "Example of a Look-Up-Table size"
@@ EVENT XCP = VARIABLE 0 1 2 3
@@ GROUP = table
@@ END
*/
uint32_t lookUpTableSize = static_cast<uint32_t>(sizeof(lookUpTable) / sizeof(double));

// End: LookUp ======================================================

// Start: Array =====================================================
/*
@@ SYMBOL = arrayParam
@@ A2L_TYPE = PARAMETER
@@ DATA_TYPE = DOUBLE
@@ DIMENSION = 20 20
@@ DESCRIPTION = "Example of an array parameter"
@@ EVENT XCP = VARIABLE 0 1 2 3
@@ GROUP = array
@@ END
*/
double arrayParam[Const::DimX][Const::DimY];

/*
@@ SYMBOL = arrayObj
@@ A2L_TYPE = MEASURE
@@ DATA_TYPE = DOUBLE
@@ DIMENSION = 20 20
@@ DESCRIPTION = "Example of an array measurement object"
@@ EVENT XCP = VARIABLE 0 1 2 3
@@ GROUP = array
@@ END
*/
double arrayObj[Const::DimX][Const::DimY];

// End: Array =======================================================