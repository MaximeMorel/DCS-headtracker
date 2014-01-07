/*******************************************************************************
 *  freetrack_shared_mem.h
 *
 *  This header defines the interface to access the freetrack data stored
 *  in a shared memory. To access the data you will need to use the mutex
 *  and memory map names defined below.
 *
 *  Inspired by the freetrack_c_interface.cpp file created by Alastair Moore,
 *  December 2007.
 *
 *  File created by Guy Webb, June 2011.
 *  This file is available for anyone to use, but please give credit where
 *  credit is due :)
 *
 ******************************************************************************/
#ifndef __FREETRACK_SHARED_MEM_H__
#define __FREETRACK_SHARED_MEM_H__
////////////////////////////////////////////////////////////////////////////////
#include <windows.h>
////////////////////////////////////////////////////////////////////////////////
// FreeTrackData structured.
// As this struct only uses the float and int data types it should be safe to pass
// between 32bit and 64bit processes as the size of these data types does not change
// between the two process types.
typedef struct
{
  int dataID;
  int camWidth;
  int camHeight;
  // Virtual pose data
  float yaw;
  float pitch;
  float roll;
  float x;
  float y;
  float z;
  // Raw pose data, no smoothing, sensitivity, response curves etc.
  float rawYaw;
  float rawPitch;
  float rawRoll;
  float rawX;
  float rawY;
  float rawZ;
  // Raw points, sorted by Y, origin top left corner
  float x1;
  float y1;
  float x2;
  float y2;
  float x3;
  float y3;
  float x4;
  float y4;
} FreeTrackData;
////////////////////////////////////////////////////////////////////////////////
// The size of the FreeTrackData structure.
const int FREETRACK_DATA_SIZE = sizeof(FreeTrackData);
////////////////////////////////////////////////////////////////////////////////
// The name of the shared memory map where an instance of FreeTrackData resides.
const LPCTSTR SHARED_MEMORY_MAP_NAME = TEXT("FT_SharedMem");
////////////////////////////////////////////////////////////////////////////////
// The name of the mutex that controls access to the shared memory map.
const LPCTSTR MUTEX_NAME = TEXT("FT_Mutext");
////////////////////////////////////////////////////////////////////////////////
#endif // __FREETRACK_SHARED_MEM_H__
