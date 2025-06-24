import os
import pipes
import platform
import sys
import time
import subprocess

repos = {
    "node-exe": "kth",
}

def repo_name_from_ci():
    full_name = os.getenv("TRAVIS_REPO_SLUG", None)

    if full_name is None:
        full_name = os.getenv("APPVEYOR_REPO_NAME", None)

    if full_name is None:
        full_name = os.getenv("CIRRUS_REPO_FULL_NAME", None)

    if full_name is None:
        return ''

    return full_name.split('/')[1]

def translate_repo_name(name):
    if name in repos:
        return repos[name]
    return name

current = repo_name_from_ci()
current = translate_repo_name(current)
print(current)
