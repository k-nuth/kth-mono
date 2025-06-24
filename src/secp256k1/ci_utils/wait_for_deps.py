import os
import pipes
import platform
import sys
import time
import subprocess

remote = "kthbuild_kth_temp_"

def repo_name_from_ci():
    # REPO_NAME=$(echo $TRAVIS_REPO_SLUG| cut -d'/' -f 2)
    # echo $REPO_NAME

    full_name = os.getenv("KTH_REPO_NAME", None)

    if full_name is None:
        full_name = os.getenv("TRAVIS_REPO_SLUG", None)

    if full_name is None:
        full_name = os.getenv("APPVEYOR_REPO_NAME", None)

    if full_name is None:
        full_name = os.getenv("CIRRUS_REPO_FULL_NAME", None)

    if full_name is None:
        return ''

    return full_name.split('/')[1]

def get_conan_search(package, remote):
    try:
        # conan search infrastructure/0.X@kth/staging -r kth_temp > $null
        params = ["conan", "search", package, "-r", remote]
        command = " ".join(params)
        print("executing for %s ..." % (command,))
        res = subprocess.Popen(params, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        output, _ = res.communicate()
        print(output)
        if output:
            return res.returncode
        return 1
    except OSError: # as e:
        return 1
    except Error as e:
        return 1

deps_graph = {
    "secp256k1": [],
    "infrastructure": ["secp256k1"],
    "domain": ["infrastructure"],
    "consensus": ["secp256k1", "domain"],
    "database":["domain"],
    "network":["domain"],
    "blockchain":["database"],
    "node" : ["blockchain", "network"],
    "rpc" : ["node"],
    "kth": ["node", "rpc"],
    "node-exe": ["node"],
    "c-api": ["node"]
}

# current = sys.argv[1]
current = repo_name_from_ci()
print("wait for deps repo name: %s ..." % (current,))

deps = deps_graph[current]

for dep in deps:
    package = "%s/0.X@kth/staging" % (dep,)
    print("waiting for %s ..." % (package,))
    res = get_conan_search(package, remote)
    while res != 0:
        # print('.', end='')
        sys.stdout.flush()
        time.sleep(1)
        res = get_conan_search(package, remote)
    print("%s found." % (package,))
