////////////////////////////////////////////////////////////////////////////////
// HeadTrackerDll.c
//
// Implementation of the ED Head Tracker API that makes use of the data
// placed into a shared memory area by FreeTrack.
//
// Based on the reference sources supplied by ED. See here:
// http://forums.eagle.ru/showpost.php?p=1212342&postcount=91
//
// File created by Guy Webb, June 2011.
// This file is available for anyone to use, but please give credit where
// credit is due :)
//
// Modified by Maxime Morel after the work of samtheeagle
// http://forums.eagle.ru/showpost.php?p=1888102&postcount=147
//
////////////////////////////////////////////////////////////////////////////////
#include "freetrack_shared_mem.h"
#include "HeadTrackerDll.h"
#include "HeadTracker.h"
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
////////////////////////////////////////////////////////////////////////////////
#ifdef HEADTRACKERDLL_EXPORTS
#define NBITER 100 // if compiling dll
#else
#define NBITER 1 // if compiling tester exe
#endif
////////////////////////////////////////////////////////////////////////////////
BOOL APIENTRY DllMain(HANDLE hModule, DWORD dwReason, LPVOID lpReserved)
{
    // When the dll is first loaded find the folder in which it resides.
    if (dwReason == DLL_PROCESS_ATTACH)
    {
        //FILE* logdll = fopen("headtracker_init.log", "w");
        //fprintf(logdll, "HeadTracker dll attached.\n");fflush(logdll);
        LPSTR moduleFileName = malloc(_MAX_PATH*sizeof(*moduleFileName));moduleFileName[0] = '\0';
        moduleFolder = malloc(_MAX_PATH*sizeof(*moduleFolder));moduleFolder[0] = '\0';

        if (!GetModuleFileName((HMODULE)hModule, moduleFileName, _MAX_PATH))
        {
            // If the module path was not retrieved set a default value.
            strcpy(moduleFolder, "./bin/headtracker");
            //fprintf(logdll, "moduleFolder : not found, ./bin/headtracker by default.");fflush(logdll);
        }
        else
        {
            // Otherwise remove the filename part from the fully qualified path.
            strcpy(moduleFolder, moduleFileName);
            int i;
            for (i=strlen(moduleFolder); i>=0; --i)
            {
                if (moduleFolder[i] == '\\')
                {
                    moduleFolder[i] = '\0';
                    break;
                }
            }
            //fprintf(logdll, "moduleFolder : found.\n");fflush(logdll);
        }
        //fprintf(logdll, "moduleFolder : %s\n", moduleFolder);fflush(logdll);

        free(moduleFileName);

        //fprintf(logdll, "HeadTracker dll attach end.\n");fflush(logdll);
        //fclose(logdll);

        return TRUE;
    }

    return FALSE;
}
////////////////////////////////////////////////////////////////////////////////
// init freetrack sharedmem access
BOOL initFreeTrackSharedMem()
{
    // Open the named file mapping object.
    if (hFileMap != NULL)
    {
        return TRUE; // already initialized
    }

    hFileMap = OpenFileMapping(
        FILE_MAP_ALL_ACCESS,                               // read/write access
        FALSE,                                           // do not inherit the name
        SHARED_MEMORY_MAP_NAME);                        // name of map object
    if (hFileMap == NULL)
    {
        fprintf(headTrackerLog, "ERROR: Failed to open file mapping. FreeTrack not running ?\n");fflush(headTrackerLog);
        shutDownHeadTracker();
        return FALSE;
    }

    // Get a pointer to the file-mapped shared memory.
    lpvSharedMemory = MapViewOfFile(
        hFileMap,                                         // object to map view of
        FILE_MAP_ALL_ACCESS,                            // read/write permission
        0,                                              // high offset: map from
        0,                                              // low offset: beginning
        FREETRACK_DATA_SIZE);
    if (lpvSharedMemory == NULL)
    {
        fprintf(headTrackerLog, "ERROR: Failed to map shared memory\n");fflush(headTrackerLog);
        shutDownHeadTracker();
        return FALSE;
    }

    // Open the shared memory access control mutex.
    hMutex = OpenMutex(
        MUTEX_ALL_ACCESS,                               // desired security attributes
        FALSE,                                             // don't inherit handle
        MUTEX_NAME);                                      // mutex name
    if (hMutex == NULL)
    {
        fprintf(headTrackerLog, "Failed to open mutex\n");fflush(headTrackerLog);
        shutDownHeadTracker();
        return FALSE;
    }

    fprintf(headTrackerLog, "HeadTracker Initialized\n");fflush(headTrackerLog);

    freeTrackSharedMemInit = 1;

    return TRUE;
}
////////////////////////////////////////////////////////////////////////////////
// Set defaults values
void loadDefaultPrefs()
{
    // set defaults values
    axis[0].active = 1; axis[0].clamp = 0; axis[0].clampMin = -1.0; axis[0].clampMax = 1.0; axis[0].maxFreeTrackValue = M_PI; // yaw
    axis[1].active = 1; axis[1].clamp = 0; axis[1].clampMin = -1.0; axis[1].clampMax = 1.0; axis[1].maxFreeTrackValue = M_PI; // pitch
    axis[2].active = 1; axis[2].clamp = 0; axis[2].clampMin = -1.0; axis[2].clampMax = 1.0; axis[2].maxFreeTrackValue = M_PI; // roll
    axis[3].active = 1; axis[3].clamp = 0; axis[3].clampMin = -1.0; axis[3].clampMax = 1.0; axis[3].maxFreeTrackValue = 500; // x
    axis[4].active = 1; axis[4].clamp = 0; axis[4].clampMin = -1.0; axis[4].clampMax = 1.0; axis[4].maxFreeTrackValue = 500; // y
    axis[5].active = 1; axis[5].clamp = 0; axis[5].clampMin = -1.0; axis[5].clampMax = 1.0; axis[5].maxFreeTrackValue = 500; // z
    logData = 0;
}
////////////////////////////////////////////////////////////////////////////////
// Load prefs from HeadTracker.prefs file
void loadPrefsFile()
{
    if (!prefsFile)
    {
        prefsFile = fopen(prefsFilePath, "r");
    }

    if (prefsFile)
    {
        prefsFile = freopen(prefsFilePath, "r", prefsFile);

        fscanf(prefsFile, "%d %d %lf %lf", &axis[0].active, &axis[0].clamp, &axis[0].clampMin, &axis[0].clampMax); // yaw
        fscanf(prefsFile, "%d %d %lf %lf", &axis[1].active, &axis[1].clamp, &axis[1].clampMin, &axis[1].clampMax); // pitch
        fscanf(prefsFile, "%d %d %lf %lf", &axis[2].active, &axis[2].clamp, &axis[2].clampMin, &axis[2].clampMax); // roll
        fscanf(prefsFile, "%d %d %lf %lf %lf", &axis[3].active, &axis[3].clamp, &axis[3].clampMin, &axis[3].clampMax, &axis[3].maxFreeTrackValue); // x
        fscanf(prefsFile, "%d %d %lf %lf %lf", &axis[4].active, &axis[4].clamp, &axis[4].clampMin, &axis[4].clampMax, &axis[4].maxFreeTrackValue); // y
        fscanf(prefsFile, "%d %d %lf %lf %lf", &axis[5].active, &axis[5].clamp, &axis[5].clampMin, &axis[5].clampMax, &axis[5].maxFreeTrackValue); // z
        fscanf(prefsFile, "%d", &logData);
    }
}
////////////////////////////////////////////////////////////////////////////////
// Check modification date of prefs file
int checkPrefsFileModif()
{
    struct stat buf;
    stat(prefsFilePath, &buf);
    if (buf.st_mtime > prefsModifTime)
    {
        prefsModifTime = buf.st_mtime;
        return 1;
    }

    return 0;
}
////////////////////////////////////////////////////////////////////////////////
// Log prefs info
void dumpPrefsInfo()
{
    fprintf(headTrackerLog, "Axis info\n");
    fprintf(headTrackerLog, "yaw : active : %d, clamp : %d, min clamp : %lf, max clamp %lf, max freetrack output : %lf\n", axis[0].active, axis[0].clamp, axis[0].clampMin, axis[0].clampMax, axis[0].maxFreeTrackValue);
    fprintf(headTrackerLog, "pitch : active : %d, clamp : %d, min clamp : %lf, max clamp %lf, max freetrack output : %lf\n", axis[1].active, axis[1].clamp, axis[1].clampMin, axis[1].clampMax, axis[1].maxFreeTrackValue);
    fprintf(headTrackerLog, "roll : active : %d, clamp : %d, min clamp : %lf, max clamp %lf, max freetrack output : %lf\n", axis[2].active, axis[2].clamp, axis[2].clampMin, axis[2].clampMax, axis[2].maxFreeTrackValue);
    fprintf(headTrackerLog, "x : active : %d, clamp : %d, min clamp : %lf, max clamp %lf, max freetrack output : %lf\n", axis[3].active, axis[3].clamp, axis[3].clampMin, axis[3].clampMax, axis[3].maxFreeTrackValue);
    fprintf(headTrackerLog, "y : active : %d, clamp : %d, min clamp : %lf, max clamp %lf, max freetrack output : %lf\n", axis[4].active, axis[4].clamp, axis[4].clampMin, axis[4].clampMax, axis[4].maxFreeTrackValue);
    fprintf(headTrackerLog, "z : active : %d, clamp : %d, min clamp : %lf, max clamp %lf, max freetrack output : %lf\n", axis[5].active, axis[5].clamp, axis[5].clampMin, axis[5].clampMax, axis[5].maxFreeTrackValue);
    fprintf(headTrackerLog, "log input data : %d\n", logData);fflush(headTrackerLog);
}
////////////////////////////////////////////////////////////////////////////////
// device initialization
// returns true if initialization was successful
HEADTRACKERDLL_API int initHeadTracker(HWND hwnd)
{
    // Create a log file for the HeadTracker.
    LPSTR logFilePath = malloc(_MAX_PATH*sizeof(*logFilePath));logFilePath[0] = '\0';
    strcpy(logFilePath, moduleFolder);
    strcat(logFilePath, "\\HeadTracker.log");
    headTrackerLog = fopen(logFilePath, "w");
    fprintf(headTrackerLog, "INFO: HeadTracker.log location: %s\n", logFilePath);fflush(headTrackerLog);
    free(logFilePath);

    // Load translation bounds from HeadTracker.prefs file.
    prefsFilePath = malloc(_MAX_PATH*sizeof(*prefsFilePath));prefsFilePath[0] = '\0';
    strcpy(prefsFilePath, moduleFolder);
    strcat(prefsFilePath, "\\HeadTracker.prefs");
    fprintf(headTrackerLog, "INFO: HeadTracker.prefs location: %s\n", prefsFilePath);fflush(headTrackerLog);

    loadDefaultPrefs();
    prefsFile = fopen(prefsFilePath, "r");
    if (prefsFile)
    {
        checkPrefsFileModif(); // this will initialize prefs file modif time
        loadPrefsFile();
    }
    else
    {
        fprintf(headTrackerLog, "WARNING: Default parameters used.\n");fflush(headTrackerLog);
    }

    dumpPrefsInfo();

    initFreeTrackSharedMem();

    return 1;
}
////////////////////////////////////////////////////////////////////////////////
// device deinitialization
HEADTRACKERDLL_API void shutDownHeadTracker()
{
    fprintf(headTrackerLog, "HeadTracker start Shut Down\n");fflush(headTrackerLog);

    // Clean up the mutex.
    if (hMutex)
    {
        CloseHandle(hMutex); hMutex = NULL;
    }
    fprintf(headTrackerLog, "HeadTracker Shut Down : close mutex\n");fflush(headTrackerLog);

    // Unmap shared memory from the process's address space.
    if (lpvSharedMemory)
    {
        UnmapViewOfFile(lpvSharedMemory); lpvSharedMemory = NULL;
    }
    fprintf(headTrackerLog, "HeadTracker Shut Down : close shared mem\n");fflush(headTrackerLog);

    // Close the process's handle to the file-mapping object.
    if (hFileMap)
    {
        CloseHandle(hFileMap); hFileMap = NULL;
    }
    fprintf(headTrackerLog, "HeadTracker Shut Down : close mmap\n");fflush(headTrackerLog);

    fprintf(headTrackerLog, "HeadTracker Shut Down\n");fflush(headTrackerLog);
    fclose(headTrackerLog);

    fclose(prefsFile);
    free(prefsFilePath);
    free(moduleFolder);
}
////////////////////////////////////////////////////////////////////////////////
// client informs headtracker which axes he would like to receive
// if axis value is 0.0 - client does not want to receive data from this axis,
// any other value means that client wants to receive data from this axis
// also if headtracker cannot receive data from some axes,
// values of these axes shuold be set to 0.0
// any other value means that headtracker is able to recieve data from these axes
// this function calls before startHeadTracker()
HEADTRACKERDLL_API void requestHeadTrackerData(HeadTrackerData* data)
{
    fprintf(headTrackerLog, "requestHeadTrackerData : \n");
    fprintf(headTrackerLog, "yaw : %d\n", axis[0].active);
    fprintf(headTrackerLog, "pitch : %d\n", axis[1].active);
    fprintf(headTrackerLog, "roll : %d\n", axis[2].active);
    fprintf(headTrackerLog, "x : %d\n", axis[3].active);
    fprintf(headTrackerLog, "y : %d\n", axis[4].active);
    fprintf(headTrackerLog, "z : %d\n", axis[5].active);
    fflush(headTrackerLog);

    // Report supported axes
    data->yaw = axis[0].active;
    data->pitch = axis[1].active;
    data->roll = axis[2].active;
    data->x = axis[3].active;
    data->y = axis[4].active;
    data->z = axis[5].active;
}
////////////////////////////////////////////////////////////////////////////////
void processValue(AxisInfo* axis, float* inData, double* outData, float factor)
{
    if (axis->active)
    {
        *outData = (*inData / axis->maxFreeTrackValue) * factor;
        if (axis->clamp)
        {
            if (*outData < axis->clampMin) *outData = axis->clampMin;
            if (*outData > axis->clampMax) *outData = axis->clampMax;
        }
    }
}
////////////////////////////////////////////////////////////////////////////////
// get tracker data
// this function called every frame
// client receives only axes, described in setDataSources() call
HEADTRACKERDLL_API int getHeadTrackerData(HeadTrackerData* data)
{
    ++iter;
    if (freeTrackSharedMemInit == 0 && iter%NBITER == 0) // try to reinit each 100 frames (~2 sec)
    {
        initFreeTrackSharedMem();
        return 0;
    }

    if (iter%(NBITER*5) == 0) // recheck prefs file for modifications each 500 frames (~10 sec)
    {
        if (checkPrefsFileModif() == 1)
        {
            fprintf(headTrackerLog, "Prefs needs reloading.\n");fflush(headTrackerLog);
            loadPrefsFile();
            dumpPrefsInfo();
        }
    }

    //fprintf(headTrackerLog, "getHeadTrackerData\n");fflush(headTrackerLog);

    // Try to access the mutex.
    DWORD dwWaitResult = WaitForSingleObject(hMutex, 0L);
    switch (dwWaitResult)
    {
        // The mutex was signaled.
        case WAIT_OBJECT_0:
            // Read from shared memory.
            memcpy(&ftData, lpvSharedMemory, FREETRACK_DATA_SIZE);
            // Release the mutex when the memory is copied.
            ReleaseMutex(hMutex);
        case WAIT_FAILED:
            break;
        case WAIT_TIMEOUT:
            break;
        default:
            break;
    }

    // If the data id has not changed then do nothing. I'm guessing that this value is updated everytime freetrack
    // supplies a new set of data values. This will hopefully work around issues found when tracking is disabled.
    if (lastDataID == ftData.dataID)
    {
        return 0;
    }
    lastDataID = ftData.dataID;

    // Rotation values come out of FreeTrack in radians, with a max value of PI
    // which equates to 180 degrees (depending on user curves).
    // So to scale to -1 .. +1 simply divide the value by PI.
    processValue(&axis[0], &ftData.yaw, &data->yaw, 1);
    processValue(&axis[1], &ftData.pitch, &data->pitch, -1);
    processValue(&axis[2], &ftData.roll, &data->roll, 1);

    // Translation values are more difficult, as they seemed to be returned in
    // units of millimeters, and the range is entirely dependent upon the specific
    // FreeTrack setup. Use the values loaded from the HeadTracker.prefs file.
    processValue(&axis[3], &ftData.x, &data->x, 1);
    processValue(&axis[4], &ftData.y, &data->y, 1);
    processValue(&axis[5], &ftData.z, &data->z, 1);

    if (logData)
    {
        fprintf(headTrackerLog, "id    : %d\n", ftData.dataID);
        fprintf(headTrackerLog, "yaw   : %lf  ->  %lf\n", ftData.yaw, data->yaw);
        fprintf(headTrackerLog, "pitch : %lf  ->  %lf\n", ftData.pitch, data->pitch);
        fprintf(headTrackerLog, "roll  : %lf  ->  %lf\n", ftData.roll, data->roll);
        fprintf(headTrackerLog, "x     : %lf  ->  %lf\n", ftData.x, data->x);
        fprintf(headTrackerLog, "y     : %lf  ->  %lf\n", ftData.y, data->y);
        fprintf(headTrackerLog, "z     : %lf  ->  %lf\n\n", ftData.z, data->z);
        fflush(headTrackerLog);
    }

    return 1;
}
////////////////////////////////////////////////////////////////////////////////
