import os
import shutil
import subprocess
import sys


def install():
    subprocess.check_call([sys.executable, "-m", "pip", "install", "markupsafe==2.0.1"])


home = os.path.expanduser("~")
install()

if os.path.exists(home + "/.conan/"):
    shutil.rmtree(home + "/.conan/")

shutil.copytree(home + "/cip_config_dir/conan/.conan", home + "/.conan/")

configfile = home + "/.conan/conan.conf"
with open(configfile, "r") as file:
    data = file.readlines()

newdata = []

for line in data:
    if line.find("user_home_short =") != -1:
        newdata.append("user_home_short = ~/cip_config_dir/conan/.conan # Provided by Bricks with override\n")
    elif line.find("path =") != -1:
        newdata.append("path = ~/cip_config_dir/conan/cache_data\n")
    else:
        newdata.append(line)

for line in newdata:
    print(line)

os.remove(configfile)
with open(configfile, "w") as file:
    file.writelines(newdata)
