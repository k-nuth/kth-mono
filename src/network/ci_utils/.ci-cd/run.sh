#!/bin/bash

set -e
set -x

# if [[ "$(uname -s)" == 'Darwin' ]]; then
#     if which pyenv > /dev/null; then
#         eval "$(pyenv init -)"
#     fi
#     pyenv activate conan
# fi

# echo $CONAN_CHANNEL
# eval $(python ci_utils/set_envvars_and_files.py)
# echo $CONAN_CHANNEL

# conan user
conan remote list

# conan remote add kth https://packages.kth.cash/api/ || true
# conan remote add kth https://packages.kth.cash/api/ || true


# conan user
# conan remote list

# python ci_utils/process_conan_reqs.py

# conan remove "*" -f || true
# conan remote remove kth || true
# conan remote remove kth || true

# conan remote remove kthbuild_kth_temp_ || true

# conan remote add tao https://taocpp.jfrog.io/artifactory/api/conan/tao -f || true


# if [[ "$(uname -s)" == 'Linux' ]]; then
#     conan profile show default
#     conan profile update settings.compiler.libcxx=libstdc++11 default
#     conan profile show default
# fi

echo "-----------------------------------------------------------"
conan config get
echo "-----------------------------------------------------------"
# conan config set general.revisions_enabled=1
# echo "-----------------------------------------------------------"
# conan config get
# echo "-----------------------------------------------------------"


python build.py
