import subprocess

def get_git_branches(default=None):
    try:
        res = subprocess.Popen(["git", "branch", "-r"], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        # res = subprocess.Popen(["git", "branch"], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        output, _ = res.communicate()
        if output:
            if res.returncode == 0:
                # return output.decode("utf-8").replace('\n', '').replace('\r', '')
                return output.decode("utf-8")
        return default
    except OSError: # as e:
        return default
    except:
        return default

def release_branch_version(branch):
    # print("%s" % branch)
    version = branch.split('-')[-1]
    # print(version)
    verarr = version.split('.')

    if len(verarr) != 3:
        return (None, None)

    # print(verarr)
    verstr = verarr[0].zfill(5) + verarr[1].zfill(5) + verarr[2].zfill(5)
    # print(verstr)
    return (int(verstr), version)

def max_release_branch(default=None):
    branches = get_git_branches()
    if branches is None:
        return False

    max = None
    max_str = None

    for line in branches.splitlines():
        line = line.strip()
        # print(line)
        if line.startswith("origin/release-"):
            veri, vers = release_branch_version(line)
            if veri is not None:
                if max is None or veri > max:
                    max = veri
                    max_str = vers

    return (max, max_str)


print(max_release_branch())
# print(get_git_branches())