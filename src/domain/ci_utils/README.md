# knuth ci-utils

Set of functions used in Knuth's continuous integration.

# Requirements

* For the jenkins_utils (debian)
```
apt-get update
apt-get install build-essential -y
apt-get install gcc -y
apt-get install git -y
apt-get install cmake -y
apt-get install python -y
apt-get install python-pip -y
pip install conan --upgrade
pip install conan-package-tools
conan config install https://github.com/k-nuth/ci-utils/raw/master/conan/config2023.zip
```

# Usage
* jenkins_dependencies.py

```
python jenkins_dependencies.py -rp="$HOME/devel/testing_script/node" -p="node"
```
