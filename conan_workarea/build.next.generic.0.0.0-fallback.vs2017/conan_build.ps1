
$Env:CONAN_COLOR_DISPLAY="0"
$Env:CONAN_LOGIN_USERNAME_CONTI_CENTER="uib55976"
$Env:CONAN_PASSWORD_CONTI_CENTER="AKCp8oho5J6ZAW6vTda3aavZCUh1y362DSxK41Dhcmw8bMv61uyDgfAcADGmD5vYD2a5NcDV2"
$Env:ISDOCKERCONTAINER="1"
$Env:GIT_USER=""
$Env:GIT_PASSWORD=""
$Env:CIP_USER_CONFIG_DIR="C:\Users\uib55976\cip_config_dir"
$Env:CONAN_USER_HOME="C:\Users\uib55976\cip_config_dir\conan"
$Env:HOST_MACHINE_USERNAME="uib55976"
& "C:\Users\uib55976\cip_config_dir\bricks\4.25.1\windows\Scripts\python.exe" "-u" "C:/Users/uib55976/cip_config_dir/bricks/4.25.1/windows/Lib/site-packages/cip_build_system/conan_build_launcher" "--cip-version" "4.25.1" "--conan-workarea" "D:/TATA/LATEST_NXT_REPO/next/conan_workarea" "--selected-entity" "D:/TATA/LATEST_NXT_REPO/next/conan_workarea/build.next.generic.0.0.0-fallback.vs2017/selected_entity.json" "--env-username" "uib55976" "--env-password" "AKCp8oho5J6ZAW6vTda3aavZCUh1y362DSxK41Dhcmw8bMv61uyDgfAcADGmD5vYD2a5NcDV2"
if (-Not $?) {
    throw "Build failed; check output"
}

Write-Warning -Message "WARNING: This script will be removed in the future. Please adjust your workflow"
