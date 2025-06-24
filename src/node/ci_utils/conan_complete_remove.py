import subprocess
import sys
from functools import cmp_to_key

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

def get_full_references(partial_ref, remote):
    res = []
    str_res = get_conan_search(partial_ref, remote)
    # print(str_res)

    for l in iter(str_res.splitlines()):
        idx = l.find(partial_ref) #, start, end)
        # print(idx)
        if idx != -1:
            # print(l)
            res.append(l)
    return res

def get_testing_full_references(partial_ref, remote):
    res = []
    full_references = get_full_references(partial_ref, remote)
    for ref in full_references:
        if "0.X" in ref:
            continue
        if "/testing" in ref:
            res.append(ref)
    return res

def get_staging_full_references(partial_ref, remote):
    res = []
    full_references = get_full_references(partial_ref, remote)
    for ref in full_references:
        if "0.X" in ref:
            continue
        if "/staging" in ref:
            res.append(ref)
    return res

# def get_removable_full_references(partial_ref, remote):
#     res = []
#     full_references = get_full_references(partial_ref, remote)
#     for ref in full_references:
#         if "0.X" in ref:
#             continue
#         if "/stable" in ref:
#             continue

#         res.append(ref)
#     return res

def stable_sorter(x, y):
    partX = x.split("@")[0]
    partX = partX.split("/")[1]
    partX = partX.split(".")
    mapX = map(int, partX)
    partX = list(mapX)

    partY = y.split("@")[0]
    partY = partY.split("/")[1]
    partY = partY.split(".")
    mapY = map(int, partY)
    partY = list(mapY)

    if partX < partY:
        return -1
    elif partX > partY:
        return 1
    else:
        return 0


def complete_remove(partial_ref, remote, remove):
    references = get_testing_full_references(partial_ref, remote)

    staging_references = get_staging_full_references(partial_ref, remote)
    staging_references = sorted(staging_references, key=cmp_to_key(stable_sorter))

    print("Latest Staging version is::")
    print(staging_references[-1])

    del staging_references[-1]
    references.extend(staging_references)

    print("References to remove packages:")
    for ref in references:
        print("    " + ref)


    for reference in references:
        packages = get_package_ids(reference, remote)
        print("For " + reference)
        print("    Packages: " + str(packages))

        if remove:
            for p in packages:
                res = exec_conan_remove(reference, p, remote)
                print(res)


def main():
    # partial_references = ["blockchain", "consensus", "database", "domain", "infrastructure", "kth", "network", "node", "secp256k1"]
    partial_references = ["secp256k1", "infrastructure", "consensus", "domain", "database", "blockchain", "network", "node", "kth", "c-api"]

    if (len(sys.argv) < 2):
        print('Arguments: <remote> [remove]')
        return

    remove = False
    if (len(sys.argv) >= 3):
        if sys.argv[2] == "remove":
            remove = True

    remote = sys.argv[1]

    # print("Reference: " + reference)
    print("Remote: " + remote)
    print("Remove packages: " + str(remove))

    for partial_ref in partial_references:
        complete_remove(partial_ref, remote, remove)

    # # reference = "secp256k1/0.6.0@kth/staging"
    # # remote = "kth"
    # packages = get_package_ids(reference, remote)
    # print("Packages: " + str(packages))

    # if remove:
    #     for p in packages:
    #         res = exec_conan_remove(reference, p, remote)
    #         print(res)

if __name__ == "__main__":
    # execute only if run as a script
    main()