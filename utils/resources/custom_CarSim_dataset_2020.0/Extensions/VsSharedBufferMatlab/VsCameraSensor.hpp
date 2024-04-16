#pragma once

#include <stdint.h>
#include <memory>
#include <string>

#include "vs_SharedBuffer_Utility.h"


namespace vssbut
{
  extern const vssbutEuclideanVector  UNIT_FORWARD;  ///< The "forward" direction from the origin is in the +X direction (1,0,0).
  extern const vssbutEuclideanVector  UNIT_LEFTWARD; ///< The "left" direction from the origin is in the +Y direction (0,1,0).
  extern const vssbutEuclideanVector  UNIT_UPWARD;   ///< The "up" direction from the origin is in the +Z direction  (0,0,1).


  /** High-level "Camera Sensor" object interface. Use the factory
    method to create a concrete implementation object.
  */
  class VSSBUTIL_API VsCameraSensor
  {
  public:
  
    /** Creates an object of this class connected as a read-only client to the
      specified shared buffer that was created elsewhere (i.e. by another
      process).

      If the autoConnect parameter is true, an immediate attempt is made to
      connect, and auto-connect is enabled. When auto-connect is enabled, this
      object will periodically attempt to connect when it is in the
      disconnected state. The connection attempt can be set by using the
      SetConnectionServiceIntervalMs() method.

      If the autoConnect parameter is false, no connection attempt will be
      made. The returned object will be in the disconnected state. To connect
      call either Connect() or SetAutoConnect(true).

      In any case (auto-connect or not), a connection service thread is created
      that automatically disconnects from an underlying shared buffer whenever
      it is closed by it's owner (state == VSSBSTATE_DONE).
    */
    static std::shared_ptr<VsCameraSensor>
                              Create(   const wchar_t*  name                      ///< Case-sensitive name of Shared Buffer to connect to.
                                      , bool            autoConnect     = true    ///< If true, this object will periodically attempt to connect (when not already connected).
                                      , vssb_Result*    opt_out_result  = nullptr ///< Reports failure reason.
                                    );

    /** Returns the connection status of this camera sensor. If not
      connected, VSSBSTATE_UNDEFINED is returned, otherwise, the state
      of the underlying shared buffer is returned.
    */
    virtual vssb_State        GetStatus() const = 0;

    /** Enable or disable auto-connect with this method. If the autoConnect
      parameter is true, a connection attempt is made immediately (before
      this method returns).

      When auto-connect is enabled, the connection service thread will
      periodically attempt to connect whenever it is in the disconnected
      state. The interval between connection attempts can be set using the
      SetConnectionServiceIntervalMs() method.
    */
    virtual void              SetAutoConnect( bool autoConnect ) = 0;
    virtual bool              GetAutoConnect() const = 0;

    /** Immediately attempt to connect to the shared memory buffer.
    
      If the buffer is already connected, this method does nothing and
      returns VSSBR_OK.
      
      If the buffer is not already connected, the this method blocks until the
      buffer is successfully connected or the timeout expires, and the result
      of the connection attempt is returned.
    */
    virtual vssb_Result       Connect( int timeoutMilliseconds ) = 0;

    /** Disconnects from the shared memory buffer.
    
      NOTE: if auto-connect is enabled, the connection service thread will
      re-connect very quickly. If you intend to disconnect and leave this
      object disconnected, be sure to call SetAutoConnect(false) first.
    */
    virtual void              Disconnect() = 0;

    /// Returns the default update/reconnect interval.
    virtual int               GetDefaultConnectionServiceIntervalMs() const = 0;

    /** Sets the interval between auto connect/disconnect attempts. If the
      parameter is <= 0, the default (GetDefaultConnectionServiceIntervalMs())
      is used.
    */
    virtual void              SetConnectionServiceIntervalMs(int intervalMs) = 0;
    virtual int               GetConnectionServiceIntervalMs() const = 0;

    virtual bool              Lock() const = 0;               ///< Lock the buffer so that it cannot be changed (by another process).
    virtual bool              Unlock() const = 0;             ///< Unlock Lock the buffer so that it can be updated (by another process).
  
    virtual std::wstring      GetBuffername() const = 0;      ///< Returns the name of this sensor, or an empty string if it does not (yet) have a name.
    virtual vssb_ContentTypeMask
                              GetContentTypes() const = 0;    ///< Identifies what type of data is provided by this camera sensor.
    virtual int               GetWidth() const = 0;           ///< Returns a value that is !Valid() if not connected.
    virtual int               GetHeight() const = 0;          ///< Returns a value that is !Valid() if not connected.
  
    virtual int               GetFrameNumber() const = 0;     ///< Returns a value that is !Valid() if not connected.
    virtual double            GetFrameTime() const = 0;       ///< Returns a value that is !Valid() if not connected.
    virtual double            GetFovHorizRad() const = 0;     ///< Get the horizontal Field Of View in radians. Returns a value that is !Valid() if not connected.
    virtual double            GetFovVertRad() const = 0;      ///< Get the vertical Field Of View in radians. Returns a value that is !Valid() if not connected.
  
    virtual vssbutEuclideanVector
                              GetPosition() const = 0;        ///< Returns the position of this sensor in World space. Returns an invalid vector if not connected.
    virtual vssbutOrientation GetOrientation() const = 0;     ///< Returns the orientation of this sensor in World space. Returns an invalid orientation if not connected.
    virtual vssbutEuclideanVector
                              GetForwardVector() const = 0;   ///< Returns the "forward" (view direction) vector in World space. Returns an invalid vector if not connected.
    virtual vssbutEuclideanVector
                              GetLeftVector() const = 0;      ///< Returns the "left" vector for the camera sensor in World space. Returns an invalid vector if not connected.
    virtual vssbutEuclideanVector
                              GetUpVector() const = 0;        ///< Returns the "up" vector for the camera sensor in World space. Returns an invalid vector if not connected.
  
    /** Converts the passed location/direction specified in World coordinates to
      camera/local coordinates.
      Returns an invalid vector if not connected.
    */
    virtual vssbutEuclideanVector
                              WorldToLocal( const vssbutEuclideanVector& world ) const = 0;
  
    /** Converts the passed location/direction specified in local/camera coordinates to
      World coordinates.
      Returns an invalid vector if not connected.
    */
    virtual vssbutEuclideanVector
                              LocalToWorld( const vssbutEuclideanVector& local ) const = 0;

    virtual double            GetColumnAzimuthRad( int columnIndex ) const = 0;

    virtual double            GetRowElevationRad( int rowIndex ) const = 0;

    // TODO: These are not yet implemented:
    //  virtual int               GetColumnIndex( double azimuthAngleRad ) const = 0;
    //  virtual int               GetRowIndex( double elevationAngleRad ) const = 0;

    /** Returns the location of the pixel at the specified direction in local/camera
      coordinates. The returned location may not fall within the extents of the buffer.
      Returns an invalid location if any of the following are true:
        * Not connected
        * The specified direction does not intersect the viewing plane.
    */
    virtual vssbutPixelLocation
                              GetPixelLocation( const vssbutEuclideanVector& directionLocal ) const = 0;

    /** Returns a direction in local/camera coordinates of the center of the specified pixel.
      The returned vector is NOT necessarily a unit vector.
      Returns an invalid vector if not connected.
      Returns an invalid vector if the pixel location parameter is invalid.
    */
    virtual vssbutEuclideanVector
                              GetPixelDirection( const vssbutPixelLocation& pixelLocation ) const = 0;

    /** Returns the position of the specified pixel in local/camera coordinates.
      The returned location may not fall within the extents of the buffer.
      Returns an invalid location if any of the following are true:
        * Not connected
        * The specified direction does not intersect the viewing plane.
    */
    virtual vssbutEuclideanVector
                              GetPositionLocal( const vssbutPixelLocation& pixelLocation ) const = 0;

    /** Returns the color at the specified pixel location. 
      Returns "transparent black" (0,0,0,0) and sets out_opt_valid to false if
      any of the following are true:
        * Not connected.
        * Color information is not available.
        * Pixel location falls outside of the extents of the buffer.
        * Pixel location is invalid.

      If only RGB information is available (i.e. the buffer data format is
      VSSBCT_RGB), the alpha component will be set to 255 (opaque).
    */
    virtual vssbutColorBgra8  GetColor( const vssbutPixelLocation& pixel, bool* out_opt_valid = nullptr ) const = 0;
  
    /** Returns the depth from the sensor eye plane to the specified pixel.
      Returns a value that is !Valid() if any of the following are true:
        * Not connected.
        * Depth information is not available from this sensor.
        * Pixel location falls outside of the extents of the buffer.
        * Pixel location is invalid.
    */
    virtual double            GetDepth( const vssbutPixelLocation& pixel ) const = 0;

    /** Returns the depth from the sensor eye point to the specified pixel.
      Returns a value that is !Valid() if any of the following are true:
        * Not connected.
        * Depth information is not available from this sensor.
        * Pixel location falls outside of the extents of the buffer.
        * Pixel location is invalid.
    */
    virtual double            GetDistance( const vssbutPixelLocation& pixel ) const = 0;

    /** Returns the direction in local/camera coordinates for the specified pixel.
      Returns a vector that is !Valid() if any of the following are true:
        * Not connected.
        * Normal information is not available from this sensor.
        * Pixel location falls outside of the extents of the buffer.
        * Pixel location is invalid.
    */
    virtual vssbutEuclideanVector
                              GetNormal( const vssbutPixelLocation& pixel ) const = 0;

    /** Returns a new handle (a new "connection") to the currently connected
      buffer, or NULL if not currently connected.

      This creates a new connection to the shared buffer. It is the
      responsibility of the caller to properly manage and destroy this new
      connection to the underlying shared buffer (i.e. the caller must use the
      Shared Buffer Core Library function DestroySharedBuffer() function when
      done with the returned buffer).
      
      This is an advanced function should only be used if you _REALLY_ know
      what you're doing.
    */
    virtual VsSharedBuffer    GetSharedBuffer() const = 0;
  };

};    // end namespace.
