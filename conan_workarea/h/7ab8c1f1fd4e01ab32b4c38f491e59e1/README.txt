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
   D:\TATA\LATEST_NXT_REPO\next\conan_workarea\h\7ab8c1f1fd4e01ab32b4c38f491e59e1 with
  D:\TATA\LATEST_NXT_REPO\next\conan_workarea\build.next.delivery.0.0.0-fallback.vs2017\cip_build
