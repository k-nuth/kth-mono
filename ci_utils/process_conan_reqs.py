import os
import subprocess
import fileinput
import platform
import json

_REMOTE = os.environ.get('KTH_CONAN_REMOTE', "kthbuild_kth_temp_")
# print("*************************************************")
# print(_REMOTE)
# print("*************************************************")

def get_conan_info(default=None):
    try:

        if platform.system() == "Linux":
            # conan info . --only requires -s compiler=gcc -s compiler.version=5 -s compiler.libcxx=libstdc++
            params = ["conan", "info", ".", "--only", "requires", "-s", "compiler=gcc", "-s", "compiler.version=5", "-s", "compiler.libcxx=libstdc++"]
        else:
            # # conan info . --only None
            # params = ["conan", "info", ".", "--only", "None"]
            # conan info . --only requires
            params = ["conan", "info", ".", "--only", "requires"]

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

def get_conan_info_json(default=None):
    try:

        if platform.system() == "Linux":
            # conan info . --only requires  --json -s compiler=gcc -s compiler.version=5 -s compiler.libcxx=libstdc++
            params = ["conan", "info", ".", "--only", "requires", "--json", "-s", "compiler=gcc", "-s", "compiler.version=5", "-s", "compiler.libcxx=libstdc++"]
        else:
            # # conan info . --only None
            # params = ["conan", "info", ".", "--only", "None"]
            # conan info . --only requires --json
            params = ["conan", "info", ".", "--only", "requires", "--json"]

        res = subprocess.Popen(params, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        output, _ = res.communicate()
        if output:
            if res.returncode == 0:
                s = output.decode("utf-8")
                pos = s.find('[{')
                ret = s[pos:]
                # print(ret)
                return ret

        return default
    except OSError: # as e:
        return default
    except:
        return default

# def get_conan_requires():
#     info = get_conan_info()
#     after_proj = False
#     after_requires = False

#     res = []
#     for line in info.splitlines():
#         if not after_proj:
#             if line == "PROJECT":
#                 after_proj = True
#         else:
#             if not after_requires:
#                 if line == "    Requires:":
#                     after_requires = True
#             else:
#                 if line.startswith("        "):
#                     line = line.strip()
#                     # print(line)
#                     # print(line[0] == '/t')
#                     # print(line[0] == ' ')
#                     pos = line.find('/')
#                     name = line[:pos]
#                     # print(name)
#                     if name == "secp256k1" or name.startswith("kth-"):
#                         # print(name)
#                         res.append(name)
#                 else:
#                     break
#     return res

def is_knuth_repo_name(name):
    repos = [
        'infrastructure',
        'domain',
        'consensus',
        'network',
        'database',
        'blockchain',
        'node',
        'rpc',
        'c-api',
        'kth',
        'secp256k1'
    ]

    ret = name in repos
    return ret
    # return name in repos


def get_conan_requires():
    res = []

    info_str = get_conan_info_json()
    # print(info_str)
    info_json = json.loads(info_str)
    # print(info_json)

    for e in info_json:
        if e['reference'].startswith("conanfile.py ("):
            # print(e['requires'])

            for req in e['requires']:
                pos = req.find('/')
                name = req[:pos]

                #TODO(fernando): check this:
                # # print(name)
                # if name == "secp256k1" or name.startswith("kth-"):
                #     # print(name)
                #     res.append(name)
                # print(name)
                # if name != "kthbuild" or name != "ci-utils" or not name.startswith("conan-"):
                #     # print(name)
                #     res.append(name)

                if is_knuth_repo_name(name):
                    # print(name)
                    res.append(name)

    # print(res)
    return res


def get_conan_get(package, remote=None, default=None):
    try:
        if remote is None:
            params = ["conan", "get", package]
        else:
            params = ["conan", "get", package, "-r", remote]

        # print("command to execute", " ".join(params))

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

def get_alias_version(package, remote=None, default=None):
    conan_alias = get_conan_get(package, remote, default)
    conan_alias = conan_alias.split('\n')[4:][0]
    return conan_alias[12:].replace('"', '')

# def write_req_file():
#     reqs = get_conan_requires()
#     print(reqs)
#     if len(reqs) == 0:
#         return

#     if not os.path.exists('conan_requirements'):
#         with open("conan_requirements", "w") as file:
#             for r in reqs:
#                 # print(r)
#                 alias = get_alias_version("%s/0.X@%s/%s" % (r, "kth", "staging"), "kth")
#                 pos = alias.find('@')
#                 alias = alias[:pos]
#                 alias = alias + "@%s/%s"
#                 # print(alias)
#                 file.write(alias)
#                 file.write("\n")

def replace_conan_recipe(recipe_file, text_to_search, replacement_text):
    # Read in the file
    with open(recipe_file, 'r') as file:
        filedata = file.read()

    # Replace the target string
    filedata = filedata.replace(text_to_search, replacement_text)

    # Write the file out again
    with open(recipe_file, 'w', newline='\n') as file:
        file.write(filedata)

    # # with fileinput.FileInput(filename, inplace=True, backup='.bak') as file:
    # with fileinput.FileInput(recipe_file, inplace=True) as file:
    #     for line in file:
    #         print(line.replace(text_to_search, replacement_text), end='')

def replace_conan_deps():
    reqs = get_conan_requires()

    print(reqs)

    if len(reqs) == 0:
        return

    if not os.path.exists('conan_requirements'):
        for r in reqs:
            # print(r)
            orig_req = ("%s/0.X@" % (r,)) + "%s/%s"
            # print(orig_req)
            alias = get_alias_version(orig_req % ("kth", "staging"), _REMOTE)
            # print(alias)
            pos = alias.find('@')
            alias = alias[:pos]
            # print(alias)
            alias = alias + "@%s/%s"
            # print(alias)
            replace_conan_recipe("conanfile.py", orig_req, alias)


channel = os.environ.get('KTH_CONAN_CHANNEL')
# channel = 'staging'
# print("--------------------------------------------")
# print("process_conan_reqs.py")
# print(f'*** process_conan_reqs.py channel: {channel}')
# print("--------------------------------------------")

if channel == 'staging':
    # write_req_file()
    replace_conan_deps()
