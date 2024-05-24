Developer Extra {#bridges_developer_extra}
======

[TOC]

# TODO stashed extra material

##### Database creation

One of the glue components of the Next Bridges is the [Next UDex](https://github-am.geo.conti.de/ADAS/Next-UDex).

The [Next UDex](https://github-am.geo.conti.de/ADAS/Next-UDex) in its turn, requires a database to be setup in a specific location.

This database, is recording-specific. This means that every time we load a different recording in the Next Player, the database is redone.

The database needs to be created for the first time, in order for the Next UDex to perform its operations. This is done by simply loading a recording into the Next Player.

UDex DB setup: `md c:\Users\*uid*\mts\signal_storage`; and `./Next_Player_Player.exe -r D:/recordings/2022.02.14_at_12.48.46_radar-mi_663_ext_00.01.29.491.027_00.05.29.999.022.rrec`






TODO:

1. V Explain the bridges working, what are the outputs, where to see the log
2. V Run the player
3. V Run the bridges. Explain ports
4. V Run the GUI. Explain configuration (ports)
5. V Explain GUI widgets configuration loading
6. V Load a recording using the control bar/control bridge/player 
7. XV Note to explain db of udex generation, wait for loading in player, etc
8. V Play open-loop simulation, visualize in 3D visualizer/table-view
9. Explain superficially plugins configuration loading, example with TP (BASE)







___

TODO: extra player options for database creation

Start and load a recording:

```cmd
./Next_Player_Player.exe -r D:/recordings/2022.02.14_at_12.48.46_radar-mi_663_ext_00.01.29.491.027_00.05.29.999.022.rrec
```






___

## Closed loop simulation

TODO: continue from above, go into detail of closed-loop simulation of nodes, re-simulation of data visualized in the 3D visualizer/table-view

##### Build CEM200

[CEM200](https://github-am.geo.conti.de/ADAS/CEM200) provides the core libraries for the simulation, needed for both open-loop and closed-loop simulation.

The Next Player can operate in both modes, hence the requirement as a dependency.

Clone repo:

```cmd
git clone git@github-am.geo.conti.de:ADAS/CEM200.git
```

Build:

```cmd
cd CEM200
py ./scripts/cip.py build  --platform vs2017 --variant gen1_ecal --cip-version 4.11.0
```

> WARNING: the CEM200 build takes long and requires some user input for sub-modules download, this is to be expected

Once it's finally done, manually copy eCAL config files

```cmd
cp ./CEM200/eCal_config/* ./CEM200/conan_workarea/package.cem200.gen1_ecal.13.0.0-fallback.vs2017/bin/
```

Congratulations, the hardest step is done :)


___

#### Post-build

Copy the contents of the bin folder of Next Player into the CEM200:

```cmd
cp -r <Next-Player>/conan_workarea/package.Next_Player.generic.0.0.0-fallback.vs2017/bin/* <CEM200>/conan_workarea/package.cem200.gen1_ecal.13.0.0-fallback.vs2017/bin/
```

`<Next-Player>` and `<CEM200>` are the folders where both repos are located.


___

# Developer zone {#howto_developer}

TODO: move to different page (?)

## Build debug version

The conan script (`<Next-Bridges>/scripts/cip.py`) has many options to build and deploy the project. Typically, the built binaries are located in the `<Next-Bridges>/conan_workarea/` folder.

Depending on the platform chosen, 2 main types of project is built: `vs2017` and `vs2017_debug`. These refer to the release optimized and debug optimized builds, respectively.

The Visual Studio solution that should be used for debugging, corresponds to the build platform `vs2017_debug`. In this case, the command for building the debug version is:
```cmd
py .\scripts\cip.py build --platform vs2017_debug
```

After this, the Visual Studio solution should be located in `<Next-Bridges>/conan_workarea/build.Next_Bridges.generic.0.0.0-fallback.vs2017/vs_sln.bat`

Open it to start debugging using Visual Studio 2017.

## Debug session in Visual Studio 2017

After a build, 2 folders are created for each platform: `build` and `package`.

The `package` contains the binaries, while the `build` contains the symbols.

After the solution is opened, in the Solution Explorer select which component you desire for debugging. The `databridge` is the *main* one.

After setting the solution as `StartUp Project`, in the `Debug->databridge Properties...` menu, choose `Configuration Properties->Debugging` on the left pane, and point the `Working Directory` to `<Next-Bridges>/conan_workarea/package.Next_Bridges.generic.0.0.0-fallback.vs2017/bin` as well as the `Command` to the databridge exe, e.g: `<Next-Bridges>\conan_workarea\package.Next_Bridges.generic.0.0.0-fallback.vs2017\bin\databridge.exe`

<img src="howto_databridge_vs2017_config.png" width="644">
