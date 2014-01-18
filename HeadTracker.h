#ifndef __HEADTRACKER_H__
#define __HEADTRACKER_H__
////////////////////////////////////////////////////////////////////////////////
#include <stdio.h>
////////////////////////////////////////////////////////////////////////////////
typedef struct
{
    int active;                 // is the axis active
    int clamp;                  // do we clamp values ?
    double clampMin, clampMax;  // min and max clamp [-1, 1]
    double maxFreeTrackValue;   // max freetrack output value
} AxisInfo;
////////////////////////////////////////////////////////////////////////////////
AxisInfo axis[6];               // axes data
LPSTR moduleFolder = NULL;      // The folder within which this dll resides
LPVOID lpvSharedMemory = NULL;  // Pointer to shared memory map
HANDLE hFileMap = NULL;         // Handle to file mapping
HANDLE hMutex = NULL;           // Handle to shared memory map mutex
FreeTrackData ftData;           // FreeTrack data exchange variables
int lastDataID = -1;            // The dataID value of the last set of values
int freeTrackSharedMemInit = 0; // freetrack shared mem init is done ?
FILE* headTrackerLog = NULL;    // Log file for messages & error reporting
int logData = 0;                // Log input data (debug) ?
unsigned int iter = 0;          // headtracker frame counter
FILE* prefsFile = NULL;         // preference file
LPSTR prefsFilePath = NULL;     // preference file path
time_t prefsModifTime = 0;      // last preference file modif time
////////////////////////////////////////////////////////////////////////////////
#endif // __HEADTRACKER_H__
