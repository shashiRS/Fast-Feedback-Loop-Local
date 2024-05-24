#pragma once
#include <string>

namespace next {
namespace plugin {
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
} // namespace test
} // namespace plugin
} // namespace next