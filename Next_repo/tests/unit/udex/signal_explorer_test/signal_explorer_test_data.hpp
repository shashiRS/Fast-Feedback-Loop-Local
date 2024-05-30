#pragma once

#include <next/udex/explorer/signal_explorer.hpp>

namespace next {
namespace udex {
namespace test {

std::string_view SDL_CONTENT = R"(
<SdlFile xmlns:xsd="http://www.w3.org/2001/XMLSchema" ByteAlignment="1" Version="2.0">
	<View Name="AlgoVehCycle" CycleID="207">
		<Group Name="VehDyn" Address="20350000" ArrayLen="1" Size="160">
			<Signal Name="uiVersionNumber" Offset="0" ArrayLen="1" Type="ulong" Bitmask="ffffffff" ByteOrder="big-endian" Size="4"/>
			<SubGroup Name="sSigHeader" Offset="4" ArrayLen="1" Size="12">
				<Signal Name="uiTimeStamp" Offset="0" ArrayLen="1" Type="ulong" Bitmask="ffffffff" ByteOrder="big-endian" Size="4"/>
				<Signal Name="uiMeasurementCounter" Offset="4" ArrayLen="1" Type="ushort" Bitmask="ffff" ByteOrder="big-endian" Size="2"/>
				<Signal Name="uiCycleCounter" Offset="6" ArrayLen="1" Type="ushort" Bitmask="ffff" ByteOrder="big-endian" Size="2"/>
				<Signal Name="eSigStatus" Offset="8" ArrayLen="1" Type="uchar" Bitmask="ff" ByteOrder="big-endian" Size="1"/>
				<Signal Name="a_reserve" Offset="9" ArrayLen="3" Type="uchar" Bitmask="ff" ByteOrder="big-endian" Size="1"/>
			</SubGroup>
			<SubGroup Name="Longitudinal" Offset="10" ArrayLen="1" Size="36">
				<Signal Name="Velocity" Offset="0" ArrayLen="1" Type="float" Bitmask="ffffffff" ByteOrder="big-endian" Size="4"/>
				<Signal Name="VelocityTimestamp" Offset="4" ArrayLen="1" Type="ulong" Bitmask="ffffffff" ByteOrder="big-endian" Size="4"/>
				<Signal Name="VelocityRaw" Offset="8" ArrayLen="1" Type="float" Bitmask="ffffffff" ByteOrder="big-endian" Size="4"/>
				<Signal Name="Accel" Offset="C" ArrayLen="1" Type="float" Bitmask="ffffffff" ByteOrder="big-endian" Size="4"/>
				<Signal Name="AccelTimestamp" Offset="10" ArrayLen="1" Type="ulong" Bitmask="ffffffff" ByteOrder="big-endian" Size="4"/>
				<Signal Name="varVelocity" Offset="14" ArrayLen="1" Type="float" Bitmask="ffffffff" ByteOrder="big-endian" Size="4"/>
				<Signal Name="varAccel" Offset="18" ArrayLen="1" Type="float" Bitmask="ffffffff" ByteOrder="big-endian" Size="4"/>
				<Signal Name="VelCorrFact" Offset="1C" ArrayLen="1" Type="float" Bitmask="ffffffff" ByteOrder="big-endian" Size="4"/>
				<Signal Name="VelocityCorrectionQuality" Offset="20" ArrayLen="1" Type="uchar" Bitmask="ff" ByteOrder="big-endian" Size="1"/>
			</SubGroup>
			<SubGroup Name="Lateral" Offset="34" ArrayLen="1" Size="92">
				<SubGroup Name="YawRate" Offset="0" ArrayLen="1" Size="20">
					<Signal Name="YawRate" Offset="0" ArrayLen="1" Type="float" Bitmask="ffffffff" ByteOrder="big-endian" Size="4"/>
					<Signal Name="YawRateTimestamp" Offset="4" ArrayLen="1" Type="ulong" Bitmask="ffffffff" ByteOrder="big-endian" Size="4"/>
					<Signal Name="YawRateRaw" Offset="8" ArrayLen="1" Type="float" Bitmask="ffffffff" ByteOrder="big-endian" Size="4"/>
					<Signal Name="YawAngle" Offset="C" ArrayLen="1" Type="float" Bitmask="ffffffff" ByteOrder="big-endian" Size="4"/>
					<Signal Name="Variance" Offset="10" ArrayLen="1" Type="float" Bitmask="ffffffff" ByteOrder="big-endian" Size="4"/>
				</SubGroup>
				<SubGroup Name="Curve" Offset="14" ArrayLen="1" Size="20">
					<Signal Name="Curve" Offset="0" ArrayLen="1" Type="float" Bitmask="ffffffff" ByteOrder="big-endian" Size="4"/>
					<Signal Name="CurveTimestamp" Offset="4" ArrayLen="1" Type="ulong" Bitmask="ffffffff" ByteOrder="big-endian" Size="4"/>
					<Signal Name="VarCurve" Offset="8" ArrayLen="1" Type="float" Bitmask="ffffffff" ByteOrder="big-endian" Size="4"/>
					<Signal Name="CrvError" Offset="C" ArrayLen="1" Type="float" Bitmask="ffffffff" ByteOrder="big-endian" Size="4"/>
					<Signal Name="CrvConf" Offset="10" ArrayLen="1" Type="uchar" Bitmask="ff" ByteOrder="big-endian" Size="1"/>
				</SubGroup>
				<SubGroup Name="DrvIntCurve" Offset="28" ArrayLen="1" Size="16">
					<Signal Name="Curve" Offset="0" ArrayLen="1" Type="float" Bitmask="ffffffff" ByteOrder="big-endian" Size="4"/>
					<Signal Name="CurveTimestamp" Offset="4" ArrayLen="1" Type="ulong" Bitmask="ffffffff" ByteOrder="big-endian" Size="4"/>
					<Signal Name="Variance" Offset="8" ArrayLen="1" Type="float" Bitmask="ffffffff" ByteOrder="big-endian" Size="4"/>
					<Signal Name="Gradient" Offset="C" ArrayLen="1" Type="float" Bitmask="ffffffff" ByteOrder="big-endian" Size="4"/>
				</SubGroup>
				<SubGroup Name="Accel" Offset="38" ArrayLen="1" Size="12">
					<Signal Name="LatAccel" Offset="0" ArrayLen="1" Type="float" Bitmask="ffffffff" ByteOrder="big-endian" Size="4"/>
					<Signal Name="LatAccelTimestamp" Offset="4" ArrayLen="1" Type="ulong" Bitmask="ffffffff" ByteOrder="big-endian" Size="4"/>
					<Signal Name="Variance" Offset="8" ArrayLen="1" Type="float" Bitmask="ffffffff" ByteOrder="big-endian" Size="4"/>
				</SubGroup>
				<SubGroup Name="SlipAngle" Offset="44" ArrayLen="1" Size="8">
					<Signal Name="SideSlipAngle" Offset="0" ArrayLen="1" Type="float" Bitmask="ffffffff" ByteOrder="big-endian" Size="4"/>
					<Signal Name="Variance" Offset="4" ArrayLen="1" Type="float" Bitmask="ffffffff" ByteOrder="big-endian" Size="4"/>
				</SubGroup>
				<SubGroup Name="SelfSteering" Offset="4C" ArrayLen="1" Size="16">
					<Signal Name="RoadBankAngle" Offset="0" ArrayLen="1" Type="float" Bitmask="ffffffff" ByteOrder="big-endian" Size="4"/>
					<Signal Name="QuRoadBankAngle" Offset="4" ArrayLen="1" Type="float" Bitmask="ffffffff" ByteOrder="big-endian" Size="4"/>
					<Signal Name="SelfSteerGradEst" Offset="8" ArrayLen="1" Type="float" Bitmask="ffffffff" ByteOrder="big-endian" Size="4"/>
					<Signal Name="QuSelfSteerGradEst" Offset="C" ArrayLen="1" Type="float" Bitmask="ffffffff" ByteOrder="big-endian" Size="4"/>
				</SubGroup>
			</SubGroup>
			<SubGroup Name="MotionState" Offset="90" ArrayLen="1" Size="3">
				<Signal Name="MotState" Offset="0" ArrayLen="1" Type="uchar" Bitmask="ff" ByteOrder="big-endian" Size="1"/>
				<Signal Name="Confidence" Offset="1" ArrayLen="1" Type="uchar" Bitmask="ff" ByteOrder="big-endian" Size="1"/>
				<Signal Name="bRollerTestBench" Offset="2" ArrayLen="1" Type="uchar" Bitmask="ff" ByteOrder="big-endian" Size="1"/>
			</SubGroup>
			<Signal Name="State" Offset="93" ArrayLen="12" Type="uchar" Bitmask="ff" ByteOrder="big-endian" Size="1"/>
		</Group>
	</View>
  <View Name="array_root" CycleID="2">
		<Group Name="simple_float_arrays" Address="90100000" ArrayLen="1" Size="80">
			<Signal Name="ten_floats" Offset="0" ArrayLen="10" Type="float" Bitmask="ffffffff" ByteOrder="little-endian" Size="4"/>
			<Signal Name="ten_more_floats" Offset="28" ArrayLen="10" Type="float" Bitmask="ffffffff" ByteOrder="little-endian" Size="4"/>
		</Group>
		<Group Name="array_of_structs" Address="901103E8" ArrayLen="1" Size="88">
			<SubGroup Name="first_group" Offset="0" ArrayLen="5" Size="12">
				<Signal Name="big_int" Offset="0" ArrayLen="1" Type="uint64" Bitmask="ffffffffffffffff" ByteOrder="little-endian" Size="8"/>
				<Signal Name="unsigned_short" Offset="8" ArrayLen="1" Type="ushort" Bitmask="ffff" ByteOrder="little-endian" Size="2"/>
				<Signal Name="another_unsigned_short" Offset="A" ArrayLen="1" Type="ushort" Bitmask="ffff" ByteOrder="little-endian" Size="2"/>
			</SubGroup>
			<SubGroup Name="second_group" Offset="3C" ArrayLen="7" Size="4">
				<Signal Name="unsigned_char1" Offset="0" ArrayLen="1" Type="uchar" Bitmask="ff" ByteOrder="big-endian" Size="1"/>
				<Signal Name="unsigned_char2" Offset="1" ArrayLen="1" Type="uchar" Bitmask="ff" ByteOrder="big-endian" Size="1"/>
				<Signal Name="unsigned_char3" Offset="2" ArrayLen="1" Type="uchar" Bitmask="ff" ByteOrder="big-endian" Size="1"/>
				<Signal Name="unsigned_char4" Offset="3" ArrayLen="1" Type="uchar" Bitmask="ff" ByteOrder="big-endian" Size="1"/>
			</SubGroup>
		</Group>
		<Group Name="array_of_array" Address="90110440" ArrayLen="1" Size="336">
			<SubGroup Name="array_group" Offset="0" ArrayLen="10" Size="21">
				<Signal Name="unsigned_short_array" Offset="0" ArrayLen="10" Type="ushort" Bitmask="ffff" ByteOrder="little-endian" Size="2"/>
				<Signal Name="unsigned_char" Offset="14" ArrayLen="1" Type="uchar" Bitmask="ff" ByteOrder="little-endian" Size="1"/>
			</SubGroup>
			<SubGroup Name="second_group" Offset="D2" ArrayLen="7" Size="18">
				<Signal Name="unsigned_char1" Offset="0" ArrayLen="6" Type="uchar" Bitmask="ff" ByteOrder="big-endian" Size="1"/>
				<Signal Name="unsigned_char2" Offset="6" ArrayLen="5" Type="uchar" Bitmask="ff" ByteOrder="big-endian" Size="1"/>
				<Signal Name="unsigned_char3" Offset="B" ArrayLen="4" Type="uchar" Bitmask="ff" ByteOrder="big-endian" Size="1"/>
				<Signal Name="unsigned_char4" Offset="F" ArrayLen="3" Type="uchar" Bitmask="ff" ByteOrder="big-endian" Size="1"/>
			</SubGroup>
		</Group>
	</View>
</SdlFile> )";

std::string_view SDL_CONTENT2 = R"(
<SdlFile xmlns:xsd="http://www.w3.org/2001/XMLSchema" ByteAlignment="1" Version="2.0">
	<View Name="AlgoVehCycle1" CycleID="207">
		<Group Name="VehDyn" Address="20350000" ArrayLen="1" Size="160">
			<Signal Name="uiVersionNumber" Offset="0" ArrayLen="1" Type="ulong" Bitmask="ffffffff" ByteOrder="big-endian" Size="4"/>
			<SubGroup Name="sSigHeader" Offset="4" ArrayLen="1" Size="12">
				<Signal Name="uiTimeStamp" Offset="0" ArrayLen="1" Type="ulong" Bitmask="ffffffff" ByteOrder="big-endian" Size="4"/>
				<Signal Name="uiMeasurementCounter" Offset="4" ArrayLen="1" Type="ushort" Bitmask="ffff" ByteOrder="big-endian" Size="2"/>
				<Signal Name="uiCycleCounter" Offset="6" ArrayLen="1" Type="ushort" Bitmask="ffff" ByteOrder="big-endian" Size="2"/>
				<Signal Name="eSigStatus" Offset="8" ArrayLen="1" Type="uchar" Bitmask="ff" ByteOrder="big-endian" Size="1"/>
				<Signal Name="a_reserve" Offset="9" ArrayLen="3" Type="uchar" Bitmask="ff" ByteOrder="big-endian" Size="1"/>
			</SubGroup>
			<SubGroup Name="Longitudinal" Offset="10" ArrayLen="1" Size="36">
				<Signal Name="Velocity" Offset="0" ArrayLen="1" Type="float" Bitmask="ffffffff" ByteOrder="big-endian" Size="4"/>
				<Signal Name="VelocityTimestamp" Offset="4" ArrayLen="1" Type="ulong" Bitmask="ffffffff" ByteOrder="big-endian" Size="4"/>
				<Signal Name="VelocityRaw" Offset="8" ArrayLen="1" Type="float" Bitmask="ffffffff" ByteOrder="big-endian" Size="4"/>
				<Signal Name="Accel" Offset="C" ArrayLen="1" Type="float" Bitmask="ffffffff" ByteOrder="big-endian" Size="4"/>
				<Signal Name="AccelTimestamp" Offset="10" ArrayLen="1" Type="ulong" Bitmask="ffffffff" ByteOrder="big-endian" Size="4"/>
				<Signal Name="varVelocity" Offset="14" ArrayLen="1" Type="float" Bitmask="ffffffff" ByteOrder="big-endian" Size="4"/>
				<Signal Name="varAccel" Offset="18" ArrayLen="1" Type="float" Bitmask="ffffffff" ByteOrder="big-endian" Size="4"/>
				<Signal Name="VelCorrFact" Offset="1C" ArrayLen="1" Type="float" Bitmask="ffffffff" ByteOrder="big-endian" Size="4"/>
				<Signal Name="VelocityCorrectionQuality" Offset="20" ArrayLen="1" Type="uchar" Bitmask="ff" ByteOrder="big-endian" Size="1"/>
			</SubGroup>
			<SubGroup Name="Lateral" Offset="34" ArrayLen="1" Size="92">
				<SubGroup Name="YawRate" Offset="0" ArrayLen="1" Size="20">
					<Signal Name="YawRate" Offset="0" ArrayLen="1" Type="float" Bitmask="ffffffff" ByteOrder="big-endian" Size="4"/>
					<Signal Name="YawRateTimestamp" Offset="4" ArrayLen="1" Type="ulong" Bitmask="ffffffff" ByteOrder="big-endian" Size="4"/>
					<Signal Name="YawRateRaw" Offset="8" ArrayLen="1" Type="float" Bitmask="ffffffff" ByteOrder="big-endian" Size="4"/>
					<Signal Name="YawAngle" Offset="C" ArrayLen="1" Type="float" Bitmask="ffffffff" ByteOrder="big-endian" Size="4"/>
					<Signal Name="Variance" Offset="10" ArrayLen="1" Type="float" Bitmask="ffffffff" ByteOrder="big-endian" Size="4"/>
				</SubGroup>
				<SubGroup Name="Curve" Offset="14" ArrayLen="1" Size="20">
					<Signal Name="Curve" Offset="0" ArrayLen="1" Type="float" Bitmask="ffffffff" ByteOrder="big-endian" Size="4"/>
					<Signal Name="CurveTimestamp" Offset="4" ArrayLen="1" Type="ulong" Bitmask="ffffffff" ByteOrder="big-endian" Size="4"/>
					<Signal Name="VarCurve" Offset="8" ArrayLen="1" Type="float" Bitmask="ffffffff" ByteOrder="big-endian" Size="4"/>
					<Signal Name="CrvError" Offset="C" ArrayLen="1" Type="float" Bitmask="ffffffff" ByteOrder="big-endian" Size="4"/>
					<Signal Name="CrvConf" Offset="10" ArrayLen="1" Type="uchar" Bitmask="ff" ByteOrder="big-endian" Size="1"/>
				</SubGroup>
				<SubGroup Name="DrvIntCurve" Offset="28" ArrayLen="1" Size="16">
					<Signal Name="Curve" Offset="0" ArrayLen="1" Type="float" Bitmask="ffffffff" ByteOrder="big-endian" Size="4"/>
					<Signal Name="CurveTimestamp" Offset="4" ArrayLen="1" Type="ulong" Bitmask="ffffffff" ByteOrder="big-endian" Size="4"/>
					<Signal Name="Variance" Offset="8" ArrayLen="1" Type="float" Bitmask="ffffffff" ByteOrder="big-endian" Size="4"/>
					<Signal Name="Gradient" Offset="C" ArrayLen="1" Type="float" Bitmask="ffffffff" ByteOrder="big-endian" Size="4"/>
				</SubGroup>
				<SubGroup Name="Accel" Offset="38" ArrayLen="1" Size="12">
					<Signal Name="LatAccel" Offset="0" ArrayLen="1" Type="float" Bitmask="ffffffff" ByteOrder="big-endian" Size="4"/>
					<Signal Name="LatAccelTimestamp" Offset="4" ArrayLen="1" Type="ulong" Bitmask="ffffffff" ByteOrder="big-endian" Size="4"/>
					<Signal Name="Variance" Offset="8" ArrayLen="1" Type="float" Bitmask="ffffffff" ByteOrder="big-endian" Size="4"/>
				</SubGroup>
				<SubGroup Name="SlipAngle" Offset="44" ArrayLen="1" Size="8">
					<Signal Name="SideSlipAngle" Offset="0" ArrayLen="1" Type="float" Bitmask="ffffffff" ByteOrder="big-endian" Size="4"/>
					<Signal Name="Variance" Offset="4" ArrayLen="1" Type="float" Bitmask="ffffffff" ByteOrder="big-endian" Size="4"/>
				</SubGroup>
				<SubGroup Name="SelfSteering" Offset="4C" ArrayLen="1" Size="16">
					<Signal Name="RoadBankAngle" Offset="0" ArrayLen="1" Type="float" Bitmask="ffffffff" ByteOrder="big-endian" Size="4"/>
					<Signal Name="QuRoadBankAngle" Offset="4" ArrayLen="1" Type="float" Bitmask="ffffffff" ByteOrder="big-endian" Size="4"/>
					<Signal Name="SelfSteerGradEst" Offset="8" ArrayLen="1" Type="float" Bitmask="ffffffff" ByteOrder="big-endian" Size="4"/>
					<Signal Name="QuSelfSteerGradEst" Offset="C" ArrayLen="1" Type="float" Bitmask="ffffffff" ByteOrder="big-endian" Size="4"/>
				</SubGroup>
			</SubGroup>
			<SubGroup Name="MotionState" Offset="90" ArrayLen="1" Size="3">
				<Signal Name="MotState" Offset="0" ArrayLen="1" Type="uchar" Bitmask="ff" ByteOrder="big-endian" Size="1"/>
				<Signal Name="Confidence" Offset="1" ArrayLen="1" Type="uchar" Bitmask="ff" ByteOrder="big-endian" Size="1"/>
				<Signal Name="bRollerTestBench" Offset="2" ArrayLen="1" Type="uchar" Bitmask="ff" ByteOrder="big-endian" Size="1"/>
			</SubGroup>
			<Signal Name="State" Offset="93" ArrayLen="12" Type="uchar" Bitmask="ff" ByteOrder="big-endian" Size="1"/>
		</Group>
	</View>
  <View Name="array_root1" CycleID="2">
		<Group Name="simple_float_arrays" Address="90100000" ArrayLen="1" Size="80">
			<Signal Name="ten_floats" Offset="0" ArrayLen="10" Type="float" Bitmask="ffffffff" ByteOrder="little-endian" Size="4"/>
			<Signal Name="ten_more_floats" Offset="28" ArrayLen="10" Type="float" Bitmask="ffffffff" ByteOrder="little-endian" Size="4"/>
		</Group>
		<Group Name="array_of_structs" Address="901103E8" ArrayLen="1" Size="88">
			<SubGroup Name="first_group" Offset="0" ArrayLen="5" Size="12">
				<Signal Name="big_int" Offset="0" ArrayLen="1" Type="uint64" Bitmask="ffffffffffffffff" ByteOrder="little-endian" Size="8"/>
				<Signal Name="unsigned_short" Offset="8" ArrayLen="1" Type="ushort" Bitmask="ffff" ByteOrder="little-endian" Size="2"/>
				<Signal Name="another_unsigned_short" Offset="A" ArrayLen="1" Type="ushort" Bitmask="ffff" ByteOrder="little-endian" Size="2"/>
			</SubGroup>
			<SubGroup Name="second_group" Offset="3C" ArrayLen="7" Size="4">
				<Signal Name="unsigned_char1" Offset="0" ArrayLen="1" Type="uchar" Bitmask="ff" ByteOrder="big-endian" Size="1"/>
				<Signal Name="unsigned_char2" Offset="1" ArrayLen="1" Type="uchar" Bitmask="ff" ByteOrder="big-endian" Size="1"/>
				<Signal Name="unsigned_char3" Offset="2" ArrayLen="1" Type="uchar" Bitmask="ff" ByteOrder="big-endian" Size="1"/>
				<Signal Name="unsigned_char4" Offset="3" ArrayLen="1" Type="uchar" Bitmask="ff" ByteOrder="big-endian" Size="1"/>
			</SubGroup>
		</Group>
		<Group Name="array_of_array" Address="90110440" ArrayLen="1" Size="336">
			<SubGroup Name="array_group" Offset="0" ArrayLen="10" Size="21">
				<Signal Name="unsigned_short_array" Offset="0" ArrayLen="10" Type="ushort" Bitmask="ffff" ByteOrder="little-endian" Size="2"/>
				<Signal Name="unsigned_char" Offset="14" ArrayLen="1" Type="uchar" Bitmask="ff" ByteOrder="little-endian" Size="1"/>
			</SubGroup>
			<SubGroup Name="second_group" Offset="D2" ArrayLen="7" Size="18">
				<Signal Name="unsigned_char1" Offset="0" ArrayLen="6" Type="uchar" Bitmask="ff" ByteOrder="big-endian" Size="1"/>
				<Signal Name="unsigned_char2" Offset="6" ArrayLen="5" Type="uchar" Bitmask="ff" ByteOrder="big-endian" Size="1"/>
				<Signal Name="unsigned_char3" Offset="B" ArrayLen="4" Type="uchar" Bitmask="ff" ByteOrder="big-endian" Size="1"/>
				<Signal Name="unsigned_char4" Offset="F" ArrayLen="3" Type="uchar" Bitmask="ff" ByteOrder="big-endian" Size="1"/>
			</SubGroup>
		</Group>
	</View>
</SdlFile> )";

std::string_view SDL_CONTENT_SINGLE_GROUP = R"(<?xml version="1.0" encoding="UTF-8"?>
<SdlFile xmlns:xsd="http://www.w3.org/2001/XMLSchema-instance" xsd:noNamespaceSchemaLocation="sdl2-after-compiler.xsd" ByteAlignment="1" Version="2.0">
    <View Name="AlgoVehCycle" CycleID="207">
        <Group Name="VehDyn" Address="20350000" ArrayLen="1" Size="160">
            <Signal Name="uiVersionNumber" Offset="0" ArrayLen="1" Type="ulong" Bitmask="ffffffff" ByteOrder="little-endian" Size="4"/>
            <SubGroup Name="sSigHeader" Offset="4" ArrayLen="1" Size="12">
                <Signal Name="uiTimeStamp" Offset="0" ArrayLen="1" Type="ulong" Bitmask="ffffffff" ByteOrder="little-endian" Size="4"/>
                <Signal Name="uiMeasurementCounter" Offset="4" ArrayLen="1" Type="ushort" Bitmask="ffff" ByteOrder="little-endian" Size="2"/>
                <Signal Name="uiCycleCounter" Offset="6" ArrayLen="1" Type="ushort" Bitmask="ffff" ByteOrder="little-endian" Size="2"/>
                <Signal Name="eSigStatus" Offset="8" ArrayLen="1" Type="uchar" Bitmask="ff" ByteOrder="little-endian" Size="1"/>
                <Signal Name="a_reserve" Offset="9" ArrayLen="3" Type="uchar" Bitmask="ff" ByteOrder="little-endian" Size="1"/>
            </SubGroup>
            <SubGroup Name="Longitudinal" Offset="10" ArrayLen="1" Size="36">
                <Signal Name="Velocity" Offset="0" ArrayLen="1" Type="float" Bitmask="ffffffff" ByteOrder="little-endian" Size="4"/>
                <Signal Name="VelocityTimestamp" Offset="4" ArrayLen="1" Type="ulong" Bitmask="ffffffff" ByteOrder="little-endian" Size="4"/>
                <Signal Name="VelocityRaw" Offset="8" ArrayLen="1" Type="float" Bitmask="ffffffff" ByteOrder="little-endian" Size="4"/>
                <Signal Name="Accel" Offset="c" ArrayLen="1" Type="float" Bitmask="ffffffff" ByteOrder="little-endian" Size="4"/>
                <Signal Name="AccelTimestamp" Offset="10" ArrayLen="1" Type="ulong" Bitmask="ffffffff" ByteOrder="little-endian" Size="4"/>
                <Signal Name="varVelocity" Offset="14" ArrayLen="1" Type="float" Bitmask="ffffffff" ByteOrder="little-endian" Size="4"/>
                <Signal Name="varAccel" Offset="18" ArrayLen="1" Type="float" Bitmask="ffffffff" ByteOrder="little-endian" Size="4"/>
                <Signal Name="VelCorrFact" Offset="1c" ArrayLen="1" Type="float" Bitmask="ffffffff" ByteOrder="little-endian" Size="4"/>
                <Signal Name="VelocityCorrectionQuality" Offset="20" ArrayLen="1" Type="uchar" Bitmask="ff" ByteOrder="little-endian" Size="1"/>
            </SubGroup>
            <SubGroup Name="Lateral" Offset="34" ArrayLen="1" Size="92">
                <SubGroup Name="YawRate" Offset="0" ArrayLen="1" Size="20">
                    <Signal Name="YawRate" Offset="0" ArrayLen="1" Type="float" Bitmask="ffffffff" ByteOrder="little-endian" Size="4"/>
                    <Signal Name="YawRateTimestamp" Offset="4" ArrayLen="1" Type="ulong" Bitmask="ffffffff" ByteOrder="little-endian" Size="4"/>
                    <Signal Name="YawRateRaw" Offset="8" ArrayLen="1" Type="float" Bitmask="ffffffff" ByteOrder="little-endian" Size="4"/>
                    <Signal Name="YawAngle" Offset="c" ArrayLen="1" Type="float" Bitmask="ffffffff" ByteOrder="little-endian" Size="4"/>
                    <Signal Name="Variance" Offset="10" ArrayLen="1" Type="float" Bitmask="ffffffff" ByteOrder="little-endian" Size="4"/>
                </SubGroup>
                <SubGroup Name="Curve" Offset="14" ArrayLen="1" Size="20">
                    <Signal Name="Curve" Offset="0" ArrayLen="1" Type="float" Bitmask="ffffffff" ByteOrder="little-endian" Size="4"/>
                    <Signal Name="CurveTimestamp" Offset="4" ArrayLen="1" Type="ulong" Bitmask="ffffffff" ByteOrder="little-endian" Size="4"/>
                    <Signal Name="VarCurve" Offset="8" ArrayLen="1" Type="float" Bitmask="ffffffff" ByteOrder="little-endian" Size="4"/>
                    <Signal Name="CrvError" Offset="c" ArrayLen="1" Type="float" Bitmask="ffffffff" ByteOrder="little-endian" Size="4"/>
                    <Signal Name="CrvConf" Offset="10" ArrayLen="1" Type="uchar" Bitmask="ff" ByteOrder="little-endian" Size="1"/>
                </SubGroup>
                <SubGroup Name="DrvIntCurve" Offset="28" ArrayLen="1" Size="16">
                    <Signal Name="Curve" Offset="0" ArrayLen="1" Type="float" Bitmask="ffffffff" ByteOrder="little-endian" Size="4"/>
                    <Signal Name="CurveTimestamp" Offset="4" ArrayLen="1" Type="ulong" Bitmask="ffffffff" ByteOrder="little-endian" Size="4"/>
                    <Signal Name="Variance" Offset="8" ArrayLen="1" Type="float" Bitmask="ffffffff" ByteOrder="little-endian" Size="4"/>
                    <Signal Name="Gradient" Offset="c" ArrayLen="1" Type="float" Bitmask="ffffffff" ByteOrder="little-endian" Size="4"/>
                </SubGroup>
                <SubGroup Name="Accel" Offset="38" ArrayLen="1" Size="12">
                    <Signal Name="LatAccel" Offset="0" ArrayLen="1" Type="float" Bitmask="ffffffff" ByteOrder="little-endian" Size="4"/>
                    <Signal Name="LatAccelTimestamp" Offset="4" ArrayLen="1" Type="ulong" Bitmask="ffffffff" ByteOrder="little-endian" Size="4"/>
                    <Signal Name="Variance" Offset="8" ArrayLen="1" Type="float" Bitmask="ffffffff" ByteOrder="little-endian" Size="4"/>
                </SubGroup>
                <SubGroup Name="SlipAngle" Offset="44" ArrayLen="1" Size="8">
                    <Signal Name="SideSlipAngle" Offset="0" ArrayLen="1" Type="float" Bitmask="ffffffff" ByteOrder="little-endian" Size="4"/>
                    <Signal Name="Variance" Offset="4" ArrayLen="1" Type="float" Bitmask="ffffffff" ByteOrder="little-endian" Size="4"/>
                </SubGroup>
                <SubGroup Name="SelfSteering" Offset="4c" ArrayLen="1" Size="16">
                    <Signal Name="RoadBankAngle" Offset="0" ArrayLen="1" Type="float" Bitmask="ffffffff" ByteOrder="little-endian" Size="4"/>
                    <Signal Name="QuRoadBankAngle" Offset="4" ArrayLen="1" Type="float" Bitmask="ffffffff" ByteOrder="little-endian" Size="4"/>
                    <Signal Name="SelfSteerGradEst" Offset="8" ArrayLen="1" Type="float" Bitmask="ffffffff" ByteOrder="little-endian" Size="4"/>
                    <Signal Name="QuSelfSteerGradEst" Offset="c" ArrayLen="1" Type="float" Bitmask="ffffffff" ByteOrder="little-endian" Size="4"/>
                </SubGroup>
            </SubGroup>
            <SubGroup Name="MotionState" Offset="90" ArrayLen="1" Size="3">
                <Signal Name="MotState" Offset="0" ArrayLen="1" Type="uchar" Bitmask="ff" ByteOrder="little-endian" Size="1"/>
                <Signal Name="Confidence" Offset="1" ArrayLen="1" Type="uchar" Bitmask="ff" ByteOrder="little-endian" Size="1"/>
                <Signal Name="bRollerTestBench" Offset="2" ArrayLen="1" Type="uchar" Bitmask="ff" ByteOrder="little-endian" Size="1"/>
            </SubGroup>
            <Signal Name="State" Offset="93" ArrayLen="12" Type="uchar" Bitmask="ff" ByteOrder="little-endian" Size="1"/>
        </Group>
    </View>
</SdlFile>
)";

std::vector<std::string> url_ref_ethernet{
    "GetFullUrlTreeEthernet.FBX4_35up_KW25_13_V5_ETHERNET1_V2_lokal_withoutETS.FRR.BrakeAssistFrontRadar_0xd019_0x1."
    "BrakeAssistFrontRadar.stateEmergencyBraking",
    "GetFullUrlTreeEthernet.FBX4_35up_KW25_13_V5_ETHERNET1_V2_lokal_withoutETS.KAFAS.BasicLegacy_0x350a_0x1."
    "StatusCameraCalibration.cameraCalibrationOffsetHorizontal",
    "GetFullUrlTreeEthernet.FBX4_35up_KW25_13_V5_ETHERNET1_V2_lokal_withoutETS.SAS.DebugServiceSAS_0x120_0x1."
    "DebugEvent.data[0]",
    "GetFullUrlTreeEthernet.FBX4_35up_KW25_13_V5_ETHERNET1_V2_lokal_withoutETS.ZGW.Legacy_0xb501_0x1.NOTIFIER."
    "NavigationGraph_0x8015.NavigationGraph.gradientNavGraph",
    "GetFullUrlTreeEthernet.FBX4_35up_KW25_13_V5_ETHERNET1_V2_lokal_withoutETS.iCAM2.Controller_0x3001_0x1."
    "DrivingFunction.requestActivationDrivingFunction",
};

std::vector<std::string> url_ref_1{
    "GetFullUrlTree1.AlgoVehCycle.VehDyn.uiVersionNumber",
    "GetFullUrlTree1.AlgoVehCycle.VehDyn.sSigHeader.uiTimeStamp",
    "GetFullUrlTree1.AlgoVehCycle.VehDyn.sSigHeader.uiMeasurementCounter",
    "GetFullUrlTree1.AlgoVehCycle.VehDyn.sSigHeader.uiCycleCounter",
    "GetFullUrlTree1.AlgoVehCycle.VehDyn.sSigHeader.eSigStatus",
    "GetFullUrlTree1.AlgoVehCycle.VehDyn.sSigHeader.a_reserve[0]",
    "GetFullUrlTree1.AlgoVehCycle.VehDyn.sSigHeader.a_reserve[1]",
    "GetFullUrlTree1.AlgoVehCycle.VehDyn.sSigHeader.a_reserve[2]",
    "GetFullUrlTree1.AlgoVehCycle.VehDyn.Longitudinal.Velocity",
    "GetFullUrlTree1.AlgoVehCycle.VehDyn.Longitudinal.VelocityTimestamp",
    "GetFullUrlTree1.AlgoVehCycle.VehDyn.Longitudinal.VelocityRaw",
    "GetFullUrlTree1.AlgoVehCycle.VehDyn.Longitudinal.Accel",
    "GetFullUrlTree1.AlgoVehCycle.VehDyn.Longitudinal.AccelTimestamp",
    "GetFullUrlTree1.AlgoVehCycle.VehDyn.Longitudinal.varVelocity",
    "GetFullUrlTree1.AlgoVehCycle.VehDyn.Longitudinal.varAccel",
    "GetFullUrlTree1.AlgoVehCycle.VehDyn.Longitudinal.VelCorrFact",
    "GetFullUrlTree1.AlgoVehCycle.VehDyn.Longitudinal.VelocityCorrectionQuality",
    "GetFullUrlTree1.AlgoVehCycle.VehDyn.Lateral.YawRate.YawRate",
    "GetFullUrlTree1.AlgoVehCycle.VehDyn.Lateral.YawRate.YawRateTimestamp",
    "GetFullUrlTree1.AlgoVehCycle.VehDyn.Lateral.YawRate.YawRateRaw",
    "GetFullUrlTree1.AlgoVehCycle.VehDyn.Lateral.YawRate.YawAngle",
    "GetFullUrlTree1.AlgoVehCycle.VehDyn.Lateral.YawRate.Variance",
    "GetFullUrlTree1.AlgoVehCycle.VehDyn.Lateral.Curve.Curve",
    "GetFullUrlTree1.AlgoVehCycle.VehDyn.Lateral.Curve.CurveTimestamp",
    "GetFullUrlTree1.AlgoVehCycle.VehDyn.Lateral.Curve.VarCurve",
    "GetFullUrlTree1.AlgoVehCycle.VehDyn.Lateral.Curve.CrvError",
    "GetFullUrlTree1.AlgoVehCycle.VehDyn.Lateral.Curve.CrvConf",
    "GetFullUrlTree1.AlgoVehCycle.VehDyn.Lateral.DrvIntCurve.Curve",
    "GetFullUrlTree1.AlgoVehCycle.VehDyn.Lateral.DrvIntCurve.CurveTimestamp",
    "GetFullUrlTree1.AlgoVehCycle.VehDyn.Lateral.DrvIntCurve.Variance",
    "GetFullUrlTree1.AlgoVehCycle.VehDyn.Lateral.DrvIntCurve.Gradient",
    "GetFullUrlTree1.AlgoVehCycle.VehDyn.Lateral.Accel.LatAccel",
    "GetFullUrlTree1.AlgoVehCycle.VehDyn.Lateral.Accel.LatAccelTimestamp",
    "GetFullUrlTree1.AlgoVehCycle.VehDyn.Lateral.Accel.Variance",
    "GetFullUrlTree1.AlgoVehCycle.VehDyn.Lateral.SlipAngle.SideSlipAngle",
    "GetFullUrlTree1.AlgoVehCycle.VehDyn.Lateral.SlipAngle.Variance",
    "GetFullUrlTree1.AlgoVehCycle.VehDyn.Lateral.SelfSteering.RoadBankAngle",
    "GetFullUrlTree1.AlgoVehCycle.VehDyn.Lateral.SelfSteering.QuRoadBankAngle",
    "GetFullUrlTree1.AlgoVehCycle.VehDyn.Lateral.SelfSteering.SelfSteerGradEst",
    "GetFullUrlTree1.AlgoVehCycle.VehDyn.Lateral.SelfSteering.QuSelfSteerGradEst",
    "GetFullUrlTree1.AlgoVehCycle.VehDyn.MotionState.MotState",
    "GetFullUrlTree1.AlgoVehCycle.VehDyn.MotionState.Confidence",
    "GetFullUrlTree1.AlgoVehCycle.VehDyn.MotionState.bRollerTestBench",
    "GetFullUrlTree1.AlgoVehCycle.VehDyn.State[0]",
    "GetFullUrlTree1.AlgoVehCycle.VehDyn.State[1]",
    "GetFullUrlTree1.AlgoVehCycle.VehDyn.State[2]",
    "GetFullUrlTree1.AlgoVehCycle.VehDyn.State[3]",
    "GetFullUrlTree1.AlgoVehCycle.VehDyn.State[4]",
    "GetFullUrlTree1.AlgoVehCycle.VehDyn.State[5]",
    "GetFullUrlTree1.AlgoVehCycle.VehDyn.State[6]",
    "GetFullUrlTree1.AlgoVehCycle.VehDyn.State[7]",
    "GetFullUrlTree1.AlgoVehCycle.VehDyn.State[8]",
    "GetFullUrlTree1.AlgoVehCycle.VehDyn.State[9]",
    "GetFullUrlTree1.AlgoVehCycle.VehDyn.State[10]",
    "GetFullUrlTree1.AlgoVehCycle.VehDyn.State[11]",
    "GetFullUrlTree1.array_root.array_of_array.array_group[0].unsigned_short_array[0]",
    "GetFullUrlTree1.array_root.array_of_array.array_group[0].unsigned_short_array[1]",
    "GetFullUrlTree1.array_root.array_of_array.array_group[0].unsigned_short_array[2]",
    "GetFullUrlTree1.array_root.array_of_array.array_group[0].unsigned_short_array[3]",
    "GetFullUrlTree1.array_root.array_of_array.array_group[0].unsigned_short_array[4]",
    "GetFullUrlTree1.array_root.array_of_array.array_group[0].unsigned_short_array[5]",
    "GetFullUrlTree1.array_root.array_of_array.array_group[0].unsigned_short_array[6]",
    "GetFullUrlTree1.array_root.array_of_array.array_group[0].unsigned_short_array[7]",
    "GetFullUrlTree1.array_root.array_of_array.array_group[0].unsigned_short_array[8]",
    "GetFullUrlTree1.array_root.array_of_array.array_group[0].unsigned_short_array[9]",
    "GetFullUrlTree1.array_root.array_of_array.array_group[0].unsigned_char",
    "GetFullUrlTree1.array_root.array_of_array.array_group[1].unsigned_short_array[0]",
    "GetFullUrlTree1.array_root.array_of_array.array_group[1].unsigned_short_array[1]",
    "GetFullUrlTree1.array_root.array_of_array.array_group[1].unsigned_short_array[2]",
    "GetFullUrlTree1.array_root.array_of_array.array_group[1].unsigned_short_array[3]",
    "GetFullUrlTree1.array_root.array_of_array.array_group[1].unsigned_short_array[4]",
    "GetFullUrlTree1.array_root.array_of_array.array_group[1].unsigned_short_array[5]",
    "GetFullUrlTree1.array_root.array_of_array.array_group[1].unsigned_short_array[6]",
    "GetFullUrlTree1.array_root.array_of_array.array_group[1].unsigned_short_array[7]",
    "GetFullUrlTree1.array_root.array_of_array.array_group[1].unsigned_short_array[8]",
    "GetFullUrlTree1.array_root.array_of_array.array_group[1].unsigned_short_array[9]",
    "GetFullUrlTree1.array_root.array_of_array.array_group[1].unsigned_char",
    "GetFullUrlTree1.array_root.array_of_array.array_group[2].unsigned_short_array[0]",
    "GetFullUrlTree1.array_root.array_of_array.array_group[2].unsigned_short_array[1]",
    "GetFullUrlTree1.array_root.array_of_array.array_group[2].unsigned_short_array[2]",
    "GetFullUrlTree1.array_root.array_of_array.array_group[2].unsigned_short_array[3]",
    "GetFullUrlTree1.array_root.array_of_array.array_group[2].unsigned_short_array[4]",
    "GetFullUrlTree1.array_root.array_of_array.array_group[2].unsigned_short_array[5]",
    "GetFullUrlTree1.array_root.array_of_array.array_group[2].unsigned_short_array[6]",
    "GetFullUrlTree1.array_root.array_of_array.array_group[2].unsigned_short_array[7]",
    "GetFullUrlTree1.array_root.array_of_array.array_group[2].unsigned_short_array[8]",
    "GetFullUrlTree1.array_root.array_of_array.array_group[2].unsigned_short_array[9]",
    "GetFullUrlTree1.array_root.array_of_array.array_group[2].unsigned_char",
    "GetFullUrlTree1.array_root.array_of_array.array_group[3].unsigned_short_array[0]",
    "GetFullUrlTree1.array_root.array_of_array.array_group[3].unsigned_short_array[1]",
    "GetFullUrlTree1.array_root.array_of_array.array_group[3].unsigned_short_array[2]",
    "GetFullUrlTree1.array_root.array_of_array.array_group[3].unsigned_short_array[3]",
    "GetFullUrlTree1.array_root.array_of_array.array_group[3].unsigned_short_array[4]",
    "GetFullUrlTree1.array_root.array_of_array.array_group[3].unsigned_short_array[5]",
    "GetFullUrlTree1.array_root.array_of_array.array_group[3].unsigned_short_array[6]",
    "GetFullUrlTree1.array_root.array_of_array.array_group[3].unsigned_short_array[7]",
    "GetFullUrlTree1.array_root.array_of_array.array_group[3].unsigned_short_array[8]",
    "GetFullUrlTree1.array_root.array_of_array.array_group[3].unsigned_short_array[9]",
    "GetFullUrlTree1.array_root.array_of_array.array_group[3].unsigned_char",
    "GetFullUrlTree1.array_root.array_of_array.array_group[4].unsigned_short_array[0]",
    "GetFullUrlTree1.array_root.array_of_array.array_group[4].unsigned_short_array[1]",
    "GetFullUrlTree1.array_root.array_of_array.array_group[4].unsigned_short_array[2]",
    "GetFullUrlTree1.array_root.array_of_array.array_group[4].unsigned_short_array[3]",
    "GetFullUrlTree1.array_root.array_of_array.array_group[4].unsigned_short_array[4]",
    "GetFullUrlTree1.array_root.array_of_array.array_group[4].unsigned_short_array[5]",
    "GetFullUrlTree1.array_root.array_of_array.array_group[4].unsigned_short_array[6]",
    "GetFullUrlTree1.array_root.array_of_array.array_group[4].unsigned_short_array[7]",
    "GetFullUrlTree1.array_root.array_of_array.array_group[4].unsigned_short_array[8]",
    "GetFullUrlTree1.array_root.array_of_array.array_group[4].unsigned_short_array[9]",
    "GetFullUrlTree1.array_root.array_of_array.array_group[4].unsigned_char",
    "GetFullUrlTree1.array_root.array_of_array.array_group[5].unsigned_short_array[0]",
    "GetFullUrlTree1.array_root.array_of_array.array_group[5].unsigned_short_array[1]",
    "GetFullUrlTree1.array_root.array_of_array.array_group[5].unsigned_short_array[2]",
    "GetFullUrlTree1.array_root.array_of_array.array_group[5].unsigned_short_array[3]",
    "GetFullUrlTree1.array_root.array_of_array.array_group[5].unsigned_short_array[4]",
    "GetFullUrlTree1.array_root.array_of_array.array_group[5].unsigned_short_array[5]",
    "GetFullUrlTree1.array_root.array_of_array.array_group[5].unsigned_short_array[6]",
    "GetFullUrlTree1.array_root.array_of_array.array_group[5].unsigned_short_array[7]",
    "GetFullUrlTree1.array_root.array_of_array.array_group[5].unsigned_short_array[8]",
    "GetFullUrlTree1.array_root.array_of_array.array_group[5].unsigned_short_array[9]",
    "GetFullUrlTree1.array_root.array_of_array.array_group[5].unsigned_char",
    "GetFullUrlTree1.array_root.array_of_array.array_group[6].unsigned_short_array[0]",
    "GetFullUrlTree1.array_root.array_of_array.array_group[6].unsigned_short_array[1]",
    "GetFullUrlTree1.array_root.array_of_array.array_group[6].unsigned_short_array[2]",
    "GetFullUrlTree1.array_root.array_of_array.array_group[6].unsigned_short_array[3]",
    "GetFullUrlTree1.array_root.array_of_array.array_group[6].unsigned_short_array[4]",
    "GetFullUrlTree1.array_root.array_of_array.array_group[6].unsigned_short_array[5]",
    "GetFullUrlTree1.array_root.array_of_array.array_group[6].unsigned_short_array[6]",
    "GetFullUrlTree1.array_root.array_of_array.array_group[6].unsigned_short_array[7]",
    "GetFullUrlTree1.array_root.array_of_array.array_group[6].unsigned_short_array[8]",
    "GetFullUrlTree1.array_root.array_of_array.array_group[6].unsigned_short_array[9]",
    "GetFullUrlTree1.array_root.array_of_array.array_group[6].unsigned_char",
    "GetFullUrlTree1.array_root.array_of_array.array_group[7].unsigned_short_array[0]",
    "GetFullUrlTree1.array_root.array_of_array.array_group[7].unsigned_short_array[1]",
    "GetFullUrlTree1.array_root.array_of_array.array_group[7].unsigned_short_array[2]",
    "GetFullUrlTree1.array_root.array_of_array.array_group[7].unsigned_short_array[3]",
    "GetFullUrlTree1.array_root.array_of_array.array_group[7].unsigned_short_array[4]",
    "GetFullUrlTree1.array_root.array_of_array.array_group[7].unsigned_short_array[5]",
    "GetFullUrlTree1.array_root.array_of_array.array_group[7].unsigned_short_array[6]",
    "GetFullUrlTree1.array_root.array_of_array.array_group[7].unsigned_short_array[7]",
    "GetFullUrlTree1.array_root.array_of_array.array_group[7].unsigned_short_array[8]",
    "GetFullUrlTree1.array_root.array_of_array.array_group[7].unsigned_short_array[9]",
    "GetFullUrlTree1.array_root.array_of_array.array_group[7].unsigned_char",
    "GetFullUrlTree1.array_root.array_of_array.array_group[8].unsigned_short_array[0]",
    "GetFullUrlTree1.array_root.array_of_array.array_group[8].unsigned_short_array[1]",
    "GetFullUrlTree1.array_root.array_of_array.array_group[8].unsigned_short_array[2]",
    "GetFullUrlTree1.array_root.array_of_array.array_group[8].unsigned_short_array[3]",
    "GetFullUrlTree1.array_root.array_of_array.array_group[8].unsigned_short_array[4]",
    "GetFullUrlTree1.array_root.array_of_array.array_group[8].unsigned_short_array[5]",
    "GetFullUrlTree1.array_root.array_of_array.array_group[8].unsigned_short_array[6]",
    "GetFullUrlTree1.array_root.array_of_array.array_group[8].unsigned_short_array[7]",
    "GetFullUrlTree1.array_root.array_of_array.array_group[8].unsigned_short_array[8]",
    "GetFullUrlTree1.array_root.array_of_array.array_group[8].unsigned_short_array[9]",
    "GetFullUrlTree1.array_root.array_of_array.array_group[8].unsigned_char",
    "GetFullUrlTree1.array_root.array_of_array.array_group[9].unsigned_short_array[0]",
    "GetFullUrlTree1.array_root.array_of_array.array_group[9].unsigned_short_array[1]",
    "GetFullUrlTree1.array_root.array_of_array.array_group[9].unsigned_short_array[2]",
    "GetFullUrlTree1.array_root.array_of_array.array_group[9].unsigned_short_array[3]",
    "GetFullUrlTree1.array_root.array_of_array.array_group[9].unsigned_short_array[4]",
    "GetFullUrlTree1.array_root.array_of_array.array_group[9].unsigned_short_array[5]",
    "GetFullUrlTree1.array_root.array_of_array.array_group[9].unsigned_short_array[6]",
    "GetFullUrlTree1.array_root.array_of_array.array_group[9].unsigned_short_array[7]",
    "GetFullUrlTree1.array_root.array_of_array.array_group[9].unsigned_short_array[8]",
    "GetFullUrlTree1.array_root.array_of_array.array_group[9].unsigned_short_array[9]",
    "GetFullUrlTree1.array_root.array_of_array.array_group[9].unsigned_char",
    "GetFullUrlTree1.array_root.array_of_array.second_group[0].unsigned_char1[0]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[0].unsigned_char1[1]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[0].unsigned_char1[2]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[0].unsigned_char1[3]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[0].unsigned_char1[4]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[0].unsigned_char1[5]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[0].unsigned_char2[0]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[0].unsigned_char2[1]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[0].unsigned_char2[2]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[0].unsigned_char2[3]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[0].unsigned_char2[4]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[0].unsigned_char3[0]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[0].unsigned_char3[1]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[0].unsigned_char3[2]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[0].unsigned_char3[3]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[0].unsigned_char4[0]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[0].unsigned_char4[1]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[0].unsigned_char4[2]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[1].unsigned_char1[0]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[1].unsigned_char1[1]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[1].unsigned_char1[2]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[1].unsigned_char1[3]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[1].unsigned_char1[4]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[1].unsigned_char1[5]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[1].unsigned_char2[0]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[1].unsigned_char2[1]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[1].unsigned_char2[2]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[1].unsigned_char2[3]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[1].unsigned_char2[4]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[1].unsigned_char3[0]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[1].unsigned_char3[1]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[1].unsigned_char3[2]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[1].unsigned_char3[3]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[1].unsigned_char4[0]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[1].unsigned_char4[1]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[1].unsigned_char4[2]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[2].unsigned_char1[0]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[2].unsigned_char1[1]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[2].unsigned_char1[2]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[2].unsigned_char1[3]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[2].unsigned_char1[4]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[2].unsigned_char1[5]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[2].unsigned_char2[0]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[2].unsigned_char2[1]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[2].unsigned_char2[2]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[2].unsigned_char2[3]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[2].unsigned_char2[4]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[2].unsigned_char3[0]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[2].unsigned_char3[1]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[2].unsigned_char3[2]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[2].unsigned_char3[3]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[2].unsigned_char4[0]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[2].unsigned_char4[1]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[2].unsigned_char4[2]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[3].unsigned_char1[0]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[3].unsigned_char1[1]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[3].unsigned_char1[2]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[3].unsigned_char1[3]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[3].unsigned_char1[4]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[3].unsigned_char1[5]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[3].unsigned_char2[0]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[3].unsigned_char2[1]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[3].unsigned_char2[2]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[3].unsigned_char2[3]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[3].unsigned_char2[4]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[3].unsigned_char3[0]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[3].unsigned_char3[1]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[3].unsigned_char3[2]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[3].unsigned_char3[3]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[3].unsigned_char4[0]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[3].unsigned_char4[1]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[3].unsigned_char4[2]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[4].unsigned_char1[0]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[4].unsigned_char1[1]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[4].unsigned_char1[2]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[4].unsigned_char1[3]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[4].unsigned_char1[4]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[4].unsigned_char1[5]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[4].unsigned_char2[0]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[4].unsigned_char2[1]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[4].unsigned_char2[2]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[4].unsigned_char2[3]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[4].unsigned_char2[4]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[4].unsigned_char3[0]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[4].unsigned_char3[1]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[4].unsigned_char3[2]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[4].unsigned_char3[3]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[4].unsigned_char4[0]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[4].unsigned_char4[1]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[4].unsigned_char4[2]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[5].unsigned_char1[0]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[5].unsigned_char1[1]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[5].unsigned_char1[2]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[5].unsigned_char1[3]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[5].unsigned_char1[4]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[5].unsigned_char1[5]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[5].unsigned_char2[0]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[5].unsigned_char2[1]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[5].unsigned_char2[2]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[5].unsigned_char2[3]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[5].unsigned_char2[4]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[5].unsigned_char3[0]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[5].unsigned_char3[1]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[5].unsigned_char3[2]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[5].unsigned_char3[3]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[5].unsigned_char4[0]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[5].unsigned_char4[1]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[5].unsigned_char4[2]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[6].unsigned_char1[0]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[6].unsigned_char1[1]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[6].unsigned_char1[2]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[6].unsigned_char1[3]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[6].unsigned_char1[4]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[6].unsigned_char1[5]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[6].unsigned_char2[0]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[6].unsigned_char2[1]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[6].unsigned_char2[2]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[6].unsigned_char2[3]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[6].unsigned_char2[4]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[6].unsigned_char3[0]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[6].unsigned_char3[1]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[6].unsigned_char3[2]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[6].unsigned_char3[3]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[6].unsigned_char4[0]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[6].unsigned_char4[1]",
    "GetFullUrlTree1.array_root.array_of_array.second_group[6].unsigned_char4[2]",
    "GetFullUrlTree1.array_root.array_of_structs.first_group[0].big_int",
    "GetFullUrlTree1.array_root.array_of_structs.first_group[0].unsigned_short",
    "GetFullUrlTree1.array_root.array_of_structs.first_group[0].another_unsigned_short",
    "GetFullUrlTree1.array_root.array_of_structs.first_group[1].big_int",
    "GetFullUrlTree1.array_root.array_of_structs.first_group[1].unsigned_short",
    "GetFullUrlTree1.array_root.array_of_structs.first_group[1].another_unsigned_short",
    "GetFullUrlTree1.array_root.array_of_structs.first_group[2].big_int",
    "GetFullUrlTree1.array_root.array_of_structs.first_group[2].unsigned_short",
    "GetFullUrlTree1.array_root.array_of_structs.first_group[2].another_unsigned_short",
    "GetFullUrlTree1.array_root.array_of_structs.first_group[3].big_int",
    "GetFullUrlTree1.array_root.array_of_structs.first_group[3].unsigned_short",
    "GetFullUrlTree1.array_root.array_of_structs.first_group[3].another_unsigned_short",
    "GetFullUrlTree1.array_root.array_of_structs.first_group[4].big_int",
    "GetFullUrlTree1.array_root.array_of_structs.first_group[4].unsigned_short",
    "GetFullUrlTree1.array_root.array_of_structs.first_group[4].another_unsigned_short",
    "GetFullUrlTree1.array_root.array_of_structs.second_group[0].unsigned_char1",
    "GetFullUrlTree1.array_root.array_of_structs.second_group[0].unsigned_char2",
    "GetFullUrlTree1.array_root.array_of_structs.second_group[0].unsigned_char3",
    "GetFullUrlTree1.array_root.array_of_structs.second_group[0].unsigned_char4",
    "GetFullUrlTree1.array_root.array_of_structs.second_group[1].unsigned_char1",
    "GetFullUrlTree1.array_root.array_of_structs.second_group[1].unsigned_char2",
    "GetFullUrlTree1.array_root.array_of_structs.second_group[1].unsigned_char3",
    "GetFullUrlTree1.array_root.array_of_structs.second_group[1].unsigned_char4",
    "GetFullUrlTree1.array_root.array_of_structs.second_group[2].unsigned_char1",
    "GetFullUrlTree1.array_root.array_of_structs.second_group[2].unsigned_char2",
    "GetFullUrlTree1.array_root.array_of_structs.second_group[2].unsigned_char3",
    "GetFullUrlTree1.array_root.array_of_structs.second_group[2].unsigned_char4",
    "GetFullUrlTree1.array_root.array_of_structs.second_group[3].unsigned_char1",
    "GetFullUrlTree1.array_root.array_of_structs.second_group[3].unsigned_char2",
    "GetFullUrlTree1.array_root.array_of_structs.second_group[3].unsigned_char3",
    "GetFullUrlTree1.array_root.array_of_structs.second_group[3].unsigned_char4",
    "GetFullUrlTree1.array_root.array_of_structs.second_group[4].unsigned_char1",
    "GetFullUrlTree1.array_root.array_of_structs.second_group[4].unsigned_char2",
    "GetFullUrlTree1.array_root.array_of_structs.second_group[4].unsigned_char3",
    "GetFullUrlTree1.array_root.array_of_structs.second_group[4].unsigned_char4",
    "GetFullUrlTree1.array_root.array_of_structs.second_group[5].unsigned_char1",
    "GetFullUrlTree1.array_root.array_of_structs.second_group[5].unsigned_char2",
    "GetFullUrlTree1.array_root.array_of_structs.second_group[5].unsigned_char3",
    "GetFullUrlTree1.array_root.array_of_structs.second_group[5].unsigned_char4",
    "GetFullUrlTree1.array_root.array_of_structs.second_group[6].unsigned_char1",
    "GetFullUrlTree1.array_root.array_of_structs.second_group[6].unsigned_char2",
    "GetFullUrlTree1.array_root.array_of_structs.second_group[6].unsigned_char3",
    "GetFullUrlTree1.array_root.array_of_structs.second_group[6].unsigned_char4",
    "GetFullUrlTree1.array_root.simple_float_arrays.ten_floats[0]",
    "GetFullUrlTree1.array_root.simple_float_arrays.ten_floats[1]",
    "GetFullUrlTree1.array_root.simple_float_arrays.ten_floats[2]",
    "GetFullUrlTree1.array_root.simple_float_arrays.ten_floats[3]",
    "GetFullUrlTree1.array_root.simple_float_arrays.ten_floats[4]",
    "GetFullUrlTree1.array_root.simple_float_arrays.ten_floats[5]",
    "GetFullUrlTree1.array_root.simple_float_arrays.ten_floats[6]",
    "GetFullUrlTree1.array_root.simple_float_arrays.ten_floats[7]",
    "GetFullUrlTree1.array_root.simple_float_arrays.ten_floats[8]",
    "GetFullUrlTree1.array_root.simple_float_arrays.ten_floats[9]",
    "GetFullUrlTree1.array_root.simple_float_arrays.ten_more_floats[0]",
    "GetFullUrlTree1.array_root.simple_float_arrays.ten_more_floats[1]",
    "GetFullUrlTree1.array_root.simple_float_arrays.ten_more_floats[2]",
    "GetFullUrlTree1.array_root.simple_float_arrays.ten_more_floats[3]",
    "GetFullUrlTree1.array_root.simple_float_arrays.ten_more_floats[4]",
    "GetFullUrlTree1.array_root.simple_float_arrays.ten_more_floats[5]",
    "GetFullUrlTree1.array_root.simple_float_arrays.ten_more_floats[6]",
    "GetFullUrlTree1.array_root.simple_float_arrays.ten_more_floats[7]",
    "GetFullUrlTree1.array_root.simple_float_arrays.ten_more_floats[8]",
    "GetFullUrlTree1.array_root.simple_float_arrays.ten_more_floats[9]",

};
std::vector<std::string> url_ref_2{
    "GetFullUrlTree2.AlgoVehCycle.VehDyn.uiVersionNumber",
    "GetFullUrlTree2.AlgoVehCycle.VehDyn.sSigHeader.uiTimeStamp",
    "GetFullUrlTree2.AlgoVehCycle.VehDyn.sSigHeader.uiMeasurementCounter",
    "GetFullUrlTree2.AlgoVehCycle.VehDyn.sSigHeader.uiCycleCounter",
    "GetFullUrlTree2.AlgoVehCycle.VehDyn.sSigHeader.eSigStatus",
    "GetFullUrlTree2.AlgoVehCycle.VehDyn.sSigHeader.a_reserve[0]",
    "GetFullUrlTree2.AlgoVehCycle.VehDyn.sSigHeader.a_reserve[1]",
    "GetFullUrlTree2.AlgoVehCycle.VehDyn.sSigHeader.a_reserve[2]",
    "GetFullUrlTree2.AlgoVehCycle.VehDyn.Longitudinal.Velocity",
    "GetFullUrlTree2.AlgoVehCycle.VehDyn.Longitudinal.VelocityTimestamp",
    "GetFullUrlTree2.AlgoVehCycle.VehDyn.Longitudinal.VelocityRaw",
    "GetFullUrlTree2.AlgoVehCycle.VehDyn.Longitudinal.Accel",
    "GetFullUrlTree2.AlgoVehCycle.VehDyn.Longitudinal.AccelTimestamp",
    "GetFullUrlTree2.AlgoVehCycle.VehDyn.Longitudinal.varVelocity",
    "GetFullUrlTree2.AlgoVehCycle.VehDyn.Longitudinal.varAccel",
    "GetFullUrlTree2.AlgoVehCycle.VehDyn.Longitudinal.VelCorrFact",
    "GetFullUrlTree2.AlgoVehCycle.VehDyn.Longitudinal.VelocityCorrectionQuality",
    "GetFullUrlTree2.AlgoVehCycle.VehDyn.Lateral.YawRate.YawRate",
    "GetFullUrlTree2.AlgoVehCycle.VehDyn.Lateral.YawRate.YawRateTimestamp",
    "GetFullUrlTree2.AlgoVehCycle.VehDyn.Lateral.YawRate.YawRateRaw",
    "GetFullUrlTree2.AlgoVehCycle.VehDyn.Lateral.YawRate.YawAngle",
    "GetFullUrlTree2.AlgoVehCycle.VehDyn.Lateral.YawRate.Variance",
    "GetFullUrlTree2.AlgoVehCycle.VehDyn.Lateral.Curve.Curve",
    "GetFullUrlTree2.AlgoVehCycle.VehDyn.Lateral.Curve.CurveTimestamp",
    "GetFullUrlTree2.AlgoVehCycle.VehDyn.Lateral.Curve.VarCurve",
    "GetFullUrlTree2.AlgoVehCycle.VehDyn.Lateral.Curve.CrvError",
    "GetFullUrlTree2.AlgoVehCycle.VehDyn.Lateral.Curve.CrvConf",
    "GetFullUrlTree2.AlgoVehCycle.VehDyn.Lateral.DrvIntCurve.Curve",
    "GetFullUrlTree2.AlgoVehCycle.VehDyn.Lateral.DrvIntCurve.CurveTimestamp",
    "GetFullUrlTree2.AlgoVehCycle.VehDyn.Lateral.DrvIntCurve.Variance",
    "GetFullUrlTree2.AlgoVehCycle.VehDyn.Lateral.DrvIntCurve.Gradient",
    "GetFullUrlTree2.AlgoVehCycle.VehDyn.Lateral.Accel.LatAccel",
    "GetFullUrlTree2.AlgoVehCycle.VehDyn.Lateral.Accel.LatAccelTimestamp",
    "GetFullUrlTree2.AlgoVehCycle.VehDyn.Lateral.Accel.Variance",
    "GetFullUrlTree2.AlgoVehCycle.VehDyn.Lateral.SlipAngle.SideSlipAngle",
    "GetFullUrlTree2.AlgoVehCycle.VehDyn.Lateral.SlipAngle.Variance",
    "GetFullUrlTree2.AlgoVehCycle.VehDyn.Lateral.SelfSteering.RoadBankAngle",
    "GetFullUrlTree2.AlgoVehCycle.VehDyn.Lateral.SelfSteering.QuRoadBankAngle",
    "GetFullUrlTree2.AlgoVehCycle.VehDyn.Lateral.SelfSteering.SelfSteerGradEst",
    "GetFullUrlTree2.AlgoVehCycle.VehDyn.Lateral.SelfSteering.QuSelfSteerGradEst",
    "GetFullUrlTree2.AlgoVehCycle.VehDyn.MotionState.MotState",
    "GetFullUrlTree2.AlgoVehCycle.VehDyn.MotionState.Confidence",
    "GetFullUrlTree2.AlgoVehCycle.VehDyn.MotionState.bRollerTestBench",
    "GetFullUrlTree2.AlgoVehCycle.VehDyn.State[0]",
    "GetFullUrlTree2.AlgoVehCycle.VehDyn.State[1]",
    "GetFullUrlTree2.AlgoVehCycle.VehDyn.State[2]",
    "GetFullUrlTree2.AlgoVehCycle.VehDyn.State[3]",
    "GetFullUrlTree2.AlgoVehCycle.VehDyn.State[4]",
    "GetFullUrlTree2.AlgoVehCycle.VehDyn.State[5]",
    "GetFullUrlTree2.AlgoVehCycle.VehDyn.State[6]",
    "GetFullUrlTree2.AlgoVehCycle.VehDyn.State[7]",
    "GetFullUrlTree2.AlgoVehCycle.VehDyn.State[8]",
    "GetFullUrlTree2.AlgoVehCycle.VehDyn.State[9]",
    "GetFullUrlTree2.AlgoVehCycle.VehDyn.State[10]",
    "GetFullUrlTree2.AlgoVehCycle.VehDyn.State[11]",
    "GetFullUrlTree2.array_root.array_of_array.array_group[0].unsigned_short_array[0]",
    "GetFullUrlTree2.array_root.array_of_array.array_group[0].unsigned_short_array[1]",
    "GetFullUrlTree2.array_root.array_of_array.array_group[0].unsigned_short_array[2]",
    "GetFullUrlTree2.array_root.array_of_array.array_group[0].unsigned_short_array[3]",
    "GetFullUrlTree2.array_root.array_of_array.array_group[0].unsigned_short_array[4]",
    "GetFullUrlTree2.array_root.array_of_array.array_group[0].unsigned_short_array[5]",
    "GetFullUrlTree2.array_root.array_of_array.array_group[0].unsigned_short_array[6]",
    "GetFullUrlTree2.array_root.array_of_array.array_group[0].unsigned_short_array[7]",
    "GetFullUrlTree2.array_root.array_of_array.array_group[0].unsigned_short_array[8]",
    "GetFullUrlTree2.array_root.array_of_array.array_group[0].unsigned_short_array[9]",
    "GetFullUrlTree2.array_root.array_of_array.array_group[0].unsigned_char",
    "GetFullUrlTree2.array_root.array_of_array.array_group[1].unsigned_short_array[0]",
    "GetFullUrlTree2.array_root.array_of_array.array_group[1].unsigned_short_array[1]",
    "GetFullUrlTree2.array_root.array_of_array.array_group[1].unsigned_short_array[2]",
    "GetFullUrlTree2.array_root.array_of_array.array_group[1].unsigned_short_array[3]",
    "GetFullUrlTree2.array_root.array_of_array.array_group[1].unsigned_short_array[4]",
    "GetFullUrlTree2.array_root.array_of_array.array_group[1].unsigned_short_array[5]",
    "GetFullUrlTree2.array_root.array_of_array.array_group[1].unsigned_short_array[6]",
    "GetFullUrlTree2.array_root.array_of_array.array_group[1].unsigned_short_array[7]",
    "GetFullUrlTree2.array_root.array_of_array.array_group[1].unsigned_short_array[8]",
    "GetFullUrlTree2.array_root.array_of_array.array_group[1].unsigned_short_array[9]",
    "GetFullUrlTree2.array_root.array_of_array.array_group[1].unsigned_char",
    "GetFullUrlTree2.array_root.array_of_array.array_group[2].unsigned_short_array[0]",
    "GetFullUrlTree2.array_root.array_of_array.array_group[2].unsigned_short_array[1]",
    "GetFullUrlTree2.array_root.array_of_array.array_group[2].unsigned_short_array[2]",
    "GetFullUrlTree2.array_root.array_of_array.array_group[2].unsigned_short_array[3]",
    "GetFullUrlTree2.array_root.array_of_array.array_group[2].unsigned_short_array[4]",
    "GetFullUrlTree2.array_root.array_of_array.array_group[2].unsigned_short_array[5]",
    "GetFullUrlTree2.array_root.array_of_array.array_group[2].unsigned_short_array[6]",
    "GetFullUrlTree2.array_root.array_of_array.array_group[2].unsigned_short_array[7]",
    "GetFullUrlTree2.array_root.array_of_array.array_group[2].unsigned_short_array[8]",
    "GetFullUrlTree2.array_root.array_of_array.array_group[2].unsigned_short_array[9]",
    "GetFullUrlTree2.array_root.array_of_array.array_group[2].unsigned_char",
    "GetFullUrlTree2.array_root.array_of_array.array_group[3].unsigned_short_array[0]",
    "GetFullUrlTree2.array_root.array_of_array.array_group[3].unsigned_short_array[1]",
    "GetFullUrlTree2.array_root.array_of_array.array_group[3].unsigned_short_array[2]",
    "GetFullUrlTree2.array_root.array_of_array.array_group[3].unsigned_short_array[3]",
    "GetFullUrlTree2.array_root.array_of_array.array_group[3].unsigned_short_array[4]",
    "GetFullUrlTree2.array_root.array_of_array.array_group[3].unsigned_short_array[5]",
    "GetFullUrlTree2.array_root.array_of_array.array_group[3].unsigned_short_array[6]",
    "GetFullUrlTree2.array_root.array_of_array.array_group[3].unsigned_short_array[7]",
    "GetFullUrlTree2.array_root.array_of_array.array_group[3].unsigned_short_array[8]",
    "GetFullUrlTree2.array_root.array_of_array.array_group[3].unsigned_short_array[9]",
    "GetFullUrlTree2.array_root.array_of_array.array_group[3].unsigned_char",
    "GetFullUrlTree2.array_root.array_of_array.array_group[4].unsigned_short_array[0]",
    "GetFullUrlTree2.array_root.array_of_array.array_group[4].unsigned_short_array[1]",
    "GetFullUrlTree2.array_root.array_of_array.array_group[4].unsigned_short_array[2]",
    "GetFullUrlTree2.array_root.array_of_array.array_group[4].unsigned_short_array[3]",
    "GetFullUrlTree2.array_root.array_of_array.array_group[4].unsigned_short_array[4]",
    "GetFullUrlTree2.array_root.array_of_array.array_group[4].unsigned_short_array[5]",
    "GetFullUrlTree2.array_root.array_of_array.array_group[4].unsigned_short_array[6]",
    "GetFullUrlTree2.array_root.array_of_array.array_group[4].unsigned_short_array[7]",
    "GetFullUrlTree2.array_root.array_of_array.array_group[4].unsigned_short_array[8]",
    "GetFullUrlTree2.array_root.array_of_array.array_group[4].unsigned_short_array[9]",
    "GetFullUrlTree2.array_root.array_of_array.array_group[4].unsigned_char",
    "GetFullUrlTree2.array_root.array_of_array.array_group[5].unsigned_short_array[0]",
    "GetFullUrlTree2.array_root.array_of_array.array_group[5].unsigned_short_array[1]",
    "GetFullUrlTree2.array_root.array_of_array.array_group[5].unsigned_short_array[2]",
    "GetFullUrlTree2.array_root.array_of_array.array_group[5].unsigned_short_array[3]",
    "GetFullUrlTree2.array_root.array_of_array.array_group[5].unsigned_short_array[4]",
    "GetFullUrlTree2.array_root.array_of_array.array_group[5].unsigned_short_array[5]",
    "GetFullUrlTree2.array_root.array_of_array.array_group[5].unsigned_short_array[6]",
    "GetFullUrlTree2.array_root.array_of_array.array_group[5].unsigned_short_array[7]",
    "GetFullUrlTree2.array_root.array_of_array.array_group[5].unsigned_short_array[8]",
    "GetFullUrlTree2.array_root.array_of_array.array_group[5].unsigned_short_array[9]",
    "GetFullUrlTree2.array_root.array_of_array.array_group[5].unsigned_char",
    "GetFullUrlTree2.array_root.array_of_array.array_group[6].unsigned_short_array[0]",
    "GetFullUrlTree2.array_root.array_of_array.array_group[6].unsigned_short_array[1]",
    "GetFullUrlTree2.array_root.array_of_array.array_group[6].unsigned_short_array[2]",
    "GetFullUrlTree2.array_root.array_of_array.array_group[6].unsigned_short_array[3]",
    "GetFullUrlTree2.array_root.array_of_array.array_group[6].unsigned_short_array[4]",
    "GetFullUrlTree2.array_root.array_of_array.array_group[6].unsigned_short_array[5]",
    "GetFullUrlTree2.array_root.array_of_array.array_group[6].unsigned_short_array[6]",
    "GetFullUrlTree2.array_root.array_of_array.array_group[6].unsigned_short_array[7]",
    "GetFullUrlTree2.array_root.array_of_array.array_group[6].unsigned_short_array[8]",
    "GetFullUrlTree2.array_root.array_of_array.array_group[6].unsigned_short_array[9]",
    "GetFullUrlTree2.array_root.array_of_array.array_group[6].unsigned_char",
    "GetFullUrlTree2.array_root.array_of_array.array_group[7].unsigned_short_array[0]",
    "GetFullUrlTree2.array_root.array_of_array.array_group[7].unsigned_short_array[1]",
    "GetFullUrlTree2.array_root.array_of_array.array_group[7].unsigned_short_array[2]",
    "GetFullUrlTree2.array_root.array_of_array.array_group[7].unsigned_short_array[3]",
    "GetFullUrlTree2.array_root.array_of_array.array_group[7].unsigned_short_array[4]",
    "GetFullUrlTree2.array_root.array_of_array.array_group[7].unsigned_short_array[5]",
    "GetFullUrlTree2.array_root.array_of_array.array_group[7].unsigned_short_array[6]",
    "GetFullUrlTree2.array_root.array_of_array.array_group[7].unsigned_short_array[7]",
    "GetFullUrlTree2.array_root.array_of_array.array_group[7].unsigned_short_array[8]",
    "GetFullUrlTree2.array_root.array_of_array.array_group[7].unsigned_short_array[9]",
    "GetFullUrlTree2.array_root.array_of_array.array_group[7].unsigned_char",
    "GetFullUrlTree2.array_root.array_of_array.array_group[8].unsigned_short_array[0]",
    "GetFullUrlTree2.array_root.array_of_array.array_group[8].unsigned_short_array[1]",
    "GetFullUrlTree2.array_root.array_of_array.array_group[8].unsigned_short_array[2]",
    "GetFullUrlTree2.array_root.array_of_array.array_group[8].unsigned_short_array[3]",
    "GetFullUrlTree2.array_root.array_of_array.array_group[8].unsigned_short_array[4]",
    "GetFullUrlTree2.array_root.array_of_array.array_group[8].unsigned_short_array[5]",
    "GetFullUrlTree2.array_root.array_of_array.array_group[8].unsigned_short_array[6]",
    "GetFullUrlTree2.array_root.array_of_array.array_group[8].unsigned_short_array[7]",
    "GetFullUrlTree2.array_root.array_of_array.array_group[8].unsigned_short_array[8]",
    "GetFullUrlTree2.array_root.array_of_array.array_group[8].unsigned_short_array[9]",
    "GetFullUrlTree2.array_root.array_of_array.array_group[8].unsigned_char",
    "GetFullUrlTree2.array_root.array_of_array.array_group[9].unsigned_short_array[0]",
    "GetFullUrlTree2.array_root.array_of_array.array_group[9].unsigned_short_array[1]",
    "GetFullUrlTree2.array_root.array_of_array.array_group[9].unsigned_short_array[2]",
    "GetFullUrlTree2.array_root.array_of_array.array_group[9].unsigned_short_array[3]",
    "GetFullUrlTree2.array_root.array_of_array.array_group[9].unsigned_short_array[4]",
    "GetFullUrlTree2.array_root.array_of_array.array_group[9].unsigned_short_array[5]",
    "GetFullUrlTree2.array_root.array_of_array.array_group[9].unsigned_short_array[6]",
    "GetFullUrlTree2.array_root.array_of_array.array_group[9].unsigned_short_array[7]",
    "GetFullUrlTree2.array_root.array_of_array.array_group[9].unsigned_short_array[8]",
    "GetFullUrlTree2.array_root.array_of_array.array_group[9].unsigned_short_array[9]",
    "GetFullUrlTree2.array_root.array_of_array.array_group[9].unsigned_char",
    "GetFullUrlTree2.array_root.array_of_array.second_group[0].unsigned_char1[0]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[0].unsigned_char1[1]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[0].unsigned_char1[2]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[0].unsigned_char1[3]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[0].unsigned_char1[4]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[0].unsigned_char1[5]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[0].unsigned_char2[0]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[0].unsigned_char2[1]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[0].unsigned_char2[2]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[0].unsigned_char2[3]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[0].unsigned_char2[4]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[0].unsigned_char3[0]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[0].unsigned_char3[1]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[0].unsigned_char3[2]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[0].unsigned_char3[3]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[0].unsigned_char4[0]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[0].unsigned_char4[1]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[0].unsigned_char4[2]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[1].unsigned_char1[0]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[1].unsigned_char1[1]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[1].unsigned_char1[2]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[1].unsigned_char1[3]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[1].unsigned_char1[4]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[1].unsigned_char1[5]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[1].unsigned_char2[0]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[1].unsigned_char2[1]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[1].unsigned_char2[2]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[1].unsigned_char2[3]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[1].unsigned_char2[4]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[1].unsigned_char3[0]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[1].unsigned_char3[1]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[1].unsigned_char3[2]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[1].unsigned_char3[3]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[1].unsigned_char4[0]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[1].unsigned_char4[1]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[1].unsigned_char4[2]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[2].unsigned_char1[0]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[2].unsigned_char1[1]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[2].unsigned_char1[2]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[2].unsigned_char1[3]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[2].unsigned_char1[4]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[2].unsigned_char1[5]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[2].unsigned_char2[0]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[2].unsigned_char2[1]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[2].unsigned_char2[2]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[2].unsigned_char2[3]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[2].unsigned_char2[4]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[2].unsigned_char3[0]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[2].unsigned_char3[1]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[2].unsigned_char3[2]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[2].unsigned_char3[3]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[2].unsigned_char4[0]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[2].unsigned_char4[1]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[2].unsigned_char4[2]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[3].unsigned_char1[0]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[3].unsigned_char1[1]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[3].unsigned_char1[2]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[3].unsigned_char1[3]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[3].unsigned_char1[4]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[3].unsigned_char1[5]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[3].unsigned_char2[0]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[3].unsigned_char2[1]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[3].unsigned_char2[2]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[3].unsigned_char2[3]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[3].unsigned_char2[4]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[3].unsigned_char3[0]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[3].unsigned_char3[1]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[3].unsigned_char3[2]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[3].unsigned_char3[3]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[3].unsigned_char4[0]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[3].unsigned_char4[1]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[3].unsigned_char4[2]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[4].unsigned_char1[0]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[4].unsigned_char1[1]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[4].unsigned_char1[2]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[4].unsigned_char1[3]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[4].unsigned_char1[4]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[4].unsigned_char1[5]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[4].unsigned_char2[0]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[4].unsigned_char2[1]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[4].unsigned_char2[2]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[4].unsigned_char2[3]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[4].unsigned_char2[4]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[4].unsigned_char3[0]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[4].unsigned_char3[1]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[4].unsigned_char3[2]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[4].unsigned_char3[3]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[4].unsigned_char4[0]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[4].unsigned_char4[1]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[4].unsigned_char4[2]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[5].unsigned_char1[0]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[5].unsigned_char1[1]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[5].unsigned_char1[2]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[5].unsigned_char1[3]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[5].unsigned_char1[4]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[5].unsigned_char1[5]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[5].unsigned_char2[0]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[5].unsigned_char2[1]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[5].unsigned_char2[2]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[5].unsigned_char2[3]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[5].unsigned_char2[4]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[5].unsigned_char3[0]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[5].unsigned_char3[1]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[5].unsigned_char3[2]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[5].unsigned_char3[3]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[5].unsigned_char4[0]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[5].unsigned_char4[1]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[5].unsigned_char4[2]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[6].unsigned_char1[0]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[6].unsigned_char1[1]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[6].unsigned_char1[2]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[6].unsigned_char1[3]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[6].unsigned_char1[4]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[6].unsigned_char1[5]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[6].unsigned_char2[0]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[6].unsigned_char2[1]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[6].unsigned_char2[2]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[6].unsigned_char2[3]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[6].unsigned_char2[4]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[6].unsigned_char3[0]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[6].unsigned_char3[1]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[6].unsigned_char3[2]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[6].unsigned_char3[3]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[6].unsigned_char4[0]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[6].unsigned_char4[1]",
    "GetFullUrlTree2.array_root.array_of_array.second_group[6].unsigned_char4[2]",
    "GetFullUrlTree2.array_root.array_of_structs.first_group[0].big_int",
    "GetFullUrlTree2.array_root.array_of_structs.first_group[0].unsigned_short",
    "GetFullUrlTree2.array_root.array_of_structs.first_group[0].another_unsigned_short",
    "GetFullUrlTree2.array_root.array_of_structs.first_group[1].big_int",
    "GetFullUrlTree2.array_root.array_of_structs.first_group[1].unsigned_short",
    "GetFullUrlTree2.array_root.array_of_structs.first_group[1].another_unsigned_short",
    "GetFullUrlTree2.array_root.array_of_structs.first_group[2].big_int",
    "GetFullUrlTree2.array_root.array_of_structs.first_group[2].unsigned_short",
    "GetFullUrlTree2.array_root.array_of_structs.first_group[2].another_unsigned_short",
    "GetFullUrlTree2.array_root.array_of_structs.first_group[3].big_int",
    "GetFullUrlTree2.array_root.array_of_structs.first_group[3].unsigned_short",
    "GetFullUrlTree2.array_root.array_of_structs.first_group[3].another_unsigned_short",
    "GetFullUrlTree2.array_root.array_of_structs.first_group[4].big_int",
    "GetFullUrlTree2.array_root.array_of_structs.first_group[4].unsigned_short",
    "GetFullUrlTree2.array_root.array_of_structs.first_group[4].another_unsigned_short",
    "GetFullUrlTree2.array_root.array_of_structs.second_group[0].unsigned_char1",
    "GetFullUrlTree2.array_root.array_of_structs.second_group[0].unsigned_char2",
    "GetFullUrlTree2.array_root.array_of_structs.second_group[0].unsigned_char3",
    "GetFullUrlTree2.array_root.array_of_structs.second_group[0].unsigned_char4",
    "GetFullUrlTree2.array_root.array_of_structs.second_group[1].unsigned_char1",
    "GetFullUrlTree2.array_root.array_of_structs.second_group[1].unsigned_char2",
    "GetFullUrlTree2.array_root.array_of_structs.second_group[1].unsigned_char3",
    "GetFullUrlTree2.array_root.array_of_structs.second_group[1].unsigned_char4",
    "GetFullUrlTree2.array_root.array_of_structs.second_group[2].unsigned_char1",
    "GetFullUrlTree2.array_root.array_of_structs.second_group[2].unsigned_char2",
    "GetFullUrlTree2.array_root.array_of_structs.second_group[2].unsigned_char3",
    "GetFullUrlTree2.array_root.array_of_structs.second_group[2].unsigned_char4",
    "GetFullUrlTree2.array_root.array_of_structs.second_group[3].unsigned_char1",
    "GetFullUrlTree2.array_root.array_of_structs.second_group[3].unsigned_char2",
    "GetFullUrlTree2.array_root.array_of_structs.second_group[3].unsigned_char3",
    "GetFullUrlTree2.array_root.array_of_structs.second_group[3].unsigned_char4",
    "GetFullUrlTree2.array_root.array_of_structs.second_group[4].unsigned_char1",
    "GetFullUrlTree2.array_root.array_of_structs.second_group[4].unsigned_char2",
    "GetFullUrlTree2.array_root.array_of_structs.second_group[4].unsigned_char3",
    "GetFullUrlTree2.array_root.array_of_structs.second_group[4].unsigned_char4",
    "GetFullUrlTree2.array_root.array_of_structs.second_group[5].unsigned_char1",
    "GetFullUrlTree2.array_root.array_of_structs.second_group[5].unsigned_char2",
    "GetFullUrlTree2.array_root.array_of_structs.second_group[5].unsigned_char3",
    "GetFullUrlTree2.array_root.array_of_structs.second_group[5].unsigned_char4",
    "GetFullUrlTree2.array_root.array_of_structs.second_group[6].unsigned_char1",
    "GetFullUrlTree2.array_root.array_of_structs.second_group[6].unsigned_char2",
    "GetFullUrlTree2.array_root.array_of_structs.second_group[6].unsigned_char3",
    "GetFullUrlTree2.array_root.array_of_structs.second_group[6].unsigned_char4",
    "GetFullUrlTree2.array_root.simple_float_arrays.ten_floats[0]",
    "GetFullUrlTree2.array_root.simple_float_arrays.ten_floats[1]",
    "GetFullUrlTree2.array_root.simple_float_arrays.ten_floats[2]",
    "GetFullUrlTree2.array_root.simple_float_arrays.ten_floats[3]",
    "GetFullUrlTree2.array_root.simple_float_arrays.ten_floats[4]",
    "GetFullUrlTree2.array_root.simple_float_arrays.ten_floats[5]",
    "GetFullUrlTree2.array_root.simple_float_arrays.ten_floats[6]",
    "GetFullUrlTree2.array_root.simple_float_arrays.ten_floats[7]",
    "GetFullUrlTree2.array_root.simple_float_arrays.ten_floats[8]",
    "GetFullUrlTree2.array_root.simple_float_arrays.ten_floats[9]",
    "GetFullUrlTree2.array_root.simple_float_arrays.ten_more_floats[0]",
    "GetFullUrlTree2.array_root.simple_float_arrays.ten_more_floats[1]",
    "GetFullUrlTree2.array_root.simple_float_arrays.ten_more_floats[2]",
    "GetFullUrlTree2.array_root.simple_float_arrays.ten_more_floats[3]",
    "GetFullUrlTree2.array_root.simple_float_arrays.ten_more_floats[4]",
    "GetFullUrlTree2.array_root.simple_float_arrays.ten_more_floats[5]",
    "GetFullUrlTree2.array_root.simple_float_arrays.ten_more_floats[6]",
    "GetFullUrlTree2.array_root.simple_float_arrays.ten_more_floats[7]",
    "GetFullUrlTree2.array_root.simple_float_arrays.ten_more_floats[8]",
    "GetFullUrlTree2.array_root.simple_float_arrays.ten_more_floats[9]",
};

std::vector<std::string> url_ref_Lat{
    "GetFullUrlTree1.AlgoVehCycle.VehDyn.Lateral.YawRate.YawRate",
    "GetFullUrlTree1.AlgoVehCycle.VehDyn.Lateral.YawRate.YawRateTimestamp",
    "GetFullUrlTree1.AlgoVehCycle.VehDyn.Lateral.YawRate.YawRateRaw",
    "GetFullUrlTree1.AlgoVehCycle.VehDyn.Lateral.YawRate.YawAngle",
    "GetFullUrlTree1.AlgoVehCycle.VehDyn.Lateral.YawRate.Variance",
    "GetFullUrlTree1.AlgoVehCycle.VehDyn.Lateral.Curve.Curve",
    "GetFullUrlTree1.AlgoVehCycle.VehDyn.Lateral.Curve.CurveTimestamp",
    "GetFullUrlTree1.AlgoVehCycle.VehDyn.Lateral.Curve.VarCurve",
    "GetFullUrlTree1.AlgoVehCycle.VehDyn.Lateral.Curve.CrvError",
    "GetFullUrlTree1.AlgoVehCycle.VehDyn.Lateral.Curve.CrvConf",
    "GetFullUrlTree1.AlgoVehCycle.VehDyn.Lateral.DrvIntCurve.Curve",
    "GetFullUrlTree1.AlgoVehCycle.VehDyn.Lateral.DrvIntCurve.CurveTimestamp",
    "GetFullUrlTree1.AlgoVehCycle.VehDyn.Lateral.DrvIntCurve.Variance",
    "GetFullUrlTree1.AlgoVehCycle.VehDyn.Lateral.DrvIntCurve.Gradient",
    "GetFullUrlTree1.AlgoVehCycle.VehDyn.Lateral.Accel.LatAccel",
    "GetFullUrlTree1.AlgoVehCycle.VehDyn.Lateral.Accel.LatAccelTimestamp",
    "GetFullUrlTree1.AlgoVehCycle.VehDyn.Lateral.Accel.Variance",
    "GetFullUrlTree1.AlgoVehCycle.VehDyn.Lateral.SlipAngle.SideSlipAngle",
    "GetFullUrlTree1.AlgoVehCycle.VehDyn.Lateral.SlipAngle.Variance",
    "GetFullUrlTree1.AlgoVehCycle.VehDyn.Lateral.SelfSteering.RoadBankAngle",
    "GetFullUrlTree1.AlgoVehCycle.VehDyn.Lateral.SelfSteering.QuRoadBankAngle",
    "GetFullUrlTree1.AlgoVehCycle.VehDyn.Lateral.SelfSteering.SelfSteerGradEst",
    "GetFullUrlTree1.AlgoVehCycle.VehDyn.Lateral.SelfSteering.QuSelfSteerGradEst",
};

std::vector<std::string> url_ref_el{
    "GetFullUrlTree1.AlgoVehCycle.VehDyn.Longitudinal.Velocity",
    "GetFullUrlTree1.AlgoVehCycle.VehDyn.Longitudinal.VelocityTimestamp",
    "GetFullUrlTree1.AlgoVehCycle.VehDyn.Longitudinal.VelocityRaw",
    "GetFullUrlTree1.AlgoVehCycle.VehDyn.Longitudinal.Accel",
    "GetFullUrlTree1.AlgoVehCycle.VehDyn.Longitudinal.AccelTimestamp",
    "GetFullUrlTree1.AlgoVehCycle.VehDyn.Longitudinal.varVelocity",
    "GetFullUrlTree1.AlgoVehCycle.VehDyn.Longitudinal.varAccel",
    "GetFullUrlTree1.AlgoVehCycle.VehDyn.Longitudinal.VelCorrFact",
    "GetFullUrlTree1.AlgoVehCycle.VehDyn.Longitudinal.VelocityCorrectionQuality",
    "GetFullUrlTree1.AlgoVehCycle.VehDyn.Lateral.Accel.LatAccel",
    "GetFullUrlTree1.AlgoVehCycle.VehDyn.Lateral.Accel.LatAccelTimestamp",
    "GetFullUrlTree1.AlgoVehCycle.VehDyn.Lateral.Accel.Variance",
    "GetFullUrlTree1.AlgoVehCycle.VehDyn.Lateral.SelfSteering.RoadBankAngle",
    "GetFullUrlTree1.AlgoVehCycle.VehDyn.Lateral.SelfSteering.QuRoadBankAngle",
    "GetFullUrlTree1.AlgoVehCycle.VehDyn.Lateral.SelfSteering.SelfSteerGradEst",
    "GetFullUrlTree1.AlgoVehCycle.VehDyn.Lateral.SelfSteering.QuSelfSteerGradEst",
};

std::vector<std::string> sigtree_ref_roller{
    "searchSignalTree_keyword.AlgoVehCycle.VehDyn.MotionState.bRollerTestBench",
};

ChildInfo GenerateChild(std::string name, int count) {
  ChildInfo child;
  child.name = name;
  child.child_count = count;

  return child;
}

std::vector<ChildInfo> set_array_members(std::vector<ChildInfo> input) {
  std::vector<ChildInfo> output;
  for (auto &child : input) {
    for (size_t index = 0; index < child.array_lenght; index++) {
      output.push_back(GenerateChild(child.name + "[" + std::to_string(index) + "]", child.child_count));
    }
  }
  return output;
}

std::vector<ChildInfo> GenerateSigHeader() {
  std::vector<ChildInfo> childs_info;
  childs_info.push_back(GenerateChild("uiTimeStamp", 0));
  childs_info.push_back(GenerateChild("uiMeasurementCounter", 0));
  childs_info.push_back(GenerateChild("uiCycleCounter", 0));
  childs_info.push_back(GenerateChild("eSigStatus", 0));
  childs_info.push_back(GenerateChild("a_reserve", 0));
  return childs_info;
}

std::vector<ChildInfo> GenerateArraysOfFloats() {
  std::vector<ChildInfo> childs_info;
  for (int i = 0; i < 10; i++) {
    childs_info.push_back(GenerateChild("ten_floats[" + std::to_string(i) + "]", 0));
  }

  for (int i = 0; i < 10; i++) {
    childs_info.push_back(GenerateChild("ten_more_floats[" + std::to_string(i) + "]", 0));
  }

  return childs_info;
}

std::vector<ChildInfo> GenerateArraysOfArrays() {
  std::vector<ChildInfo> childs_info;
  for (int i = 0; i < 10; i++) {
    childs_info.push_back(GenerateChild("array_group[" + std::to_string(i) + "]", 2));
  }
  for (int i = 0; i < 7; i++) {
    childs_info.push_back(GenerateChild("second_group[" + std::to_string(i) + "]", 4));
  }

  return childs_info;
}

std::vector<ChildInfo> GenerateArraysOfStructs() {
  std::vector<ChildInfo> childs_info;
  for (int i = 0; i < 5; i++) {
    childs_info.push_back(GenerateChild("first_group[" + std::to_string(i) + "]", 3));
  }
  for (int i = 0; i < 7; i++) {
    childs_info.push_back(GenerateChild("second_group[" + std::to_string(i) + "]", 4));
  }

  return childs_info;
}

std::vector<ChildInfo> GenerateSubgroupOfArrays() {
  std::vector<ChildInfo> childs_info;
  for (int i = 0; i < 10; i++) {
    childs_info.push_back(GenerateChild("unsigned_short_array[" + std::to_string(i) + "]", 0));
  }

  return childs_info;
}

std::vector<ChildInfo> GenerateDevices() {
  std::vector<ChildInfo> childs_info;
  childs_info.push_back(GenerateChild("get_child_by_url_test", 2));
  childs_info.push_back(GenerateChild("get_child_by_url_test1", 2));

  return childs_info;
}

std::vector<ChildInfo> GenerateViews() {
  std::vector<ChildInfo> childs_info;
  childs_info.push_back(GenerateChild("AlgoVehCycle", 1));
  childs_info.push_back(GenerateChild("array_root", 3));

  return childs_info;
}

std::vector<ChildInfo> GenerateAlgoVehCycleTopics() {
  std::vector<ChildInfo> childs_info;
  childs_info.push_back(GenerateChild("VehDyn", 6)); // udex monitor
  // childs_info.push_back(GenerateChild("VehDyn", 17)); // signal_watcher

  return childs_info;
}

std::vector<ChildInfo> GenerateArrayRootTopics() {
  std::vector<ChildInfo> childs_info;

  // signal_watcher
  /* childs_info.push_back(GenerateChild("array_of_array", 17));
   childs_info.push_back(GenerateChild("array_of_structs", 12));
   childs_info.push_back(GenerateChild("simple_float_arrays", 20));*/

  childs_info.push_back(GenerateChild("array_of_array", 2));
  childs_info.push_back(GenerateChild("array_of_structs", 2));
  childs_info.push_back(GenerateChild("simple_float_arrays", 2));

  return childs_info;
}

} // namespace test
} // namespace udex
} // namespace next
