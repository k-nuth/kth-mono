# Branch Naming Conventions for CI Optimization

This project uses branch naming conventions to optimize CI/CD execution and avoid running heavy pipelines on changes that don't affect the software.

## 🚀 Branches that run full CI

- `master` - Main branch
- `release/**` - Release branches
- `hotfix/**` - Hotfix branches
- `dev-publish/**` - Development branches that publish packages
- `feature/**` - New feature branches
- `fix/**` - Bug fix branches
- `refactor/**` - Refactoring branches

## ⚡ Branches with optimized CI (basic validations only)

The following prefixes execute only lightweight validations and skip heavy CI:

### `docs/`
For documentation-only changes:
- README.md, CHANGELOG.md, LICENSE
- Files in `/doc/`, `/docs/`
- Documentation comments in code

**Example:** `docs/update-readme-installation`

### `style/`
For formatting and style changes:
- Copyright header changes
- Code reformatting (no functional changes)
- Typo fixes in comments
- Whitespace/indentation changes

**Example:** `style/update-copyright-2025`

### `chore/`
For maintenance tasks:
- Minor dependency updates
- Tool configuration changes
- Cleanup of obsolete files

**Example:** `chore/update-conan-deps`

### `noci/`
For changes that explicitly don't require CI:
- Temporary experiments
- Local configuration changes
- Tests that don't affect the software

**Example:** `noci/test-new-build-script`

## 📋 Rules and validations

1. **Automatic validation**: The `cosmetic-changes.yml` workflow verifies that cosmetic branches only contain allowed changes.

2. **Allowed files for cosmetic branches**:
   - `*.md`, `*.txt`, `*.rst`
   - `LICENSE*`, `CHANGELOG*`, `NOTICE*`, `AUTHORS*`
   - Files in `doc/`, `docs/`
   - Cosmetic CI workflows

3. **Auto-approval**: Cosmetic changes from trusted contributors can be auto-approved.

## 🛠️ How to use

### For documentation changes:
```bash
git checkout -b docs/improve-installation-guide
# make changes to README.md
git commit -m "docs: improve installation guide with examples"
git push origin docs/improve-installation-guide
```

### For style/formatting changes:
```bash
git checkout -b style/update-copyright-headers
# update copyright headers
git commit -m "style: update copyright to 2025"
git push origin style/update-copyright-headers
```

### For maintenance changes:
```bash
git checkout -b chore/cleanup-old-configs
# clean up obsolete files
git commit -m "chore: remove obsolete configuration files"
git push origin chore/cleanup-old-configs
```

## ⚠️ Important

- If your cosmetic branch includes actual code changes, CI will fail and ask you to use a different branch name.
- For mixed changes (code + documentation), use a regular prefix like `feature/` or `fix/`.
- Cosmetic branches are only allowed in Pull Requests, not in direct pushes to `master`.

## 🎯 Benefits

- ⏱️ **Time**: Documentation PRs validate in ~30 seconds vs ~45 minutes
- 💰 **Resources**: Saves CI/CD resources on GitHub Actions
- 🔄 **Productivity**: Faster feedback for minor changes
- 🌱 **Sustainability**: Lower computational resource consumption
