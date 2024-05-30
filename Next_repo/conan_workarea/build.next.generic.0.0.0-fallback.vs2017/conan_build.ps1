
$Env:CONAN_COLOR_DISPLAY="0"
$Env:CONAN_LOGIN_USERNAME_CONTI_CENTER="uig44320"
$Env:CONAN_PASSWORD_CONTI_CENTER="AKCp8pRjGdVWcBpjRT4LbfHaeKB7mDJxuYPAScocLD3AzRMVcbB79MjK8UnMUn8tKanoRtbAR"
$Env:ISDOCKERCONTAINER="1"
$Env:GIT_USER=""
$Env:GIT_PASSWORD=""
$Env:CIP_USER_CONFIG_DIR="C:\Users\uig44320\cip_config_dir"
$Env:CONAN_USER_HOME="C:\Users\uig44320\cip_config_dir\conan"
$Env:HOST_MACHINE_USERNAME="uig44320"
& "C:\Users\uig44320\cip_config_dir\bricks\4.25.0\windows\Scripts\python.exe" "-u" "C:/Users/uig44320/cip_config_dir/bricks/4.25.0/windows/Lib/site-packages/cip_build_system/conan_build_launcher" "--cip-version" "4.25.0" "--conan-workarea" "D:/Next-python_API/next/conan_workarea" "--selected-entity" "D:/Next-python_API/next/conan_workarea/build.next.generic.0.0.0-fallback.vs2017/selected_entity.json" "--env-username" "uig44320" "--env-password" "AKCp8pRjGdVWcBpjRT4LbfHaeKB7mDJxuYPAScocLD3AzRMVcbB79MjK8UnMUn8tKanoRtbAR"
if (-Not $?) {
    throw "Build failed; check output"
}

Write-Warning -Message "WARNING: This script will be removed in the future. Please adjust your workflow"
