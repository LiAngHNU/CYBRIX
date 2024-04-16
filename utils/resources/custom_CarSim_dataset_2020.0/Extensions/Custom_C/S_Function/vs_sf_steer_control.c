/* MATLAB/Simulink functions to define a wrapper for a VehicleSim solver.
   This version is intended for public release.
   
   Log:
   Mar 03, 11. M. Sayers. Use newer vs_get_api function.
   Jun 24, 08. M. Sayers. Simplified loading VS API functions from DLL.
   Jan 19, 07. M. Sayers. Created this version for release with CarSim 7.
   */

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <string.h>

#include "vs_deftypes.h" // VS types and definitions
#include "vs_api.h"  // VS API functions
#include "external.h" // user-supplied custom C code

// Mathworks macro and include file for SimStruct and associated macros.
#define S_FUNCTION_NAME vs_sf_steer_control // This must match the DLL file name
#define S_FUNCTION_LEVEL 2
#include "simstruc.h"

static int sImportN, sExportN, sIcount;

// string buffer used over and over (locally) in functions
static char sSimfile[FILENAME_MAX], *sMsgDLL; 
static HMODULE vsDLL = NULL; // DLL with VS API
static char error_msg[2048];

static void sPrintMsg (char *str);

static void sResetPortSizes(SimStruct *S)
{
  ssSetNumInputPorts(S, 1);
  ssSetInputPortWidth(S, 0, DYNAMICALLY_SIZED);
  ssSetNumOutputPorts(S, 1);
  ssSetOutputPortWidth(S, 0, DYNAMICALLY_SIZED);
}

/* ----------------------------------------------------------------------------
   Read the VehicleSim simfile, load the DLL and API, then read the simulation
   data from the expanded parsfile.
---------------------------------------------------------------------------- */
static void mdlInitializeSizes (SimStruct *S)
{
  vs_real tstart;
  char pathDLL[FILENAME_MAX];

  // Set and check number of S-Function parameters 
  ssSetNumSFcnParams(S, 1);
  if (ssGetNumSFcnParams(S) != ssGetSFcnParamsCount(S)) {
    sResetPortSizes(S);
    sprintf(error_msg,
      "Error: Unexpected S-Function parameter count.\n");
    ssSetErrorStatus(S, error_msg);
    return;
  }

  // read configuration data for VehicleSim math model
  mxGetString(ssGetSFcnParam(S, 0), sSimfile, FILENAME_MAX - 1);
  if (vs_get_dll_path(sSimfile, pathDLL))
  {
    sResetPortSizes(S);
    sprintf(error_msg, "Unable to load solver module %s\n", pathDLL);
    ssSetErrorStatus(S, error_msg);
    return;
  }

  if (vsDLL == NULL)
  {
    vsDLL = LoadLibrary(pathDLL);
    if (vs_get_api(vsDLL, pathDLL))
    {
      sResetPortSizes(S);
      sprintf(error_msg,
        "Incompatible solver library file discovered. Please ensure a valid solver is available and"
        " verify 32/64 bit settings.");
      ssSetErrorStatus(S, error_msg);
      return;
    }

    tstart = vs_setdef_and_read(sSimfile, external_setdef, external_scan);
    vs_initialize(tstart, external_calc, external_echo);
    sImportN = *vs_get_var_ptr_int("NIMPORT");
    sExportN = *vs_get_var_ptr_int("NOUT_EXPORT");
  }

  // set number of import and export variables based on what was just read
  ssSetNumContStates(S, 0);
  ssSetNumDiscStates(S, 0);
  if (sImportN > 0)
  {
    if (!ssSetNumInputPorts(S, 1)) {
      sResetPortSizes(S);
      sprintf(error_msg,
        "Error: Unable to configure input ports.\n");
      ssSetErrorStatus(S, error_msg);
      return;
    }
    ssSetInputPortWidth(S, 0, sImportN);
    ssSetInputPortRequiredContiguous(S, 0, 1);
  }
  if (sExportN > 0)
  {
    if (!ssSetNumOutputPorts(S, 1)) {
      sResetPortSizes(S);
      sprintf(error_msg,
        "Error: Unable to configure output ports.\n");
      ssSetErrorStatus(S, error_msg);
      return;
    }
    ssSetOutputPortWidth(S, 0, sExportN);
  }

  ssSetNumSampleTimes(S, 1);
  ssSetNumRWork(S, 0);
  ssSetNumIWork(S, 0);
  ssSetNumPWork(S, 0);
  ssSetNumModes(S, 0);
  ssSetNumNonsampledZCs(S, 0);
  ssSetOptions(S, 0);
}

/* ----------------------------------------------------------------------------
   Specify the time step for the S-function. This matches the VehicleSim step
   if opt_io_update is FALSE. Otherwise it is 1/2 the VehicleSim time step.
---------------------------------------------------------------------------- */
static void mdlInitializeSampleTimes (SimStruct *S)
{
  ssSetSampleTime (S, 0, (real_T)vs_get_tstep());
  ssSetOffsetTime (S, 0, 0.0);
}


#define MDL_START // Activate function for level-2 S-Function
/* ----------------------------------------------------------------------------
   Set up import and export arrays, and exchange data prior to start of run.
---------------------------------------------------------------------------- */
static void mdlStart(SimStruct *S)
{
  real_T *ss_inputs, *ss_output;

  if (sImportN > 0) ss_inputs = (real_T *)ssGetInputPortSignal(S,0);
  else ss_inputs = NULL;
  if (sExportN > 0) ss_output = (real_T *)ssGetOutputPortSignal(S,0);
  else ss_output = NULL;

  vs_copy_io (ss_inputs, ss_output);
  
  // set up pointer for VS messages from this solver
  sMsgDLL = vs_get_output_message();
  sPrintMsg (sMsgDLL);
}

/* ----------------------------------------------------------------------------
   Must be defined but isn't used.
---------------------------------------------------------------------------- */
static void mdlOutputs(SimStruct *S, int_T tid)
{
}

/* ----------------------------------------------------------------------------
   Update the VehicleSim model for a new time step. 
---------------------------------------------------------------------------- */
#define MDL_UPDATE // Activate function for level-2 S-Function
static void mdlUpdate (SimStruct *S, int_T tid)
{
  vs_real t;
  real_T *ss_inputs, *ss_output;

  if (sImportN > 0) ss_inputs = (real_T*)ssGetInputPortSignal(S,0);
  else ss_inputs = NULL;
  if (sExportN > 0) ss_output = (real_T*)ssGetOutputPortSignal(S,0);
  else ss_output = NULL;

  // Get time. If the simulation is just starting, return.
  t = (vs_real)ssGetT(S);
  if (t == (vs_real)ssGetTStart(S)) return;

  // do all the work with this API call (June 2008). If return != 0, time to stop
  if (vs_integrate_io_2(t, ss_inputs, ss_output, external_calc))
    ssSetStopRequested (S, (int_T)1);

  sPrintMsg (sMsgDLL);
}

/* ----------------------------------------------------------------------------
   Terminate the simulation run.
---------------------------------------------------------------------------- */
static void mdlTerminate (SimStruct *S)
{
  vs_real t = ssGetT(S);

  external_calc (t, VS_EXT_EQ_END);
  vs_terminate (t, external_echo);

  sPrintMsg (sMsgDLL);

  vs_free_all ();
  FreeLibrary (vsDLL);
  vsDLL = NULL;
  printf("\n");
}

/* ----------------------------------------------------------------------------
  Print text and set first character to NULL
---------------------------------------------------------------------------- */
static void sPrintMsg (char *str)
{
  if (!*str) return;
  printf (str);
  *str = 0;
}


// Required S-function trailer
#ifdef  MATLAB_MEX_FILE  // Is this file being compiled as a MEX-file?
  #include "simulink.c"  // MEX-file interface mechanism
#else
  #include "cg_sfun.h"   // Code generation registration function
#endif
