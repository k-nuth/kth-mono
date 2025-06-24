import os
import json
from argparse import ArgumentParser
from os.path import expanduser
import subprocess
import shutil
from kthbuild import get_git_branch, access_file

# Usage
# rp = root path
# p = project
# python jenkins_dependencies.py -rp="$HOME/devel/testing_script/node" -p="node"

GITHUB_URL = "https://github.com/k-nuth/"
VERSION = "0.11.0"
CHANNEL = "testing"
COIN = "BCH"
REPO = "domain"
PATH = "$HOME/devel/testing_script/domain"
ALREADY_INSTALLED_DEPS = []
# Avoid building secp256k1 until the changes on the CI are completed
ALREADY_INSTALLED_DEPS.append('secp256k1')


def parse_args():
    parser = ArgumentParser('Knuth Build Dependencies Manager')
    parser.add_argument("-rp", "--root_path", dest="root_path", help="root path where the projects are",
                        default=expanduser("~"))
    parser.add_argument('-p', "--project", dest="project", type=str, nargs=1, help='Project that is going to be built')
    args = parser.parse_args()
    return args.root_path, args.project


def get_version(path):
    return access_file(path + '/conan_version')


def get_channel(path, default=None):
    try:
        return access_file(path + '/conan_channel')
    except IOError:
        return default


def get_branch_name(path):
    os.chdir(path)
    return get_git_branch("dev")


def get_dependencies():
    # Call it before using the os.chdir
    with open('dependencies.json') as data_file:
        data = json.load(data_file)
    return data


def build_dependency(dep_path, dep, branch):
    os.chdir(dep_path)
    try:
        # Already built
        ALREADY_INSTALLED_DEPS.index(dep)
        print(dep + ' already installed')
    except ValueError:
        # Build the dependency
        os.system('git clone ' + GITHUB_URL + dep + ' -b dev --recursive')
        os.chdir(dep_path + dep)
        # Use the same branch when available
        try:
            subprocess.check_call(['git', 'checkout', branch])
            print('git checkout out at ' + str(branch))
        except subprocess.CalledProcessError:
            subprocess.check_call(['git', 'checkout', 'dev'])
            print('git checkout out at dev')
        os.system('conan create . ' + dep + '/' + VERSION + '@kth/' + CHANNEL + ' -o *:currency=' + COIN + '')
        ALREADY_INSTALLED_DEPS.append(dep)
        print(dep + ' installed')


def build_dependencies(dep_path, dep_list, repo, branch):
    print('build dependencies called using ' + repo)
    for dep in dep_list[repo]:
        build_dependencies(dep_path, dep_list, dep, branch)
        build_dependency(dep_path, dep, branch)


def main():
    global VERSION
    global CHANNEL
    global COIN

    global PATH
    global REPO

    # Initialize vars
    PATH, REPO = parse_args()
    REPO = REPO[0]
    dep_list = get_dependencies()
    dep_path = os.path.dirname(PATH) + '/dependencies_temp/'
    print(dep_path)
    os.mkdir(dep_path)
    branch = get_branch_name(PATH)
    os.chdir(PATH)
    VERSION = get_version(PATH)
    CHANNEL = get_channel(PATH, branch)

    # Build dependencies
    build_dependencies(dep_path, dep_list, REPO, branch)
    os.chdir(PATH)
    shutil.rmtree(dep_path, ignore_errors=True)

    # print(PATH)
    # print(REPO)
    # print(branch)
    # print(VERSION)
    # print(CHANNEL)


if __name__ == '__main__':
    main()
