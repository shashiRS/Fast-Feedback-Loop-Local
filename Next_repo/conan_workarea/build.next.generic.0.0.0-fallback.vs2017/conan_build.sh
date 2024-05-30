
export CONAN_COLOR_DISPLAY="0"
export CONAN_LOGIN_USERNAME_CONTI_CENTER="uig44320"
export CONAN_PASSWORD_CONTI_CENTER="AKCp8pRjGdVWcBpjRT4LbfHaeKB7mDJxuYPAScocLD3AzRMVcbB79MjK8UnMUn8tKanoRtbAR"
export ISDOCKERCONTAINER="1"
export GIT_USER=""
export GIT_PASSWORD=""
export CIP_USER_CONFIG_DIR="C:\Users\uig44320\cip_config_dir"
export CONAN_USER_HOME="C:\Users\uig44320\cip_config_dir\conan"
export HOST_MACHINE_USERNAME="uig44320"
"C:\Users\uig44320\cip_config_dir\bricks\4.25.0\windows\Scripts\python.exe" "-u" "C:/Users/uig44320/cip_config_dir/bricks/4.25.0/windows/Lib/site-packages/cip_build_system/conan_build_launcher" "--cip-version" "4.25.0" "--conan-workarea" "D:/Next-python_API/next/conan_workarea" "--selected-entity" "D:/Next-python_API/next/conan_workarea/build.next.generic.0.0.0-fallback.vs2017/selected_entity.json" "--env-username" "uig44320" "--env-password" "AKCp8pRjGdVWcBpjRT4LbfHaeKB7mDJxuYPAScocLD3AzRMVcbB79MjK8UnMUn8tKanoRtbAR"
retcode=$?
if [ ${retcode} -ne 0 ]; then
   echo "Build failed" >&2
   exit ${retcode}
fi

echo "WARNING: This script will be removed in the future. Please adjust your workflow" >&2

