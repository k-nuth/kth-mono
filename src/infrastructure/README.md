<!-- <a target="_blank" href="http://semver.org">![Version][badge.version]</a> -->
<!-- <a target="_blank" href="https://cirrus-ci.com/github/k-nuth/infrastructure">![Build Status][badge.Cirrus]</a> -->

# infrastructure <a target="_blank" href="https://github.com/k-nuth/infrastructure/releases">![Github Releases][badge.release]</a> <a target="_blank" href="https://github.com/k-nuth/infrastructure/actions">![Build status][badge.GhA]</a> <a href="#">![CPP][badge.cpp]</a> <a target="_blank" href="https://t.me/knuth_cash">![Telegram][badge.telegram]</a> <a target="_blank" href="https://k-nuth.slack.com/">![Slack][badge.slack]</a>

> Infrastructure functionality

## Getting started

Installing the library is as simple as:

1. Install and configure the Knuth build helper:
```
$ pip install kthbuild --user --upgrade

$ conan config install https://github.com/k-nuth/ci-utils/raw/master/conan/config2023.zip
```

2. Install the appropriate library:

```
$ conan install --requires=infrastructure/0.33.0 --update
```

For more more detailed instructions, please refer to our [documentation](https://kth.cash/docs/).

## About this library

This library can be used stand-alone, but it is probably convenient for you to use one of our main projects, [look over here](https://github.com/k-nuth/kth/).

## Issues

Each of our modules has its own Github repository, but in case you want to create an issue, please do so in our [main repository](https://github.com/k-nuth/kth/issues).

<!-- Links -->
[badge.Travis]: https://travis-ci.org/k-nuth/infrastructure.svg?branch=master
[badge.Appveyor]: https://ci.appveyor.com/api/projects/status/github/k-nuth/infrastructure?svg=true&branch=master
[badge.Cirrus]: https://api.cirrus-ci.com/github/k-nuth/infrastructure.svg?branch=master
[badge.GhA]: https://img.shields.io/endpoint.svg?url=https%3A%2F%2Factions-badge.atrox.dev%2Fk-nuth%2Finfrastructure%2Fbadge&style=for-the-badge
<!-- [badge.GhA]: https://github.com/k-nuth/infrastructure/workflows/Build%20and%20Test/badge.svg?branch=master&style=for-the-badge -->
[badge.version]: https://badge.fury.io/gh/k-nuth%2Finfrastructure.svg
[badge.release]: https://img.shields.io/github/v/release/k-nuth/infrastructure?display_name=tag&style=for-the-badge&color=00599C&logo=cplusplus
[badge.cpp]: https://img.shields.io/badge/C++-23-blue.svg?logo=c%2B%2B&style=for-the-badge
[badge.telegram]: https://img.shields.io/badge/telegram-badge-blue.svg?logo=telegram&style=for-the-badge
[badge.slack]: https://img.shields.io/badge/slack-badge-orange.svg?logo=slack&style=for-the-badge
<!-- [badge.Gitter]: https://img.shields.io/badge/gitter-join%20chat-blue.svg -->

