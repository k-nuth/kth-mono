import os
import pipes
import platform
from utils import get_version

if 'KTH_BRANCH' in os.environ:
    branch = os.environ.get('KTH_BRANCH')
elif 'TRAVIS_BRANCH' in os.environ:
    branch = os.environ.get('TRAVIS_BRANCH')
elif 'APPVEYOR_REPO_BRANCH' in os.environ:
    branch = os.environ.get('APPVEYOR_REPO_BRANCH')

os.environ['KTH_BRANCH'] = branch       # needed by get_version()

#Note: duplicated in utils.branch_to_channel
if branch == 'dev':
    channel = "testing"
    full_build = "0"
elif branch.startswith('release'):
    channel = "staging"
    full_build = "1"
elif branch.startswith('hotfix'):
    channel = "staging"
    full_build = "1"
elif branch.startswith('feature'):
    channel = branch
    full_build = "0"
else:
    channel = "staging"
    full_build = "1"

# export KTH_CONAN_VERSION="$(python ci_utils/print_version.py)"
version = get_version()

if not os.path.exists('conan_version'):
    with open("conan_version", "w") as file:
        file.write(version)

if platform.system() == "Windows":
    export_str = '$Env:KTH_BRANCH="%s"; $Env:KTH_CONAN_CHANNEL="%s"; $Env:KTH_FULL_BUILD="%s"; $Env:KTH_CONAN_VERSION="%s"; $Env:CONAN_CHANNEL="%s";'
else:
    export_str = "export KTH_BRANCH=%s KTH_CONAN_CHANNEL=%s KTH_FULL_BUILD=%s KTH_CONAN_VERSION=%s CONAN_CHANNEL=%s "

print(export_str % (pipes.quote(str(branch)),
                    pipes.quote(str(channel)),
                    pipes.quote(str(full_build)),
                    pipes.quote(str(version)),
                    pipes.quote(str(channel)),
     ))



