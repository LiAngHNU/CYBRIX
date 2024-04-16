#  Copyright (c) 2018-2019, Mechanical Simulation Corporation
# All rights reserved.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.

import argparse
import ctypes
from ctypes import cdll
from ctypes import *
import os
import struct
import platform

import sys


bufferName = 'EBS' # this can be provided when executing Python code.

path_to_sb_dll = 'vs_sb_64.dll'  # this file needs to present in the directory.
sb_dll = cdll.LoadLibrary(path_to_sb_dll)

class VsSharedBuffer(Structure):
    _fields_ = [
        ("mSharedMemory", ctypes.POINTER(ctypes.c_long)),
        ("mAccessType", ctypes.c_int),
        ("mSharedBufferMapFile", ctypes.POINTER(ctypes.c_int)),
        ("mSharedBufferMutex", ctypes.POINTER(ctypes.c_int))
    ]

class vssb_FrameInfo(Structure):
    _fields_ = [
        ("mFrameNumber", ctypes.c_int32),
        ("mFrameSimTime", ctypes.c_double),
        ("mFovHorizRad", ctypes.c_double),
        ("mFovVertRad",ctypes.c_double),
        ("mCamPosForward",ctypes.c_double),
        ("mCamPosLeftward",ctypes.c_double),
        ("mCamPosUpward",ctypes.c_double),
        ("mCamOriAxisForward",ctypes.c_double),
        ("mCamOriAxisLeftward",ctypes.c_double),
        ("mCamOriAxisUpward",ctypes.c_double),
        ("mCamOriAngle",ctypes.c_double)
    ]

IsValidIntFuncPtr = ctypes.CFUNCTYPE(ctypes.c_int, ctypes.c_int)
InvalidIntFuncPtr = ctypes.CFUNCTYPE(ctypes.c_int)
InvalidateIntFuncPtr = ctypes.CFUNCTYPE(None, ctypes.POINTER(ctypes.c_int))

IsValidSizeFuncPtr = ctypes.CFUNCTYPE(ctypes.c_int, ctypes.c_size_t)
InvalidSizeFuncPtr = ctypes.CFUNCTYPE(ctypes.c_size_t)
InvalidateSizeFuncPtr = ctypes.CFUNCTYPE(None, ctypes.POINTER(ctypes.c_size_t))

IsValidDoubleFuncPtr = ctypes.CFUNCTYPE(ctypes.c_int, ctypes.c_double)
InvalidDoubleFuncPtr = ctypes.CFUNCTYPE(ctypes.c_double)
InvalidateDoubleFuncPtr = ctypes.CFUNCTYPE(None, ctypes.POINTER(ctypes.c_double))

DescribeResultFuncPtr = ctypes.CFUNCTYPE(ctypes.POINTER(ctypes.c_char_p), ctypes.c_int)
DescribeStatusFuncPtr = ctypes.CFUNCTYPE(ctypes.POINTER(ctypes.c_char_p), ctypes.c_int)

GetBufferVersionSupportedFuncPtr = ctypes.CFUNCTYPE(ctypes.c_int, ctypes.c_int)
GetDefaultBufferVersionFuncPtr = ctypes.CFUNCTYPE(ctypes.c_int)

GetPageCountFuncPtr = ctypes.CFUNCTYPE(ctypes.c_size_t, ctypes.c_int, ctypes.c_int)
GetPageSizeFuncPtr = ctypes.CFUNCTYPE(ctypes.c_size_t, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int)
GetDataBufferSizeFuncPtr = ctypes.CFUNCTYPE(ctypes.c_size_t, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int)

CreateFuncPtr = ctypes.CFUNCTYPE(ctypes.POINTER(VsSharedBuffer), ctypes.c_wchar_p, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.POINTER(ctypes.c_int))
ConnectFuncPtr = ctypes.CFUNCTYPE(ctypes.POINTER(VsSharedBuffer), ctypes.c_wchar_p, ctypes.c_int, ctypes.POINTER(ctypes.c_int))
CloneConnectionFuncPtr = ctypes.CFUNCTYPE(ctypes.POINTER(VsSharedBuffer), ctypes.POINTER(VsSharedBuffer))
DisconnectFuncPtr = ctypes.CFUNCTYPE(None, ctypes.POINTER(ctypes.POINTER(VsSharedBuffer)))


LockFuncPtr = ctypes.CFUNCTYPE(ctypes.POINTER(c_int), ctypes.POINTER(VsSharedBuffer), ctypes.c_int)
UnLockFuncPtr = ctypes.CFUNCTYPE(ctypes.POINTER(c_int), ctypes.POINTER(VsSharedBuffer))

GetBufferFormatVersionFuncPtr = ctypes.CFUNCTYPE(ctypes.c_int, ctypes.POINTER(VsSharedBuffer))
GetContentTypesFuncPtr = ctypes.CFUNCTYPE(ctypes.c_long, ctypes.POINTER(VsSharedBuffer))

GetWidthPtr = ctypes.CFUNCTYPE(ctypes.c_size_t,ctypes.POINTER(VsSharedBuffer))
GetHeightPtr = ctypes.CFUNCTYPE(ctypes.c_size_t,ctypes.POINTER(VsSharedBuffer))

GetSharedBufferStateFuncPtr = ctypes.CFUNCTYPE(ctypes.c_int, ctypes.POINTER(VsSharedBuffer))
SetSharedBufferStateFuncPtr = ctypes.CFUNCTYPE(None, ctypes.POINTER(VsSharedBuffer), ctypes.c_int)

GetFrameNumberFuncPtr = ctypes.CFUNCTYPE(ctypes.c_int32,ctypes.POINTER(VsSharedBuffer))
FrameInfo = ctypes.POINTER(vssb_FrameInfo)

GetFrameInfoFuncPtr = ctypes.CFUNCTYPE(None,ctypes.POINTER(VsSharedBuffer),ctypes.POINTER(vssb_FrameInfo))
SetFrameInfoFuncPtr = ctypes.CFUNCTYPE(ctypes.POINTER(c_int), ctypes.POINTER(VsSharedBuffer),ctypes.POINTER(vssb_FrameInfo))
GetDataFuncPtr = ctypes.CFUNCTYPE(ctypes.POINTER(ctypes.c_ubyte), ctypes.POINTER(VsSharedBuffer), ctypes.c_int, ctypes.c_uint)
#GetDataFuncPtr = ctypes.CFUNCTYPE(ctypes.POINTER(ctypes.c_void_p), ctypes.POINTER(VsSharedBuffer), ctypes.c_int, ctypes.c_uint)

GetDataDepthFuncPtr = ctypes.CFUNCTYPE(ctypes.POINTER(ctypes.c_float), ctypes.POINTER(VsSharedBuffer), ctypes.c_int, ctypes.c_uint)

GetWritableDataFuncPtr = ctypes.CFUNCTYPE(ctypes.POINTER(ctypes.c_ubyte), ctypes.POINTER(VsSharedBuffer), ctypes.c_int, ctypes.c_uint)

class Vss_API(ctypes.Structure):
    _fields_ = [
        ("IsValidInteger", IsValidIntFuncPtr),
        ("InvalidInteger", InvalidIntFuncPtr),
        ("InvalidateInteger", InvalidateIntFuncPtr),
        ("IsValidSize", IsValidSizeFuncPtr),
        ("InvalidSize", InvalidSizeFuncPtr),
        ("InvalidateSize", InvalidateSizeFuncPtr),
        ("IsValidDouble", IsValidDoubleFuncPtr),
        ("InvalidDouble", InvalidDoubleFuncPtr),
        ("InvalidateDouble", InvalidateDoubleFuncPtr),
        ("DescribeResult", DescribeResultFuncPtr),
        ("DescribeStatus", DescribeStatusFuncPtr),
        ("GetBufferVersionSupported", GetBufferVersionSupportedFuncPtr),
        ("GetDefaultBufferVersion", GetDefaultBufferVersionFuncPtr),
        ("GetPageCount", GetPageCountFuncPtr),
        ("GetPageSize", GetPageSizeFuncPtr),
        ("GetDataBufferSize", GetDataBufferSizeFuncPtr),
        ("Create", CreateFuncPtr),
        ("Connect", ConnectFuncPtr),
        ("CloneConnection", CloneConnectionFuncPtr), # clone connection
        ("Disconnect", DisconnectFuncPtr), # disconnect
        ("GetBufferFormatVersion", GetBufferFormatVersionFuncPtr), # getbufferformatversion
        ("GetContentTypes", GetContentTypesFuncPtr), # getcontenttype
        ("GetWidth", GetWidthPtr), # getwidth
        ("GetHeight", GetHeightPtr), # get height
        ("Lock", LockFuncPtr), # lock
        ("UnLock", UnLockFuncPtr), # unlock
        ("GetSharedBufferState", GetSharedBufferStateFuncPtr), # getsharedbufferstate
        ("SetSharedBufferState", SetSharedBufferStateFuncPtr), # setsharedbufferstate
        ("GetFrameNumber", GetFrameNumberFuncPtr), # getframenumber
        ("GetFrameInfo",GetFrameInfoFuncPtr), # getframeinfo
        ("SetFrameInfo", SetFrameInfoFuncPtr), # setframeinfo
        ("GetData", GetDataFuncPtr), # getdata
        ("GetWritableData", GetWritableDataFuncPtr)
    ]

bufferapi = sb_dll.GetVsSharedBufferApi

bufferapi.argtypes=[ctypes.c_int]
bufferapi.restype = ctypes.POINTER(Vss_API)

api_ver2 = bufferapi(2)
api_ver2_contents = api_ver2.contents

out_result = ctypes.POINTER(ctypes.c_int)
open_result = out_result()

timeout_milliseconds = 22
sbHandle = api_ver2_contents.Connect(bufferName,timeout_milliseconds,open_result)

width = api_ver2_contents.GetWidth(sbHandle)
height = api_ver2_contents.GetHeight(sbHandle)

frameinfo = vssb_FrameInfo()

# this is how to call GetFrameInfo.
#api_ver2_contents.GetFrameInfo(sbHandle,frameinfo)
#print(frameinfo.mFrameSimTime)

# this is how to call Lock and UnLock.
#api_ver2_contents.Lock(sbHandle, 100)
#api_ver2_contents.UnLock(sbHandle)

VSSBCT_RGB = ctypes.c_long(1)
pageindex = ctypes.c_ulong(0)

VSSBCT_DEPTH = ctypes.c_long(4)

pixeldatatemp = api_ver2_contents.GetData(sbHandle, VSSBCT_RGB, pageindex)
depthdatatemp = api_ver2_contents.GetData(sbHandle, VSSBCT_DEPTH, pageindex)

pixeldata = cast(pixeldatatemp, ctypes.POINTER(ctypes.c_ubyte))
#print(newpixceldata[0])
#print(newpixceldata[1])
#print(newpixceldata[2])
depthdata = cast(depthdatatemp, ctypes.POINTER(ctypes.c_float))
#print(newdepthdata[0])


#print(cast(pixeldata, ctypes.POINTER(ctypes.c_ubyte))[0])
#print(pixeldata[0])
#print(pixeldata[1])
#print(pixeldata[2])
#print(depthdata[0])
#t8 = struct.pack('Q',depthdata[0])
#print(struct.unpack('d', t8)[0])





