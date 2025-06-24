#!/bin/bash

set -e
set -x

if [[ "$(uname -s)" == 'Darwin' ]]; then
    # brew update
    brew outdated pyenv || brew upgrade pyenv
    brew install pyenv-virtualenv
    brew install cmake || true

    if which pyenv > /dev/null; then
        eval "$(pyenv init -)"
    fi

    # pyenv install 3.7.1
    # pyenv virtualenv 3.7.1 conan
    pyenv install 3.10.8
    pyenv virtualenv 3.10.8 conan

    pyenv rehash
    pyenv activate conan
fi

# pip install  --upgrade pip > /dev/null
## pip install "conan>=1.50.0,<2.0" --upgrade
# pip install "conan>=2.0" --upgrade
# pip install conan_package_tools==0.39.0 #> /dev/null
# pip install kthbuild --upgrade

conan --version
# conan user

uname -s

if [[ "$(uname -s)" == 'Linux' ]]; then
    # conan remote list
    conan profile new default --detect
    conan profile show default
    ls ~/.conan
    ls ~/.conan/profiles
    cat ~/.conan/profiles/default
    conan profile update settings.compiler.version=11 default
    conan profile update settings.compiler.libcxx=libstdc++11 default
    conan profile show default
fi

if [[ "$(uname -s)" == 'Darwin' ]]; then
    conan profile new default --detect
    conan profile show default
    conan profile update settings.compiler.version=13 default
    conan profile show default
fi