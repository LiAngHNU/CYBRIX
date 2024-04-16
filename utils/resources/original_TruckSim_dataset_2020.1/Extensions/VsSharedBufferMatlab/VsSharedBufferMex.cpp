#include "VsCameraSensor.hpp"
#include <math.h>
#include <matrix.h>
#include <mex.h>
#include <algorithm>
#include <map>



using namespace vssbut;

#define SHARED_BUFFER_COMMAND_MEXNAME   "VsSharedBufferMex"
#define SHARED_BUFFER_COMMAND_CONNECT   "connect"
#define SHARED_BUFFER_COMMAND_LOCK      "lock"
#define SHARED_BUFFER_COMMAND_UNLOCK    "unlock"
#define SHARED_BUFFER_COMMAND_QUERY     "query"
#define SHARED_BUFFER_COMMAND_CLOSE     "close"
#define SHARED_BUFFER_COMMAND_STATUS    "status"
#define SHARED_BUFFER_COMMAND_HELP      "help"
#define SB_HELP_INDENT                  "  "
#define SB_HELP_COMMAND_VERTICAL_INDENT "\n"
#define SB_HELP_EOL                     "\n"

#define SHARED_BUFFER_MEX_USAGE_STRING \
    "Usage:" SB_HELP_EOL \
    "------" SB_HELP_EOL \
    SHARED_BUFFER_COMMAND_MEXNAME " [command] [optional: command parameters]" SB_HELP_EOL \
    SB_HELP_COMMAND_VERTICAL_INDENT \
    "Commands:" SB_HELP_EOL \
    "---------" SB_HELP_EOL \
    SHARED_BUFFER_COMMAND_CONNECT " [shared buffer name]" SB_HELP_EOL \
    SB_HELP_INDENT "Connects to 'shared buffer name', which must already exist." SB_HELP_EOL \
    SB_HELP_COMMAND_VERTICAL_INDENT \
    SHARED_BUFFER_COMMAND_LOCK " [shared buffer name]" SB_HELP_EOL \
    SB_HELP_INDENT "Lock the specified buffer. Can be done before multiple queries to ensure\n"\
    SB_HELP_INDENT "that the samples are all from the same frame." SB_HELP_EOL \
    SB_HELP_COMMAND_VERTICAL_INDENT \
    SHARED_BUFFER_COMMAND_UNLOCK " [shared buffer name]" SB_HELP_EOL \
    SB_HELP_INDENT "Unlock the specified buffer." SB_HELP_EOL \
    SB_HELP_COMMAND_VERTICAL_INDENT \
    SHARED_BUFFER_COMMAND_QUERY "" SB_HELP_EOL \
    SB_HELP_INDENT "Please enter '" SHARED_BUFFER_COMMAND_MEXNAME " " SHARED_BUFFER_COMMAND_QUERY "' for detailed help." SB_HELP_EOL \
    SB_HELP_COMMAND_VERTICAL_INDENT \
    SHARED_BUFFER_COMMAND_CLOSE  " [optional shared buffer name]" SB_HELP_EOL \
    SB_HELP_INDENT "If buffer name is provided, closes connection to that buffer. If no name is provided, closes all." SB_HELP_EOL \
    SB_HELP_COMMAND_VERTICAL_INDENT \
    SHARED_BUFFER_COMMAND_STATUS SB_HELP_EOL \
    SB_HELP_INDENT "Prints the status of all connected buffers." SB_HELP_EOL \
    SB_HELP_COMMAND_VERTICAL_INDENT \
    SHARED_BUFFER_COMMAND_HELP SB_HELP_EOL \
    SB_HELP_INDENT "Displays this message." SB_HELP_EOL

#define SHARED_BUFFER_QUERY_USAGE_STRING \
    "Usage:" SB_HELP_EOL \
    "------" SB_HELP_EOL \
    "[outputs] = "SHARED_BUFFER_COMMAND_MEXNAME"( 'query', 'BufferName', azimuth, elevation)" SB_HELP_EOL \
    SB_HELP_COMMAND_VERTICAL_INDENT \
    "Inputs:" SB_HELP_EOL \
    "-------" SB_HELP_EOL \
    SB_HELP_COMMAND_VERTICAL_INDENT \
    SB_HELP_INDENT "[string]   Command (in this case 'query')." SB_HELP_EOL \
    SB_HELP_INDENT "[string]   Sensor buffer name - name of the sensor buffer to query." SB_HELP_EOL \
    SB_HELP_INDENT "[double]   Azimuth, in radians, for the ray to interrogate, center is 0." SB_HELP_EOL \
    SB_HELP_INDENT "[double]   Elevation, in radians, for the ray to interrogate, center is 0." SB_HELP_EOL \
    SB_HELP_COMMAND_VERTICAL_INDENT \
    "Outputs:" SB_HELP_EOL \
    "--------" SB_HELP_EOL \
    SB_HELP_COMMAND_VERTICAL_INDENT \
    SB_HELP_INDENT "[double]   Frame number of generated results." SB_HELP_EOL \
    SB_HELP_INDENT "[double]   Frame time of generated results." SB_HELP_EOL \
    SB_HELP_INDENT "[boolean]  True if color information was retrieved." SB_HELP_EOL \
    SB_HELP_INDENT "[boolean]  True if depth information was retrieved." SB_HELP_EOL \
    SB_HELP_INDENT "[boolean]  True if normal information was retrieved." SB_HELP_EOL \
    SB_HELP_INDENT "[matrix]   Color information, 8-bit RGB." SB_HELP_EOL \
    SB_HELP_INDENT "[double]   Depth, in meters, of the ray when it intersected the scene." SB_HELP_EOL \
    SB_HELP_INDENT "[matrix]   Normal, in world coordinates, of intersected surface." SB_HELP_EOL \
    SB_HELP_INDENT "[matrix]   World coordinates of camera origin." SB_HELP_EOL \
    SB_HELP_INDENT "[matrix]   World coordinates of intersected surface." SB_HELP_EOL \
    SB_HELP_COMMAND_VERTICAL_INDENT \
    "Example, querying the center of a sensor named 'Left':" SB_HELP_EOL \
    "---------" SB_HELP_EOL \
    "[frameNumL, frameTimeL, gotColorL, gotDepthL, gotNormalL, colorL, depthL," SB_HELP_EOL \
    " normalL, camOriginL, intersectPtL] = " SHARED_BUFFER_COMMAND_MEXNAME "('query', 'Left', 0, 0)" SB_HELP_EOL


std::map<std::string, std::shared_ptr<VsCameraSensor> > sSensorMap;


//
// [func] connect [shared buffer name]
//   * return: error code, 0 on success
//

static void sConnect(int nOutput, mxArray *output[], int nInput, const mxArray *input[])
{
  const int VINDEX_BUFFER_NAME = 1;
  double *returnErrorCode = mxGetPr(output[0] = mxCreateDoubleScalar(-1));
  std::string cameraStatus("?");

  if (nInput > VINDEX_BUFFER_NAME)
  {
    std::string camName8(mxArrayToString(input[VINDEX_BUFFER_NAME]));
    std::wstring camName(camName8.begin(), camName8.end());
    std::shared_ptr<VsCameraSensor> camSensor;

    mexPrintf("Creating camera attached to buffer \"%s\".\n", camName8.c_str());

    camSensor = VsCameraSensor::Create(camName.c_str(), false);

    if (nullptr != camSensor)
    {
      mexPrintf("  connecting...\n");

      camSensor->Connect(1000);

      vssb_State state = camSensor->GetStatus();
      std::wstring cameraStatusWide(vssbut_DescribeStatus(state));
      cameraStatus = std::string(cameraStatusWide.begin(), cameraStatusWide.end());

      switch(state)
      {
        case VSSBSTATE_ACTIVE:
          {
            std::pair<std::string, std::shared_ptr<VsCameraSensor> > mapEntry(camName8, camSensor);
            std::map<std::string, std::shared_ptr<VsCameraSensor> >::iterator it;

            it = sSensorMap.find(camName8);
            if (it != sSensorMap.end())
            {
              mexPrintf("  removing old mapped sensor \"%s\"\n", camName8.c_str());
              sSensorMap.erase(camName8.c_str());
            }

            sSensorMap.insert(mapEntry);

            *returnErrorCode = 0;
          }
          break;

        case VSSBSTATE_UNDEFINED:
        case VSSBSTATE_DONE:
          break;
      }
    }
  }
  else
  {
    mexPrintf("  missing parameters, found %d\n", nInput);
  }

  mexPrintf("Camera Status: \"%s\"\n", cameraStatus.c_str());
}

// [func] query   [connectionName] [ray azimuth, ray elevation]
//   * return: frame number, integer
//   * return: frame time, double
//   * return: retrieved color, non-zero if color acquired, integer
//   * return: retrieved depth, non-zero if color acquired, integer
//   * return: retrieved normal, non-zero if color acquired, integer
//   * return: Color at intersection point, vector3 (RGB)
//   * return: Depth at intersection point, double
//   * return: Surface normal vector at intersection point, vector3
//   * return: Global coords of camera origin point, vector3
//   * return: Global coords of intersection point, vector3
//
// [frameNum, frameTime, bColor, bDepth, bNormal,
//  color[3], depth, normal[3], camOrigin[3], intersection[3]] = query('Left', localRay[3])
//
// -----------------------------------------------------------------------------------
// Doug Blue's current:
//
// Inputs:
//   sen_xyz   Sensor location is global coords
//   ray_dir   Sensor axis in global coords
//
// Outputs:
//   sloc      Global coords of intersection point
//   sgrad     Surface normal vector at intersection point
//   surfID    ID of surface intersected by ray
//
// [sloc,sgrad,surfID]=BRSmex('dummy', 2, [sen_xyz(1) sen_xyz(2) sen_xyz(3)], [ray_dir(1) ray_dir(2) ray_dir(3)]);

static void sQuery(int nOutput, mxArray *output[], int nInput, const mxArray *input[])
{
  enum QueryOutputIndices
  {
    QO_FRAME_NUMBER,
    QO_FRAME_TIME,
    QO_SUCCESS_COLOR,
    QO_SUCCESS_DEPTH,
    QO_SUCCESS_NORMAL,
    QO_COLOR,
    QO_DEPTH,
    QO_NORMAL,
    QO_CAMERA_ORIGIN,
    QO_INTERSECT_POINT,
    //-------------
    QO_MAXIMUM,
  };

  enum QueryInputIndices
  {
    QI_COMMAND_NAME,
    QI_SENSOR_NAME,
    QI_RAY_AZIMUTH,
    QI_RAY_ELEVATION,
    //---------------
    QI_MAXIMUM,
  };

  if (nInput <= QI_SENSOR_NAME)
  {
    mexPrintf(SHARED_BUFFER_QUERY_USAGE_STRING);
    return;
  }

  std::shared_ptr<VsCameraSensor> camSensor;
  std::string camName8(mxArrayToString(input[QI_SENSOR_NAME]));

  camSensor = sSensorMap[camName8];

  if (nullptr == camSensor)
  {
    mexPrintf("No camera sensor connected at \"%s\".\n\n" SHARED_BUFFER_QUERY_USAGE_STRING "\n", camName8.c_str());
    return;
  }

  if ((nOutput >= QO_MAXIMUM) && (nInput >= QI_MAXIMUM))
  {
    double *retFrameNumber    =  mxGetPr(output[QO_FRAME_NUMBER   ] = mxCreateDoubleScalar(-1));
    double *retFrameTime      =  mxGetPr(output[QO_FRAME_TIME     ] = mxCreateDoubleScalar(-1));

    bool *retGotColor  =  mxGetLogicals(output[QO_SUCCESS_COLOR  ] = mxCreateLogicalScalar(false));
    bool *retGotDepth  =  mxGetLogicals(output[QO_SUCCESS_DEPTH  ] = mxCreateLogicalScalar(false));
    bool *retGotNormal =  mxGetLogicals(output[QO_SUCCESS_NORMAL ] = mxCreateLogicalScalar(false));

    double *retColor   =  mxGetPr(output[QO_COLOR          ] = mxCreateDoubleMatrix(1,3,mxREAL));
    double *retDepth   =  mxGetPr(output[QO_DEPTH          ] = mxCreateDoubleScalar(-1));
    double *retNormal  =  mxGetPr(output[QO_NORMAL         ] = mxCreateDoubleMatrix(1,3,mxREAL));

    double *retCamOrigin      =  mxGetPr(output[QO_CAMERA_ORIGIN  ] = mxCreateDoubleMatrix(1,3,mxREAL));
    double *retIntersectPoint =  mxGetPr(output[QO_INTERSECT_POINT] = mxCreateDoubleMatrix(1,3,mxREAL));

    // grab inputs
    double *inputRayAzimuth    = mxGetPr(input[QI_RAY_AZIMUTH]);
    double *inputRayElevation  = mxGetPr(input[QI_RAY_ELEVATION]);

    assert(inputRayAzimuth);
    assert(inputRayElevation);

    // determine target pixel
    vssbutPixelLocation pixel;
    pixel.mColumn = vssbut_GetPixelIndex(camSensor->GetWidth(),  camSensor->GetFovHorizRad(), *inputRayAzimuth);
    pixel.mRow    = vssbut_GetPixelIndex(camSensor->GetHeight(), camSensor->GetFovVertRad(),  *inputRayElevation);

    //
    // Lock and retrieve data
    //
    camSensor->Lock();

    *retFrameNumber = camSensor->GetFrameNumber();
    *retFrameTime   = camSensor->GetFrameTime();

    vssbutColorBgra8 pixelColor = camSensor->GetColor(pixel, retGotColor);
    if (*retGotColor)
    {
      // normalize the color values, from 8-bit per channel source.
      retColor[0] = pixelColor.red / 255.0;
      retColor[1] = pixelColor.green / 255.0;
      retColor[2] = pixelColor.blue / 255.0;
    }

    *retDepth = camSensor->GetDepth(pixel);
    *retGotDepth = (vssbut_IsValidDouble(*retDepth));

    vssbutEuclideanVector normal = camSensor->GetNormal(pixel);
    *retGotNormal = vssbut_IsValidVect(&normal);
    if (*retGotNormal)
    {
      retNormal[0] = normal.forward;
      retNormal[1] = normal.leftward;
      retNormal[2] = normal.upward;
    }

    vssbutEuclideanVector camPosition = camSensor->GetPosition();
    retCamOrigin[0] = camPosition.forward;
    retCamOrigin[1] = camPosition.leftward;
    retCamOrigin[2] = camPosition.upward;

    // turn depth into world position of intersection, if it was retrieved
    if (*retGotDepth)
    {
      vssbutEuclideanVector intersectLocal, intersectWorld;

      intersectLocal.forward = *retDepth;
      intersectLocal.leftward = 0;
      intersectLocal.upward = 0;

      intersectWorld = camSensor->LocalToWorld(intersectLocal);

      retIntersectPoint[0] = intersectWorld.forward;
      retIntersectPoint[1] = intersectWorld.leftward;
      retIntersectPoint[2] = intersectWorld.upward;
    }

    camSensor->Unlock();
  }
  else
  {
    if (nOutput < QO_MAXIMUM)
      mexPrintf("Missing outputs, found %d, expected %d.", nOutput, QO_MAXIMUM);
    if (nInput < QI_MAXIMUM)
      mexPrintf("\nMissing inputs, found %d, expected %d", nInput, QI_MAXIMUM);
    mexPrintf("\n");
  }
}


static void sLock(int nOutput, mxArray *output[], int nInput, const mxArray *input[])
{
  enum LockInputIndices
  {
    LI_COMMAND_NAME,
    LI_SENSOR_NAME,
  };

  std::shared_ptr<VsCameraSensor> camSensor;
  std::string camName8(mxArrayToString(input[LI_SENSOR_NAME]));

  camSensor = sSensorMap[camName8];

  if (nullptr == camSensor)
  {
    mexPrintf("No camera sensor connected at \"%s\".\n\n" SHARED_BUFFER_QUERY_USAGE_STRING "\n", camName8.c_str());
    return;
  }

  camSensor->Lock();
  mexPrintf("Locked camera sensor \"%s\".\n", camName8.c_str());
}


static void sUnlock(int nOutput, mxArray *output[], int nInput, const mxArray *input[])
{
  enum LockInputIndices
  {
    LI_COMMAND_NAME,
    LI_SENSOR_NAME,
  };

  std::shared_ptr<VsCameraSensor> camSensor;
  std::string camName8(mxArrayToString(input[LI_SENSOR_NAME]));

  camSensor = sSensorMap[camName8];

  if (nullptr == camSensor)
  {
    mexPrintf("No camera sensor connected at \"%s\".\n\n" SHARED_BUFFER_QUERY_USAGE_STRING "\n", camName8.c_str());
    return;
  }

  camSensor->Unlock();
  mexPrintf("Unlocked camera sensor \"%s\".\n", camName8.c_str());
}


// [func] close   [connectionName *or* 'all']
//   * return: error code, 0 on success
static void sClose(int nOutput, mxArray *output[], int nInput, const mxArray *input[])
{
  enum CloseInputIndices
  {
    CI_COMMAND_NAME,
    CI_OPTIONAL_SENSOR_NAME,
    //---------------
    CI_MAX,
  };

  if (nInput > CI_OPTIONAL_SENSOR_NAME)
  {
    char *sensorName = mxArrayToString(input[CI_OPTIONAL_SENSOR_NAME]);
    if (sensorName)
    {
      std::map<std::string, std::shared_ptr<VsCameraSensor> >::iterator it;
      it = sSensorMap.find(std::string(sensorName));
      if (it == sSensorMap.end())
      {
        mexPrintf("\nCould not find sensor \"%s\" to close.\n");
      }
      else
      {
        sSensorMap.erase(sensorName);
        mexPrintf("Removed sensor \"%s\".\n", sensorName);
      }
    }
    else
    {
      mexPrintf("\nNo sensor to close named \"%s\".\n", sensorName);
    }
  }
  else
  {
    // close all connections
    mexPrintf("Clearing all sensors. %d removed.\n", sSensorMap.size());
    sSensorMap.clear();
  }
}


void sStatus(void)
{
  mexPrintf("Sensors Status:\n");
  mexPrintf("---------------\n");

  std::map<std::string, std::shared_ptr<VsCameraSensor> >::iterator it = sSensorMap.begin();
  while (it != sSensorMap.end())
  {
    vssb_State state = it->second->GetStatus();
    std::wstring cameraStatusWide(vssbut_DescribeStatus(state));
    std::string status = std::string(cameraStatusWide.begin(), cameraStatusWide.end());

    mexPrintf("%15s: %s\n", it->first.c_str(), status.c_str());

    ++it;
  }

  mexPrintf("- Status End --\n");
}


void sMexFileExitCallback(void)
{
  mexPrintf("Clearing all sensors. %d removed.\n", sSensorMap.size());
  sSensorMap.clear();
}


void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
  #ifdef _DEBUG
    mexPrintf("%s: Inputs (Right-Hand Side): %d\n", mexFunctionName(), nrhs);
    mexPrintf("    Outputs (Left-Hand Side): %d\n\n", nlhs);
  #endif
  
  if (nrhs < 1)
  {
    mexPrintf(SHARED_BUFFER_MEX_USAGE_STRING "\n");
    return;
  }

  char *arrayCommand = mxArrayToString(prhs[0]);
  std::string command(arrayCommand);
  std::transform(command.begin(), command.end(), command.begin(), ::tolower);

  if (0 == command.compare(SHARED_BUFFER_COMMAND_CONNECT))
  {
    mexAtExit(sMexFileExitCallback);
    sConnect(nlhs, plhs, nrhs, prhs);
  }
  else if (0 == command.compare(SHARED_BUFFER_COMMAND_QUERY))
  {
    sQuery(nlhs, plhs, nrhs, prhs);
  }
  else if (0 == command.compare(SHARED_BUFFER_COMMAND_LOCK))
  {
    sLock(nlhs, plhs, nrhs, prhs);
  }
  else if (0 == command.compare(SHARED_BUFFER_COMMAND_UNLOCK))
  {
    sUnlock(nlhs, plhs, nrhs, prhs);
  }
  else if (0 == command.compare(SHARED_BUFFER_COMMAND_CLOSE))
  {
    sClose(nlhs, plhs, nrhs, prhs);
  }
  else if (0 == command.compare(SHARED_BUFFER_COMMAND_STATUS))
  {
    sStatus();
  }
  else if (0 == command.compare(SHARED_BUFFER_COMMAND_HELP))
  {
    mexPrintf(SHARED_BUFFER_MEX_USAGE_STRING);
  }

  mxFree(arrayCommand);
}
