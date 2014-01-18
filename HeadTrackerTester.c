////////////////////////////////////////////////////////////////////////////////
#include "freetrack_shared_mem.h"
#include "HeadTrackerDll.h"
#include <stdio.h>
////////////////////////////////////////////////////////////////////////////////
extern LPSTR moduleFolder;
extern FILE* headTrackerLog;
////////////////////////////////////////////////////////////////////////////////
int main(int argc, char** argv)
{
    moduleFolder = malloc(2*sizeof(*moduleFolder));
    moduleFolder[0] = '.';
    moduleFolder[1] = '\0';
    HWND hwnd;
    initHeadTracker(hwnd);
    headTrackerLog = stdout;
    HeadTrackerData data;
    requestHeadTrackerData(&data);

    for(;;)
    {
        getHeadTrackerData(&data);
        Sleep(500);
    }

    shutDownHeadTracker();

    return 0;
}
////////////////////////////////////////////////////////////////////////////////
