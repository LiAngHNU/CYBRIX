/** Utility functions for use with the VS Shared Buffer library.
*/

#ifndef _vs_SharedBuffer_Utility_h_INCLUDED
#define _vs_SharedBuffer_Utility_h_INCLUDED

#include <assert.h>
#include <stdint.h>
#include <stdbool.h>

/* NOTE: The configuration of this utility library corresponds to the
  configuration of the core VS Shared Buffer library used. I.e. the core
  library and this utility library have to be either both dynamically-linked
  DLL's, or both have to be statically-linked libs.
  
  If using statically-linked lib's, define VSSBUTIL_STATICLIB before including this file (e.g. in your project settings).

  The following code will define the corresponding macro for the core library
  as needed.
*/
# if defined( VSSBUTIL_MAKINGDLL )
#   if defined( VSSBUTIL_STATICLIB )
#     #error "Both VSSBUTIL_MAKINGDLL and VSSBUTIL_STATICLIB are defined."
#   endif
#   define VSSBUTIL_API __declspec(dllexport)
# elif defined( VSSBUTIL_STATICLIB )
#   define VSSBUTIL_API
# else /* Not making DLL, or using static libs... must be using DLL. */
#   define VSSBUTIL_API __declspec(dllimport)
# endif


#include "vs_SharedBuffer.h"


/** Describes the location of a pixel relative to the lower-left corner
  of the image/buffer and increasing to the right and upward.
  An invalid vssbutPixelLocation is identified by having the first value
  return false from vssbut_GetApi()->IsValidInt().
*/
typedef struct vssbutPixelLocation
{
  int   mColumn;  ///< "X".
  int   mRow;     ///< "Y".
  #if defined(__cplusplus)
  bool                        operator ==(const vssbutPixelLocation& rhs) const
  {
    return  (     rhs.mColumn   == mColumn
              &&  rhs.mRow      == mRow     );
  }
  #endif

} vssbutPixelLocation;


/** A point or direction in 3D space.
  An invalid vssbutEuclideanVector is identified by having its first component
  return false from vssbut_GetApi()->IsValidDouble().
*/
typedef struct vssbutEuclideanVector
{
  double  forward;      ///< VehicleSim X
  double  leftward;     ///< VehicleSim Y
  double  upward;       ///< VehicleSim Z
} vssbutEuclideanVector;


/** Axis-angle representation of an orientation in 3D space.
  See https://en.wikipedia.org/wiki/Axis-angle_representation

  An invalid vssbutOrientation is identified by having the angle component
  return false from vssbut_GetApi()->IsValidDouble().
*/
typedef struct vssbutOrientation
{
  double                        angleRad; ///< Angle of rotation about the axis, in radians.
  struct vssbutEuclideanVector  axis;     ///< Axis of rotation.
} vssbutOrientation;


/// Color containing Blue, Green, Red, and Alpha, each represented by 8-bits.
typedef struct vssbutColorBgra8
{
  unsigned char   blue;
  unsigned char   green;
  unsigned char   red;
  unsigned char   alpha;
} vssbutColorBgra8;


#if defined(__cplusplus)
extern "C"
{
#endif

  /** Returns the core VS Shared Buffer API, which is used by all functions/classes of this library.
    This pointer will be valid for the duration of the process execution (i.e. you can call this
    once and store a local copy of the pointer).
  */
  VSSBUTIL_API const vssb_Api*
                              vssbut_GetApi();

  VSSBUTIL_API bool           vssbut_IsValidInt(int value);
  VSSBUTIL_API void           vssbut_InvalidateInt(int* inout_value);

  VSSBUTIL_API bool           vssbut_IsValidDouble(double value);
  VSSBUTIL_API void           vssbut_InvalidateDouble(double* inout_value);

  /** Returns true if the location is valid. This does NOT indicate
  that the pixel location is or is not within any given range. I.e.
  this does not indicate whether the specified location is within
  the current camera sensor's buffer range. E.g. the location 
  (-68432257, 85633245) is a "valid" location even though it's
  not in the range of any buffer.
  */
  VSSBUTIL_API bool           vssbut_IsValidPixelLocation(const struct vssbutPixelLocation* pixelLoc);
  VSSBUTIL_API void           vssbut_InvalidatePixelLocation(struct vssbutPixelLocation* inout_pixelLoc);

  VSSBUTIL_API bool           vssbut_IsValidVect(const struct vssbutEuclideanVector* vect);
  VSSBUTIL_API void           vssbut_InvalidateVect(struct vssbutEuclideanVector* inout_vect);

  VSSBUTIL_API bool           vssbut_IsValidOri(const vssbutOrientation* ori);
  VSSBUTIL_API void           vssbut_InvalidateOri(vssbutOrientation* inout_ori);

  /** Returns the pixel index given valid values for:
        numPixels - The number of pixels in the row/column.
        fovRad    - The "field of view" of the row/column in radians - the total angle that
                    the row/column of pixels represent.
        angleRad  - The angle offset, in radians, from the CENTER of the pixel row/column.
  
    Return value and behavior are undefined if passed values are not valid. Validity
    means:
        numPixels > 0
        fovRad is between 0 and Pi, exclusive.
        angleRad is between -Pi/2 and Pi/2, exclusive.
  
    This function may return a value less than 0 or greater than numPixels (if
    the passed angle falls outside of the FOV).
  */
  VSSBUTIL_API int            vssbut_GetPixelIndex( int numPixels, double fovRad, double angleRad);

  /// Alias for vssbut_GetPixelIndex().
  VSSBUTIL_API int            vssbut_GetPixelColumn( int frameWidthPixels, double fovHorizRad, double azimuthRad);

  /// Alias for vssbut_GetPixelIndex().
  VSSBUTIL_API int            vssbut_GetPixelRow( int frameHeightPixels, double fovVertRad, double elevationRad);

  /** Returns the angle of the center of the specified pixel in a row/column
    of pixels with the specified Field Of View. The angle returned is relative
    to the center of the center point of the row/column.

    Return value and behavior are undefined if passed values are not valid. Validity
    means:
      numPixels > 0
      fovRad is between 0 and Pi, exclusive.
      vssbut_IsValidInt(pixelIndex) returns true.

    This function may return a value that is outside of the specified FOV (if
    the passed pixel index is < 0 or > (numPixels - 1)).
  */
  VSSBUTIL_API double         vssbut_GetPixelAngle( int numPixels, double fovRad, int pixelIndex );

  /// Alias for vssbut_GetPixelAngle().
  VSSBUTIL_API double         vssbut_GetPixelAzimuth( int frameWidthPixels, double fovHorizRad, int pixelColumn );

  /// Alias for vssbut_GetPixelAngle().
  VSSBUTIL_API double         vssbut_GetPixelElevation( int frameHeightPixels, double fovVertRad, int pixelRow );

  /** Returns the offset distance (in pixels) of the center of the specified
    pixel from the center of the row/column of pixels.
    
    For example, the offset distance of pixel 0 in a row of 2 pixels is -0.5,
    and the center of pixel 1 is 0.5.
    
    In a row of 3 pixels, the offset of pixel 1 (the center pixel) is 0.0, and
    pixel 0 is -1.0.

    The pixelIndex does NOT have to be in the range of 0 to (numPixels - 1).
    For example, the offset distance of pixel -1 in a row of 2 pixels is 
    -1.5, and the offset distance of pixel 4 is 3.5.
  */
  VSSBUTIL_API double         vssbut_GetPixelCenterOffset( int numPixels, int pixelIndex );

  /// Returns an English string describing the result code, or "?" if the status code is unrecognized.
  VSSBUTIL_API const wchar_t* vssbut_DescribeResult(vssb_Result result);

  /// Returns an English string describing the status code (e.g. "Undefined", "Active", "Done", or "?" if the status code is unrecognized).
  VSSBUTIL_API const wchar_t* vssbut_DescribeStatus( vssb_State status );

  /** Returns the location (image/buffer coordinates) of the pixel at the
    specified direction (local/camera coordinates).
    
    The returned location may not fall within the extents of the buffer.
    
    Returns an invalid location if any of the following are true:
      * Not connected
      * The specified direction does not intersect the viewing plane.
  */
  VSSBUTIL_API vssbutPixelLocation  
                              vssbut_GetPixelLocation( VsSharedBuffer bufferHandle, const struct vssbutEuclideanVector* directionLocal );

  /** Returns a direction in local/camera coordinates of the center of the
    pixel (specified in image/buffer coordinates).
    The returned vector is NOT necessarily a unit vector.
    Returns an invalid vector if not connected.

    Behavior is undefined if the pixel location is invalid.
  */
  VSSBUTIL_API vssbutEuclideanVector
                              vssbut_GetPixelDirectionLocal( VsSharedBuffer bufferHandle, const struct vssbutPixelLocation* pixelLocation );

  /** Returns the position of the specified pixel in local/camera coordinates.
    Returns an invalid location if any of the following are true:
      * Not connected

    Behavior is undefined if the pixel location falls outside the extents
    of the buffer.
  */
  VSSBUTIL_API vssbutEuclideanVector
                              vssbut_GetPixelPositionLocal( VsSharedBuffer bufferHandle, const struct vssbutPixelLocation* pixelLocation );

  /** Returns the color at the specified pixel location. 
    Returns "transparent black" (0,0,0,0) and sets out_opt_valid to false if
    any of the following are true:
      * Not connected.
      * Color information is not available.
  
    If only RGB information is available (i.e. the buffer data format is
    VSSBCT_RGB), the alpha component will be set to 255 (opaque).

    Behavior is undefined if the pixel location falls outside the extents
    of the buffer.
  */
  VSSBUTIL_API vssbutColorBgra8
                              vssbut_GetPixelColor( VsSharedBuffer bufferHandle, const vssbutPixelLocation* pixel, bool* out_opt_valid );

  /** Returns the depth from the camera eye plane to the specified pixel.
    Returns a value that is !Valid() if any of the following are true:
      * Not connected.
      * Depth information is not available from this shared buffer.

    Behavior is undefined if the pixel location falls outside the extents
    of the buffer.
  */
  VSSBUTIL_API double         vssbut_GetPixelDepth( VsSharedBuffer bufferHandle, const struct vssbutPixelLocation* pixelLocation );

  /** Returns the square of the distance eye point to the specified pixel.
    Returns a value that is !Valid() if any of the following are true:
      * Not connected.
      * Depth information is not available from this sensor.

    Behavior is undefined if the pixel location falls outside the extents
    of the buffer.
  */
  VSSBUTIL_API double         vssbut_GetPixelDistanceSquared( VsSharedBuffer bufferHandle, const struct vssbutPixelLocation* pixel );

  /** Returns the distance from the eye point to the specified pixel.
    Returns a value that is !Valid() if any of the following are true:
      * Not connected.
      * Depth information is not available from this sensor.

    Behavior is undefined if the pixel location falls outside the extents
    of the buffer.
  */
  VSSBUTIL_API double         vssbut_GetPixelDistance( VsSharedBuffer bufferHandle, const vssbutPixelLocation* pixel );

  /** Returns the direction in local/camera coordinates for the specified pixel.
    Returns a vector that is !Valid() if any of the following are true:
      * Not connected.
      * Normal information is not available from this sensor.

    Behavior is undefined if the pixel location falls outside the extents
    of the buffer.
  */
  VSSBUTIL_API vssbutEuclideanVector
                              vssbut_GetPixelNormal( VsSharedBuffer bufferHandle, const vssbutPixelLocation* pixel );

#if defined(__cplusplus)
} // extern "C"


namespace vssbut
{
  // C++ olverloaded IsValid() and Invalidate() functions for convienience:
  /// Returns true if the passed value is valid, returns false if it is not.
  inline bool                 IsValid(int value)                                  { return (0 != vssbut_GetApi()->IsValidInteger(value)); }  
  /// Sets the passed variable to a value that will return false from IsValid).
  inline void                 Invalidate(int* inout_value)                        { vssbut_GetApi()->InvalidateInteger(inout_value); }

  /// Returns true if the passed value is valid, returns false if it is not.
  inline bool                 IsValid(size_t value)                               { return (0 != vssbut_GetApi()->IsValidSize(value)); }  
  /// Sets the passed variable to a value that will return false from IsValid).
  inline void                 Invalidate(size_t* inout_value)                     { vssbut_GetApi()->InvalidateSize(inout_value); }

  /// Returns true if the passed value is valid, returns false if it is not.
  inline bool                 IsValid(double value)                               { return (0 != vssbut_GetApi()->IsValidDouble(value)); }
  /// Sets the passed variable to a value that will return false from IsValid).
  inline void                 Invalidate(double* inout_value)                     { vssbut_GetApi()->InvalidateDouble(inout_value);  }

  /// Returns true if the passed value is valid, returns false if it is not.
  inline bool                 IsValid(const vssbutPixelLocation& pixelLoc)        { return vssbut_IsValidPixelLocation(&pixelLoc); }
  /// Sets the passed variable to a value that will return false from IsValid).
  inline void                 Invalidate(vssbutPixelLocation* inout_pixelLoc)     { vssbut_InvalidatePixelLocation(inout_pixelLoc);  }

  /// Returns true if the passed value is valid, returns false if it is not.
  inline bool                 IsValid(const vssbutEuclideanVector& vect)          { return vssbut_IsValidVect(&vect); }
  /// Sets the passed variable to a value that will return false from IsValid).
  inline void                 Invalidate(vssbutEuclideanVector* inout_vect)       { vssbut_InvalidateVect(inout_vect);  }

  /// Returns true if the passed value is valid, returns false if it is not.
  inline bool                 IsValid(const vssbutOrientation& ori)               { return vssbut_IsValidOri(&ori); }
  /// Sets the passed variable to a value that will return false from IsValid).
  inline void                 Invalidate(vssbutOrientation* inout_ori)            { vssbut_InvalidateOri(inout_ori);  }
}


#endif  // __cplusplus

#endif  // _vs_SharedBuffer_Utility_h_INCLUDED
