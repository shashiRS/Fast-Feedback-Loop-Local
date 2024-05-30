Common Pitfalls {#common_pitfalls}
=====

# Failed to transform Plantuml to XMI
### Known Occurrence: Linux

When building a project component it might happen that eco throws this issue:

"Failed to transform PlantUML to xmi"

One solution might be, that Java is not installed on your system or outdated.

How to install/update Java on Linux:
```
sudo apt update
sudo apt install openjdk-11-jdk
```

You can validate the installation with:

```
java -version
```

Which should look something like this: 
```
openjdk version "11.0.21" 2023-10-17
OpenJDK Runtime Environment (build 11.0.21+9-post-Ubuntu-0ubuntu120.04)
OpenJDK 64-Bit Server VM (build 11.0.21+9-post-Ubuntu-0ubuntu120.04, mixed mode, sharing)
```

---


# Wrong Byte Alignment for project component 
### Known Occurrence: Windows, Linux

Currently, for running simulation components it is needed to change the byte alignment to properly run the component.
This can be done in the SDL file for the respective component.

The SDL file can be found in:

***<repo>/conan_workarea/package.<component_name>.<variant>/bin/<component_name>.sdl***

E.g.: si_core\conan_workarea\package.si_core.generic.13.0.0-fallback.vs2017\bin\sicoregeneric.sdl

The **Byte Alignment** can be found in the second line. Usually the default value is **ByteAlignment="0"** and needs to be set to "1".

Example for updated sicoregeneric.sdl:
```
<?xml version="1.0" encoding="UTF-8"?>
<SdlFile xmlns:xsd="http://www.w3.org/2001/XMLSchema-instance" xsd:noNamespaceSchemaLocation="sdl2-rc1-before-after.xsd" ByteAlignment="1" Version="2.0">
<!-- SDL Generated From Cobolt Tool Chain  !-->
    <View Name="SiCoreGeneric" CycleID="100">
        <Group Name="m_environmentModelPort" Address="0" ArrayLen="1" Size="3524">
        <Signal Name="uiVersionNumber" Offset="0" ArrayLen="1" Type="ulong" Bitmask="ffffffff" ByteOrder="little-endian" Size="4"/>
        <SubGroup Name="sSigHeader" Offset="8" ArrayLen="1" Size="16">
            <Signal Name="uiTimeStamp" Offset="0" ArrayLen="1" Type="uint64" Bitmask="ffffffffffffffff" ByteOrder="little-endian" Size="8"/>
            <Signal Name="uiMeasurementCounter" Offset="8" ArrayLen="1" Type="ushort" Bitmask="ffff" ByteOrder="little-endian" Size="2"/>
            <Signal Name="uiCycleCounter" Offset="A" ArrayLen="1" Type="ushort" Bitmask="ffff" ByteOrder="little-endian" Size="2"/>
            <Signal Name="eSigStatus" Offset="C" ArrayLen="1" Type="uchar" Bitmask="ff" ByteOrder="big-endian" Size="1"/>
        </SubGroup>
        <Signal Name="numberOfStaticObjects_u8" Offset="18" ArrayLen="1" Type="uchar" Bitmask="ff" ByteOrder="little-endian" Size="1"/>
        <Signal Name="numberOfDynamicObjects_u8" Offset="19" ArrayLen="1" Type="uchar" Bitmask="ff" ByteOrder="little-endian" Size="1"/>
        <SubGroup Name="resetOriginResult" Offset="1C" ArrayLen="1" Size="16">
            <SubGroup Name="originTransformation" Offset="0" ArrayLen="1" Size="12">
                <Signal Name="x_dir" Offset="0" ArrayLen="1" Type="float" Bitmask="ffffffff" ByteOrder="little-endian" Size="4"/>
                <Signal Name="y_dir" Offset="4" ArrayLen="1" Type="float" Bitmask="ffffffff" ByteOrder="little-endian" Size="4"/>
                <Signal Name="yaw_rad" Offset="8" ArrayLen="1" Type="float" Bitmask="ffffffff" ByteOrder="little-endian" Size="4"/>
            </SubGroup>
...
```