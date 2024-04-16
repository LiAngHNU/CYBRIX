#ifndef _vs_SharedBufferFormat_h_INCLUDED
#define _vs_SharedBufferFormat_h_INCLUDED


#include <stdlib.h>
#include <stdint.h>


# ifdef _WIN32
#   if defined( VSSHAREDBUFFER_MAKINGDLL )
#     if defined( VSSHAREDBUFFER_STATICLIB ) || defined( VSSHAREDBUFFER_USINGDLL )
#       #error "Multiple linkage types specified. Only one may be defined."
#     endif
#     define VSSBIMPORTEXPORT   __declspec(dllexport)
#   elif defined( VSSHAREDBUFFER_STATICLIB )
#     if defined( VSSHAREDBUFFER_USINGDLL )
#       #error "Multiple linkage types specified. Only one may be defined."
#     endif
#     define VSSBIMPORTEXPORT
# 	else /* Not making DLL, or using static libs... must be using DLL. */
#     if !defined( VSSHAREDBUFFER_USINGDLL )
#       define VSSHAREDBUFFER_USINGDLL
#     endif
#     define VSSBIMPORTEXPORT   __declspec(dllimport)
#   endif
#   define VSSB_DECLSPEC_2
#   define VSSB_ALIGN_1 _declspec(align(8))
#   define VSSB_ALIGN_2
# else
#   define VSSBIMPORTEXPORT
#   define VSSB_DECLSPEC_2 __attribute__((visibility("default")))
#   define __cdecl
#   define VSSB_ALIGN_1
#   define VSSB_ALIGN_2 __attribute__((aligned(8),packed))
# endif


/** A variable of type VsSharedBuffer is an "opaque" pointer to an undefined
  struct. This pointer uniquely identifies a "SharedBuffer object" (it is a
  "handle" to the object), but code that uses this library has no information
  about the structure, so you cannot directly modify or extract information
  about it. Access to the object's processes and to the data that it contains
  are available only via the API, using the functions that operate on a
  shared buffer handle (a variable of type VsSharedBuffer).

  The only way to create a valid VsSharedBuffer handle object is to call one
  of the functions that returns a VsSharedBuffer.
*/
typedef struct VsSharedBufferStruct*  VsSharedBuffer;


typedef enum
{
  VSSBR_ERROR_UNSUPPORTED_PATFORM   = -16,  ///< The operation is not supported on this platform.
  VSSBR_ERROR_MEMORY                = -15,  ///< Failed to allocate necessary memory.
  VSSBR_ERROR_EXITING               = -14,  ///< The shared buffer is in the process of being shut down/closed.
  VSSBR_RESERVED_FOR_FUTURE_USE_00  = -13,
  VSSBR_ERROR_DOESNOTEXIST          = -12,  ///< The requested resource does not exist.
  VSSBR_ERROR_NORIGHTS              = -11,  ///< The caller does not have the right to perform the requested action.
  VSSBR_ERROR_TIMEOUT               = -10,  ///< Operation timed out.
  VSSBR_ERROR_SYNC                  = -9,   ///< Failure of buffer access synchronization.
  VSSBR_ERROR_INTERNAL              = -8,   ///< An internal error prevents the operation from succeeding.
  VSSBR_ERROR_BUFFER_VERSION        = -7,   ///< The version of the shared buffer you are trying to use is incompatible with the API/Library.
  VSSBR_ERROR_LICENSE               = -6,   ///< The operation cannot be completed because the required license is not active.
  VSSBR_ERROR_ALREADYEXISTS         = -5,   ///< The specified name is already in use.
  VSSBR_ERROR_INVALIDNAME           = -4,   ///< The specified name is not valid.
  VSSBR_ERROR_INVALIDSIZE           = -3,   ///< The specified size is not valid.
  VSSBR_ERROR_INVALIDINPUT          = -2,   ///< An input/parameter is not valid.
  VSSBR_ERROR                       = -1,   ///< Generic error.
  //-----------------------
  VSSBR_OK                          = 0,    ///< Success.
  //-----------------------
  VSSBR_WARNING                     = 1,    ///< Generic warning.
} vssb_Result;


typedef enum
{
  VSSBCT_UNDEFINED    = -1,
  VSSBCT_NONE         = 0,
  VSSBCT_FIRST_VALID  = 1,
  //==========================
  VSSBCT_RGB          = 1 << 0,
  VSSBCT_BGRA         = 1 << 1,
  VSSBCT_DEPTH        = 1 << 2,
  VSSBCT_DISTANCE     = VSSBCT_DEPTH, ///< DEPRECATED - Use VSSBCT_DEPTH instead.
  VSSBCT_NORMAL       = 1 << 3,
  //==========================
  ___MEANINGLESS_VALUE_DO_NOT_USE___,
  VSSBCT_LAST_VALID   = ___MEANINGLESS_VALUE_DO_NOT_USE___ - 1,
} vssb_ContentTypeMask;


typedef enum
{
  VSSBSTATE_UNDEFINED = 0,  ///< The buffer has not yet been configured by it's owner.
  VSSBSTATE_ACTIVE    = 1,  ///< The buffer is configured and is actively being updated by it's owner.
  VSSBSTATE_DONE      = 2,  /**< The buffer is no longer active. The source of this buffer has closed
                                it and it will never be updated again. When a buffer is found to be
                                in this state, it should be immediately closed and it's resources
                                released using DestroySharedBuffer(). */
} vssb_State;


/** Misc. info and stats related to the current frame of data.
*/
typedef struct VSSB_ALIGN_1 vssb_FrameInfo_V2
{
  int32_t   mFrameNumber;         /**< The current frame number, which is to be changed by the owner each time
                                    the data in the shared buffer is changed.
                                    This can be used to check if new data is available without locking the buffer.
                                    If the frame count is negative, it means that a valid frame has not been drawn (i.e.
                                    the shared buffer(s) contain invalid/junk data).
                                  */
  double    mFrameSimTime;        ///< Simulation time of the data frame (or 0.0 if time is unavailable).
  double    mFovHorizRad;         ///< Horizontal Field of View of the image (radians).
  double    mFovVertRad;          ///< Vertical Field of View of the image (radians).
  double    mCamPosForward;       ///< Position of the camera (VehicleSim X).
  double    mCamPosLeftward;      ///< Position of the camera (VehicleSim Y).
  double    mCamPosUpward;        ///< Position of the camera (VehicleSim Z).
  double    mCamOriAxisForward;   ///< The forward (VehicleSim X) component of the rotation axis vector of the axis-angle representation of the camera orientation.
  double    mCamOriAxisLeftward;  ///< The leftward (VehicleSim Y) component of the axis axis vector of the axis-angle representation of the camera orientation.
  double    mCamOriAxisUpward;    ///< The upward (VehicleSim Z) component of the axis axis vector of the axis-angle representation of the camera orientation.
  double    mCamOriAngle;         ///< The rotation angle (radians) of the axis-angle representation of the camera orientation.
} VSSB_ALIGN_2 vssb_FrameInfo_V2;

/** A structure that contains function pointers to the API functions of the
  version 2 API. Retrieve a set of function pointers by calling
  GetVsSharedBufferApi(2), then use the returned pointer to call the API
  functions contained in this struct. For example:

  \code{.cpp}
    vssb_Api_V2* sbApi = GetVsSharedBufferApi(2);
    VsSharedBuffer mySharedBuffer = sbApi->CreateSharedBuffer("Mine!", 640, 480, VSSBCT_RGB, 0, 0);
    vssb_State myBuffersState = sbApi->GetSharedBufferState(mySharedBuffer);
    sbApi->DestroySharedBuffer(&mySharedBuffer);
  \endcode
*/
typedef struct vssb_Api_V2
{
  //////////////////////////////////////////////////////////////////////////////
  // Utility functions which don't need a valid Shared Buffer handle:
  //////////////////////////////////////////////////////////////////////////////

  /// Returns 0 if the passed value is invalid, returns non-zero if the value is valid.
  int                         (__cdecl *IsValidInteger)(int i);

  /// Returns an invalid integer value (i.e. IsValidInteger() is gauranteed to return 0 for any value returned from this function).
  int                         (__cdecl *InvalidInteger)();

  /// Changes the passed variable such that it will return 0 from IsValidInteger().
  void                        (__cdecl *InvalidateInteger)(int* out_variableToInvalidate);

  /// Returns 0 if the passed value is invalid, returns non-zero if the value is valid.
  int                         (__cdecl *IsValidSize)(size_t i);

  /// Returns an invalid size value (i.e. IsValidSize() is gauranteed to return 0 for any value returned from this function).
  size_t                      (__cdecl *InvalidSize)();

  /// Changes the passed variable such that it will return 0 from IsValidSize().
  void                        (__cdecl *InvalidateSize)(size_t* out_variableToInvalidate);

  /// Returns 0 if the passed value is invalid, returns non-zero if the value is valid.
  int                         (__cdecl *IsValidDouble)(double x);

  /// Returns an invalid double value (i.e. IsValidDouble() is gauranteed to return 0 for any value returned from this function).
  double                      (__cdecl *InvalidDouble)();

  /// Changes the passed variable such that it will return 0 from IsValidDouble().
  void                        (__cdecl *InvalidateDouble)(double* out_variableToInvalidate);

  /// Returns a descriptive (English) string for the passed result.
  const wchar_t*              (__cdecl *DescribeResult)(vssb_Result result);

  /// Returns a descriptive (English) string for the passed status.
  const wchar_t*              (__cdecl *DescribeStatus)(vssb_State status);

  /** Returns non-zero if the specified shared buffer format version is
    supported by this API, returns zero if not.
  */
  int                         (__cdecl *GetBufferVersionSupported)(int bufferFormatVersion);

  /** Returns the default shared buffer format version used by this version
    of this API. This is the buffer format version that will be used when
    calling CreateSharedBuffer() with 0 as the buffer format parameter.
  */
  int                         (__cdecl *GetDefaultBufferVersion)();

  /** Returns the number of pages the data is comprised of. For example,
    a shared color buffer has a single page of data which contains RGB or BGRA
    color values, whereas a shared buffer containing surface normals contains
    three (3) separate pages of data - one each for the X, Y, and Z components
    of the normal vector.

    The bufferType parameter must contain a single valid content type, and the
    specified buffer version must be supported, otherwise the behavior of this
    function is undefined.
  */
  size_t                      (__cdecl *GetPageCount)(    vssb_ContentTypeMask  singleContentType
                                                        , int                   bufferFormatVersion  );

  /** Returns the size of a single page of data of the specified type.

    The bufferType parameter must contain a single valid content type, and the
    specified buffer version must be supported, otherwise the behavior of this
    function is undefined.
  */
  size_t                      (__cdecl *GetPageSize)(   int                     width
                                                      , int                     height
                                                      , vssb_ContentTypeMask    singleContentType
                                                      , int                     bufferFormatVersion   );

  /*** Returns the size of a data buffer of the specified size and type.
    Specify 0 for the sharedBufferFormat version to use the default version.

    Behavior of this function is undefined if any of the parameters are
    invalid, which includes (but is not necessarily limited to):
      * Width or height is <= 0
      * Width or height is an unreasonably large value.
      * Specified content type parameter is not a single content type (i.e. a mask containing multiple set bits).
      * Specified content type is not unrecognized.
      * The shared buffer format version specified is not supported (and is not 0).
  */
  size_t                      (__cdecl *GetDataBufferSize)(   int                   width
                                                            , int                   height
                                                            , vssb_ContentTypeMask  singleContentType
                                                            , int                   bufferFormatVersion );

  //////////////////////////////////////////////////////////////////////////////
  // Shared Buffer object functions:
  //////////////////////////////////////////////////////////////////////////////

  /** Creates a shared buffer large enough to hold the specified amount of data.
    You must give a unique name (that is not already in use by any other named
    shared memory on the system), and a dataSize that is greater than zero.
    returns VSSBR_OK on success and populates out_sharedBuffer parameter with
    a handle that can be used in subsequent calls to the API.

    Note that the shared memory buffer version is unrelated to the API version.

    If bufferFormatVersion is 0, the default buffer version will be used. You
    should specify 0 for the buffer version unless you are certain that you
    know what you're doing and that you want to create a buffer of an older
    version.

    Note: Call Disconnect() when done with this shared buffer. The shared
    memory resources will be automatically deleted when all processes have
    disconnected from it.
  */
  VsSharedBuffer              (__cdecl *Create)(    const wchar_t*const   name
                                                  , int                   width
                                                  , int                   height
                                                  , vssb_ContentTypeMask  contentTypes
                                                  , int                   bufferFormatVersion
                                                  , vssb_Result*          out_result          );

  /** Opens (connects to) a shared buffer that was created elsewhere (i.e. by another process). */
  VsSharedBuffer              (__cdecl *Connect)(const wchar_t*const name, int timeoutMilliseconds, vssb_Result* out_result);

  /** Creates a new read-only connection based on the existing connection. The
    caller is responsible for destroying the returned object (i.e. calling
    Disconnect() on the returned handle).

    The source/original/passed connection handle is unchanged by this call. You
    must still Disconnect() for it when it is no longer in use.
  */
  VsSharedBuffer              (__cdecl *CloneConnection)(VsSharedBuffer bufferHandle);

  /** Releases all resources associated with this shared buffer for this
    process and invalidates the passed handle.

    This call does NOT not delete the actual shared memory buffer or other
    system resources if they are still in use by other processes.
  */
  void                        (__cdecl *Disconnect)(VsSharedBuffer* inout_bufferHandle);

  /** Returns the version of the shared memory buffer.

    Note that the version of the shared memory buffer is unrelated to the API version.
  */
  int                         (__cdecl *GetBufferFormatVersion)(VsSharedBuffer bufferHandle);

  /**  Returns a bit mask containing the type(s) of content that the
    shared buffer contains.
  */
  vssb_ContentTypeMask        (__cdecl *GetContentTypes)(VsSharedBuffer bufferHandle);

  size_t                      (__cdecl *GetWidth)(VsSharedBuffer bufferHandle);

  size_t                      (__cdecl *GetHeight)(VsSharedBuffer bufferHandle);

  /** Call to lock the buffer for exclusive access. Blocks until exclusive access is granted,
    an error occurs, or the timeout expires. Pass -1 for infinite timeout. Pass 0 for non-
    blocking behavior (returns immediately whether or not the lock was acquired).

    Returns:
    VSSBR_ERROR_TIMEOUT - The timeout period elapsed before exclusive access was obtained.
    <any other error> - Something unexpected happened. Exclusive access is not granted.
    VSSBR_OK - Exclusive lock is granted (don't forget to call Unlock() when done!).
  */
  vssb_Result                 (__cdecl *Lock)(VsSharedBuffer bufferHandle, int timeoutMilliseconds);

  /** Release exclusive lock on data buffer. Returns VSSBR_OK on success.
  */
  vssb_Result                 (__cdecl *Unlock)(VsSharedBuffer bufferHandle);

  /** Returns the current state of the buffer */
  vssb_State                  (__cdecl *GetSharedBufferState)(VsSharedBuffer bufferHandle);

  /** Sets the state of the data. This should only be called by the owner. */
  void                        (__cdecl *SetSharedBufferState)(VsSharedBuffer bufferHandle, vssb_State state);

  /** Returns the current frame number, which is changed each time a new frame is drawn.
    This can be used to check if new data is available without locking the buffer.
    If the frame count is negative, it means that a valid frame has not been drawn (i.e.
    the shared buffer(s) contain invalid/junk data).
  */
  int32_t                     (__cdecl *GetFrameNumber)(VsSharedBuffer bufferHandle);

  /** Fills the passed structure with information about the current frame of
    data. The data is NOT guaranteed to be valid.

    Multi-thread/multi-process synchronization is the responsibility of the
    caller. Any call to this function should be contained between calls to
    Lock() and Unlock() to ensure that all of the contents of the
    returned structure are updated atomically (i.e. ensure all data copied to
    the passed structure is from the same frame).
  */
  void                        (__cdecl *GetFrameInfo)(VsSharedBuffer bufferHandle, struct vssb_FrameInfo_V2* out_frameInfo);

  /** Copies the contents of the passed structure to the shared buffer.

    Only the process which created the shared memory buffer is allowed to
    call this function.

    The data in the returned structure is NOT guaranteed to be valid.

    Multi-thread/multi-process synchronization synchronization is the
    responsibility of the caller. Any call to this function should be contained
    between calls to Lock() and Unlock() (along with any
    code that updates the shared memory data buffer). This will ensure that
    another process calling GetFrameInfo() will wait until all data is
    updated and read consistent data.
  */
  vssb_Result                 (__cdecl *SetFrameInfo)(VsSharedBuffer bufferHandle, const struct vssb_FrameInfo_V2* frameInfo);

  /** Returns a pointer to where the shared data can be read. This is for
    read-only access. Writing to this area will result in undefined behavior.
    This pointer will be valid until the buffer is destroyed.

    To ensure maximum performance, none of the parameters are checked for
    validity. It is the responsibility of the caller to ensure that all of
    the parameters are valid and compatible with the shared buffer. Passing
    invalid/incompatible parameters will result in undefined behavior.

    The singleContentType parameter must contain a single valid content type,
    otherwise the behavior of this function is undefined.
  */
  volatile const unsigned char*
                              (__cdecl *GetData)(VsSharedBuffer bufferHandle, vssb_ContentTypeMask singleContentType, unsigned int pageIndex);

  /** Returns a pointer to where the shared data can be written to. Only the
    process which creates the shared memory buffer is allowed to write to it.
    If client processes call this method, it will return NULL.
    This pointer will be valid until the buffer is destroyed.

    To ensure maximum performance, none of the parameters are checked for
    validity. It is the responsibility of the caller to ensure that all of
    the parameters are valid and compatible with the shared buffer. Passing
    invalid/incompatible parameters will result in undefined behavior.

    The singleContentType parameter must contain a single valid content type,
    otherwise the behavior of this function is undefined.
  */
  volatile unsigned char*     (__cdecl *GetWritableData)(VsSharedBuffer bufferHandle, vssb_ContentTypeMask singleContentType, unsigned int pageIndex);

} vssb_Api_V2;


#if defined(__cplusplus)
extern "C"
{
#endif

/** Retrieves a pointer to the API of the specified version, NULL if the
  specified version is not supported/available.

  GetDefaultVsSharedBufferApi() is preferred over this function unless there is
  a reason that you need a specific version of the API.

  Note: The version of the shared memory buffer is unrelated to the API version.

  Note: This is the only function exported from this DLL. Use it to
  get function pointers to the API functions.
*/
VSSBIMPORTEXPORT const void VSSB_DECLSPEC_2 * __cdecl
                              GetVsSharedBufferApi(int apiVersion);

#if defined(__cplusplus)
} // extern "C"
#endif


/// The latest/default/preferred version of the API.
typedef vssb_Api_V2         vssb_Api;

/// Typedef of the FrameInfo structure version that corresponds to vssb_Api.
typedef vssb_FrameInfo_V2   vssb_FrameInfo;

/// Returns API version number of vssb_Api (the latest/default/preferred version of the API).
inline const int              GetDefaultApiVersion()  { return 2;}

/// Retrieves the latest version of the API, or NULL if API is not available.
inline const vssb_Api*        GetDefaultVsSharedBufferApi() { return (vssb_Api*)GetVsSharedBufferApi(GetDefaultApiVersion()); }


#endif    // _vs_SharedBufferFormat_h_INCLUDED
