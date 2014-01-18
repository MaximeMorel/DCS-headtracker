DCS-headtracker
===============

Implementation of DCS (Digital Combat Simulator) headtracker interface. To use with FreeTrack.

Headtracker dll interface for freetrack / opentrack and DCS World.

Using sources by samtheeagle in this post : http://forums.eagle.ru/showpost.php?p=1888102&postcount=147

Using the interface given here : http://forums.eagle.ru/showpost.php?p=1212342&postcount=91

Modifications by popux.

I made this plugin modification because I was annoyed by the crash when exiting the simulation. I also add some logging features to debug. And the ability to clamp the values of an axis in one direction or another.

How to :
Just as before, put HeadTracker.dll and HeadTracker.prefs in bin (relative to your root dcs world folder) (it was in bin/headtracker before DCS 1.2.6).
HeadTracker.prefs is not mandatory, if it's not here, default values will be used, which are :

```
1 0 -1.0 1.0
1 0 -1.0 1.0
1 0 -1.0 1.0
1 0 -1.0 1.0 500
1 0 -1.0 1.0 500
1 0 -1.0 1.0 500
0
```

Line 1, 2, 3 are for axis yaw, pitch and roll.

The 4 values mean :

* is axis active ?
* is axis clamped ?
* min clamp value
* max clamp value

Line 4, 5, 6 are for axis x, y and z.

The 4 values mean :

* is axis active ?
* is axis clamped ?
* min clamp value
* max clamp value
* max input value -> this value depends on your curves settings in FreeTrack

Line 7 is for logging. Put a '1' to enable full logging.

max input value is the maximum value given by freetrack. If we name it vmax, freetrack outputs values for each axis in the range [-vmax; vmax]. And it will be mapped to [-1; 1] for DCS.

Clamping can be used to restrain an axis in a given direction. For example the z axis, you might want to be able to go full backward, but not full forward, to be not too close from the A-10 MFD.
For this, line 6, you put :

1 1 -1.0 0.3 500

When running, a HeadTracker.log log file will be generated in the dll folder.

Dll was compiled with mingw64 toolchain in codeblocks and rewrote in pure C, so no dependencies with libstdc++ !

I only tested 64bit build, but 32bit build is also available.

It seems to work with opentrack too !

Have Fun !
