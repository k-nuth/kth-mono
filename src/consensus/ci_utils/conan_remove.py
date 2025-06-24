import subprocess
import sys

def get_conan_search(reference, remote=None, default=None):
    try:
        if remote is None:
            params = ["conan", "search", reference]
        else:
            params = ["conan", "search", reference, "-r", remote]

        res = subprocess.Popen(params, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        output, _ = res.communicate()
        if output:
            if res.returncode == 0:
                return output.decode("utf-8")
        return default
    except OSError: # as e:
        return default
    except:
        return default

def get_package_ids(reference, remote=None, default=None):
    res = []
    str_res = get_conan_search(reference, remote)
    # print(str_res)

    for l in iter(str_res.splitlines()):
        idx = l.find("Package_ID:") #, start, end)
        # print(idx)
        if idx != -1:
            # print(l)
            res.append(l[idx + len("Package_ID:") + 1:])
    return res

def exec_conan_remove(reference, package, remote=None, default=None):
    try:
        if remote is None:
            params = ["conan", "remove", reference, "--packages", package, "--force"]
        else:
            params = ["conan", "remove", reference, "--packages", package, "-r", remote, "--force"]

        res = subprocess.Popen(params, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        output, _ = res.communicate()
        if output:
            if res.returncode == 0:
                return output.decode("utf-8")
        return default
    except OSError: # as e:
        return default
    except:
        return default

def main():

    res = get_conan_search("secp256k1", "kth")
    print(res)
    return


    if (len(sys.argv) < 3):
        print('Arguments: <reference> <remote> [remove]')
        return

    remove = False
    if (len(sys.argv) >= 4):
        if sys.argv[3] == "remove":
            remove = True

    reference = sys.argv[1]
    remote = sys.argv[2]

    print("Reference: " + reference)
    print("Remote: " + remote)
    print("Remove packages: " + str(remove))

    # reference = "secp256k1/0.6.0@kth/staging"
    # remote = "kth"
    packages = get_package_ids(reference, remote)
    print("Packages: " + str(packages))

    if remove:
        for p in packages:
            res = exec_conan_remove(reference, p, remote)
            print(res)

if __name__ == "__main__":
    # execute only if run as a script
    main()