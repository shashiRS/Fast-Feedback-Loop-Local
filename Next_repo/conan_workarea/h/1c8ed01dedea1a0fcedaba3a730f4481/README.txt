The 'h' folder contains links to the more human readable
build folders done with a Windows directory junction.

We are doing this to reduce the path lengths during builds
so that we don't hit the Windows path length limit so
quickly.
This is why you will see only the 'h' folders in the build
logs.

If we come up with a better solution we might remove it
again so don't rely on it.
This is a linked folder (directory junction) and links
   D:\Next-python_API\next\conan_workarea\h\1c8ed01dedea1a0fcedaba3a730f4481 with
  D:\Next-python_API\next\conan_workarea\build.next.generic.0.0.0-fallback.vs2017\cip_build
