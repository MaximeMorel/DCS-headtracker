#ifndef __HEADTRACKERDLL_H__
#define __HEADTRACKERDLL_H__
////////////////////////////////////////////////////////////////////////////////
#ifdef HEADTRACKERDLL_EXPORTS
#define HEADTRACKERDLL_API __declspec(dllexport)
#else
#define HEADTRACKERDLL_API //__declspec(dllimport)
#endif
////////////////////////////////////////////////////////////////////////////////
#include <windows.h>
////////////////////////////////////////////////////////////////////////////////
typedef struct
{
    // version of HeadTrackerData (for future releases)
    double version;

    // axes values
    // values must be in range [-1; 1]
    double pitch;
    double yaw;
    double roll;
    double x;
    double y;
    double z;
} HeadTrackerData;
////////////////////////////////////////////////////////////////////////////////
typedef int (*InitHeadTrackerProc)(HWND);
typedef void (*ShutDownHeadTrackerProc)();
typedef void (*RequestHeadTrackerDataProc)(HeadTrackerData*);
typedef int (*GetHeadTrackerDataProc)(HeadTrackerData*);
////////////////////////////////////////////////////////////////////////////////
// device initialization
// returns true if initialization was successful
HEADTRACKERDLL_API int initHeadTracker(HWND hwnd);
////////////////////////////////////////////////////////////////////////////////
// device deinitialization
HEADTRACKERDLL_API void shutDownHeadTracker();
////////////////////////////////////////////////////////////////////////////////
// client informs headtracker which axes he would like to receive
// if axis value is 0.0 - client does not want to receive data from this axis,
// any other value means that client want to receive data from this axis
// also if headtracker cannot receive data from some axes,
// values ot these axes shuld be set to 0.0
// any other value means that headtracker able to recieve data from these axes
// this function calls before startHeadTracker()
HEADTRACKERDLL_API void requestHeadTrackerData(HeadTrackerData* data);
////////////////////////////////////////////////////////////////////////////////
// get tracker data
// this function called every frame
// client recieves only axes, described in requestData() call
// returns true, if struct data was filled
HEADTRACKERDLL_API int getHeadTrackerData(HeadTrackerData* data);
////////////////////////////////////////////////////////////////////////////////
#endif // __HEADTRACKERDLL_H__
