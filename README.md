DCS-headtracker
===============

Implementation of DCS (Digital Combat Simulator) headtracker interface. To use with freetrack.

Headtracker dll using freetrack for dcs world
Using sources by samtheeagle in this post : http://forums.eagle.ru/showpost.php?p=1234317&postcount=28
modifications by popux

I made this plugin modification because i was annoyed by the crash when exiting the simulation.

How to :
Just as before, put HeadTracker.dll and HeadTracker.prefs in bin/headtracker (relative to your root dcs world folder).
HeadTracker.prefs is not mandatory, if it's not here, default values will be used, which are :
1 -> activated yaw axis
1 -> activated pitch axis
1 -> activated roll axis
1 -> activated x axis
1 -> activated y axis
1 -> activated z axis
500 -> maximum x translation freetrack output value
200 -> maximum y translation freetrack output value
500 -> maximum z translation freetrack output value
0 -> log input data ? (debug purpose)

A HeadTracker.log log file will be generated in this folder.
Dll was compiled with mingw toolchain in codeblocks and rewrote in pure C, so no dependencies with libstdc++ !
I only tested 64bit build, but 32bit build is also available.

Have Fun !
