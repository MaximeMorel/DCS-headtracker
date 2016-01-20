////////////////////////////////////////////////////////////////////////////////
#include "freetrack_shared_mem.h"
#include "HeadTrackerDll.h"
#include <stdio.h>
////////////////////////////////////////////////////////////////////////////////
extern LPSTR moduleFolder;
extern FILE* headTrackerLog;
////////////////////////////////////////////////////////////////////////////////
#define UNUSED(x) (void)(x)
////////////////////////////////////////////////////////////////////////////////
int main(int argc, char** argv)
{
    UNUSED(argc);
    UNUSED(argv);
    moduleFolder = malloc(2 * sizeof(*moduleFolder));
    moduleFolder[0] = '.';
    moduleFolder[1] = '\0';
    HWND hwnd = 0;
    initHeadTracker(hwnd);
    headTrackerLog = stdout;
    HeadTrackerData data;
    requestHeadTrackerData(&data);

    for (;;)
    {
        getHeadTrackerData(&data);
        Sleep(500);
    }

    shutDownHeadTracker();

    return 0;
}
////////////////////////////////////////////////////////////////////////////////
