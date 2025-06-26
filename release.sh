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

gh pr create --title "release: ${VERSION}" --body "release: ${VERSION}" --base master --head "release/${VERSION}"

echo "Waiting for the build to finish for branch: release/${VERSION}"
while true; do
    # Get workflow runs for the current branch, including event type
    run_info=$(gh run list --branch "release/${VERSION}" --workflow "Build and Test" --limit 5 --json status,conclusion,url,number,event)
    
    if [ -z "$run_info" ] || [ "$run_info" == "[]" ]; then
        echo "No workflow runs found for branch release/${VERSION}. Waiting..."
        sleep 30
        continue
    fi
    
    # Find the most recent run that was triggered by 'push' (not 'pull_request')
    push_run=$(echo "$run_info" | jq -r '.[] | select(.event == "push") | . | @base64' | head -1)
    
    if [ -z "$push_run" ]; then
        echo "No push-triggered workflow runs found for branch release/${VERSION}. Waiting..."
        sleep 30
        continue
    fi
    
    # Decode the base64 encoded JSON
    push_run_decoded=$(echo "$push_run" | base64 --decode)
    
    status=$(echo "$push_run_decoded" | jq -r '.status')
    conclusion=$(echo "$push_run_decoded" | jq -r '.conclusion')
    url=$(echo "$push_run_decoded" | jq -r '.url')
    run_number=$(echo "$push_run_decoded" | jq -r '.number')
    event=$(echo "$push_run_decoded" | jq -r '.event')
    
    echo "Workflow run #${run_number} (${event}): status=${status}, conclusion=${conclusion}"
    echo "URL: ${url}"
    
    if [ "$status" == "completed" ]; then
        if [ "$conclusion" == "success" ]; then
            echo "✅ Build completed successfully!"
            break
        else
            echo "❌ Build completed but failed with conclusion: ${conclusion}"
            echo "Please check the workflow at: ${url}"
            exit 1
        fi
    else
        echo "🔄 Build is still in progress (${status}). Waiting..."
        sleep 30
    fi
done

# squash merge the PR, do not delete the branch
gh pr merge --squash --auto "release/${VERSION}"

# switch to master and pull latest changes
git checkout master
git pull origin master

# create and push the version tag
echo "Creating tag v${VERSION}..."
git tag -a "v${VERSION}" -m "Release version ${VERSION}"
git push origin "v${VERSION}"

# create GitHub release
echo "Creating GitHub release v${VERSION}..."
gh release create "v${VERSION}" \
    --title "v${VERSION}" \
    --generate-notes \
    --latest

echo "✅ Release v${VERSION} created successfully!"

# remove the release branch locally and remotely
git push origin --delete "release-${VERSION}"
git branch -D "release-${VERSION}"
