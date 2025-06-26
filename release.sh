#!/bin/bash

# VERSION=$(cat conanfile.py | grep "version" | cut -d '"' -f 2)
VERSION="0.68.0"

echo "Building version: ${VERSION}"

git checkout master
git stash
git pull origin master
git stash pop
git checkout -b "release/${VERSION}"

rm -rf build
rm -rf conan.lock

conan lock create conanfile.py --version="${VERSION}" --update

git add conan.lock
git commit -m "release: update conan.lock for version ${VERSION}"
git push origin "release/${VERSION}"

# create a PR for the release
gh pr create --title "release: ${VERSION}" --body "release: ${VERSION}" --base master --head "release/${VERSION}"
# squash merge the PR, do not delete the branch
gh pr merge --squash --auto --merge "release/${VERSION}"
# we need to give time to GhA to run the build and publish the package
# wait for the build to finish
echo "Waiting for the build to finish..."
while true; do
    status=$(gh run list --limit 1 --json status --jq '.[0].status')
    if [ "$status" == "completed" ]; then
        echo "Build completed successfully."
        break
    else
        echo "Build is still in progress. Waiting..."
        sleep 30
    fi
done

# remove the release branch locally and remotely
git push origin --delete "release/${VERSION}"
git branch -D "release/${VERSION}"
git checkout master
git pull origin master




# conan lock create conanfile.py --version "${VERSION}" --lockfile=conan.lock --lockfile-out=build/conan.lock
# conan install conanfile.py --lockfile=build/conan.lock -of build --build=missing

# cmake --preset conan-release \
#          -DCMAKE_VERBOSE_MAKEFILE=ON \
#          -DGLOBAL_BUILD=ON \
#          -DCMAKE_BUILD_TYPE=Release

# cmake --build --preset conan-release -j4 \
#          -DCMAKE_VERBOSE_MAKEFILE=ON \
#          -DGLOBAL_BUILD=ON \
#          -DCMAKE_BUILD_TYPE=Release
