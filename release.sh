#!/bin/bash

if [ -z "$1" ]; then
    echo "Usage: $0 <version>"
    exit 1
fi
VERSION="$1"

echo "Building version: ${VERSION}"

git checkout master
git stash
git pull origin master
git stash pop
git checkout -b "release/${VERSION}"

# Update README versions before creating the release
echo "Updating README files to version: ${VERSION}"
./scripts/update_readme_versions.sh "${VERSION}"

# Commit README updates
if git diff --quiet src/*/README.md; then
    echo "No README changes needed"
else
    git add src/*/README.md
    git commit -m "docs: update README versions to ${VERSION}"
fi

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

# Step 1: Create temporary release to generate notes
echo "Creating temporary release to generate notes..."
TEMP_TAG="temp-v${VERSION}"
git tag -a "${TEMP_TAG}" -m "Temporary tag for release notes generation"
git push origin "${TEMP_TAG}"

gh release create "${TEMP_TAG}" \
    --title "temp-v${VERSION}" \
    --generate-notes \
    --prerelease

# Step 2: Extract the generated notes
echo "Extracting generated release notes..."
RELEASE_NOTES=$(gh release view "${TEMP_TAG}" --json body -q '.body')

if [ -z "$RELEASE_NOTES" ]; then
    echo "❌ Failed to extract release notes"
    exit 1
fi

# Step 3: Update local release notes file
echo "Updating local release notes file..."
NOTES_FILE="doc/release-notes/release-notes.md"

# Create a backup
cp "$NOTES_FILE" "${NOTES_FILE}.backup"

# Prepare the new release notes entry
NEW_ENTRY="# version ${VERSION}

You can install Knuth node version v${VERSION} [using these instructions](https://kth.cash/#download).

${RELEASE_NOTES}

"

# Add the new entry at the top of the file (after any existing content)
{
    echo "$NEW_ENTRY"
    cat "$NOTES_FILE"
} > "${NOTES_FILE}.tmp" && mv "${NOTES_FILE}.tmp" "$NOTES_FILE"

echo "✅ Updated release notes file"

# Step 4: Commit the updated release notes
git add "$NOTES_FILE"
git commit -m "docs: update release notes for v${VERSION}"
git push origin master

# Step 5: Clean up temporary release and tag
echo "Cleaning up temporary release..."
gh release delete "${TEMP_TAG}" --yes
git tag -d "${TEMP_TAG}"
git push origin --delete "${TEMP_TAG}"

# Step 6: Create the real release with the processed notes
echo "Creating final release v${VERSION}..."
git tag -a "v${VERSION}" -m "Release version ${VERSION}"
git push origin "v${VERSION}"

# Get the updated notes from the file for the final release
FINAL_NOTES=$(awk "
    /^# version ${VERSION}$/ { found=1; next }
    /^# version / && found { exit }
    found && NF > 0 { print }
" "$NOTES_FILE" | sed '/^$/d')

gh release create "v${VERSION}" \
    --title "v${VERSION}" \
    --notes "$FINAL_NOTES" \
    --latest

echo "✅ Release v${VERSION} created successfully!"
echo "✅ Release notes have been updated in $NOTES_FILE"

# remove the release branch locally and remotely
git push origin --delete "release-${VERSION}"
git branch -D "release-${VERSION}"
