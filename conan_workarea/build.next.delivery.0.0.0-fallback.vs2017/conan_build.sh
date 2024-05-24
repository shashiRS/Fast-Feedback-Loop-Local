
export CONAN_COLOR_DISPLAY="0"
export CONAN_LOGIN_USERNAME_CONTI_CENTER="uib55976"
export CONAN_PASSWORD_CONTI_CENTER="AKCp8oho5J6ZAW6vTda3aavZCUh1y362DSxK41Dhcmw8bMv61uyDgfAcADGmD5vYD2a5NcDV2"
export ISDOCKERCONTAINER="1"
export GIT_USER=""
export GIT_PASSWORD=""
export CIP_USER_CONFIG_DIR="C:\Users\uib55976\cip_config_dir"
export CONAN_USER_HOME="C:\Users\uib55976\cip_config_dir\conan"
export HOST_MACHINE_USERNAME="uib55976"
"C:\Users\uib55976\cip_config_dir\bricks\4.25.1\windows\Scripts\python.exe" "-u" "C:/Users/uib55976/cip_config_dir/bricks/4.25.1/windows/Lib/site-packages/cip_build_system/conan_build_launcher" "--cip-version" "4.25.1" "--conan-workarea" "D:/TATA/LATEST_NXT_REPO/next/conan_workarea" "--selected-entity" "D:/TATA/LATEST_NXT_REPO/next/conan_workarea/build.next.delivery.0.0.0-fallback.vs2017/selected_entity.json" "--env-username" "uib55976" "--env-password" "AKCp8oho5J6ZAW6vTda3aavZCUh1y362DSxK41Dhcmw8bMv61uyDgfAcADGmD5vYD2a5NcDV2"
retcode=$?
if [ ${retcode} -ne 0 ]; then
   echo "Build failed" >&2
   exit ${retcode}
fi

echo "WARNING: This script will be removed in the future. Please adjust your workflow" >&2

