# CONTRIBUTING
All contributions are welcome. If you find a bug or have a feature request, please open an issue first and then submit a pull request.

When thinking of contributing to this repository, please first discuss the change you wish to make via issue, email, or any other method with the owners of this repository before making a change. The developers or other contributors might be already working on that specific issue.

When contributing to this repository, you agree that your additions will be licensed under the project's main license, **Apache License 2.0** (the License), unless explicitly stated otherwise in subdirectories. Please refer to our [NOTICE](./NOTICE) file for a complete breakdown of module and asset licensing. Read the main license [here](./LICENSE).

Note that we have a [Code Of Conduct](CODE_OF_CONDUCT.md) which you are expected to follow in all your interactions with the project.
Serious violations of the Code of Conduct will result in a ban from future contributions.

Any contribution that intentionally goes against our Guidelines or the Code of Conduct will be ignored, and the contributor put in a blacklist. This also means that future interactions or contributions might be limited or not accepted.
This also applies if the violation isn't intentional but repeated.

While we welcome contributions, the maintainers reserve the right to decline pull requests that do not align with the project's goals or quality standards.
Their decision is considered final, unless further information/instructions have been provided.

> NOTE: Neither the owner, nor any of the active maintainers, is allowed to use code from a pull-request that has not been accepted first.

## Guidelines
### Translation Contributions
Localization files in the `./po/` directory (in both raw source `.po`/`.pot` and compiled forms) are licensed under the **Apache License, Version 2.0**.

* **Hosted Weblate:** To maintain translation quality, prevent merge conflicts, and streamline localization workflow, translation contributions must be submitted via our [Hosted Weblate project](https://hosted.weblate.org/fconvert).
* **Pull Requests:** Do **not** submit direct pull requests containing localization files. Direct PRs modifying `./po/` files will be closed; only verified repository maintainers and owners are authorized to merge translation updates into the source tree.
* **Branding Protection:** Localization strings containing official project identifiers (such as `"fconvert"` or maintainer identity strings) are protected under Section 6 of the Apache License 2.0 and the visual asset terms in `./branding/`. Third-party forks redistributing derivative builds must rebrand these strings prior to distribution.
### Instructions for Bugs
Open an issue providing as much information as possible.
#### Required:
- Version of the program
- Manually built or prebuilt (downloaded from the releases page)
- Does the program initiate? (test with running `fconvert -v` and `fconvert -i`)
- When, during runtime, does the bug occur?
- Provide logs if possible (usually in `~/.local/state/fconvert/fconvert-date-time.log` or `%APPDATA%/fconvert/Logs/fconvert-date-time.log`)
- Screenshots, recordings, etc
#### Optional:
- OS name and version
- Kernel version `uname -r`
- gcc and CMake version, if manually built
- Dependencies versions (runtime and/or compile time)
- Part of the codebase you believe is causing the bug

> NOTE: If you believe you are able to resolve or fix that issue, suggest to be considered as a contributor in the opened issue, and then you may open a pull request with your version of the fix.

### Instructions for Features
Open an Issue with the following name: `FEATURE REQUEST: name-of-your-feature`. The feature name must be descriptive of the feature you want added.

During feature requests, you are expected to provide complete and clear descriptions and provide extra information if one of the maintainers asks for it.

> NOTE: If you believe you are able to add that feature, suggest to be considered as a contributor in the opened issue, and then you may open a pull request with your version of the feature.

### General Instructions
If you have added a feature, make sure to document it in the README.md accordingly.

After you have finished writing and testing the code for your contribution, it is mandatory that you add this section in the pull request description with your features, fixes, and possible bugs.
Here are the fields you should include in your description:
```
### Patch Notes #issue-index

contributor : your-username@github/gitlab;
email : <your-public-email>;
name : feature/bug-name-from-issues;
type : bug/feature;
index : #pull-request-index;
Features :
* List-your-features-here
    - this-is-how-you-list-extra-details
    - ...
Fixed :
* List-your-bug-fixes-here
    - this-is-how-you-list-extra-details
    - ...
Possible Bugs :
* List-possible-bugs-that-happened-while-testing-your-release
    - this-is-how-you-list-extra-details
    - ...
Comment : "Your comments about this contribution"
```
**!!! LEAVE the `index` field as is !!!** 
It is strictly used and modified by maintainers and added to `./contributions_changelogs/BUG_CHANGELOGS.md` or `./contributions_changelogs/FEATURE_CHANGELOGS.md`
Here is an example:

`### Patch Notes #000`
```
contributor : Eraldo-Bako@github;
email : <eraldobako@gmail.com>;
name : Add XXX to YYY;
type : feature;
index : #pull-request-index;
Features :
* I added XXX
    - to YYY
    - Used code from fileXXX.cpp
Possible Bugs :
* ZZZ sometimes appears blank
Comment : "I enjoyed adding this feature since I use something like this a lot. I used code from github.com/another-repo/WOW-CODE to implement it"
```
If you are using code from another repository:
1. Make sure the owner of that repository allows using their code
2. Make sure the license is compatible with Apache v2.0 (the License)
3. Make sure to credit the original creator in the `NOTICE` file and add their license(and possibly notice file) to `./codebase-licenses/THEIR_LICENSE`. Make sure to rename the license as `original_repository_name-LICENSE`.

Some licenses compatible with Apache v2.0 (the License) include:
- MIT License
- BSD (2-Clause / 3-Clause)
- Apache 2.0 (Obviously): Preserve their NOTICE in `./codebase-licenses/THEIR_NOTICE` and add a clause in `NOTICE` indicating its location
- CC0 / Public Domain
- Boost Software License (BSL)

Avoid adding **source code** from codebases using the following licenses:
- GPLv1, GPLv2, GPLv3, and AGPLv3
- LGPL (v2.1 / v3)
- Mozilla Public License 2.0 (MPL 2.0)
- Eclipse Public License (EPL)
- Non-software Creative Commons licenses (CC BY-SA, CC BY-NC, etc.) for source code modules
- other restrictive, copy-left, non-Apache v2.0 (the License) compatible

### Allowed sources and use of AI/LLM
If you used/plan to use outside sources while contributing to our project, make sure the owners of those sources permit it and credit them in the NOTICE file.
Additionally, the use of AI (artificial intelligence) or LLMs (Large Language Models) for writing code is prohibited.
Uploaded code should be 100% human-written and human-thought.
If you are a beginner and use them as learning tools to explain certain fields or operations within the code, that is tolerated, but make sure you are writing code yourself. Online forums are a great help when learning coding techniques than AI suggestions. It's okay to struggle writing code, the important part is that you are learning.
You are not allowed, however, to use them as code debuggers. You are expected to fully-check and debug your additions before publishing.
The use of such tools is also prohibited when describing the features, bugs, or fixes in your code additions, README.md, and inside descriptions when opening an issue or a pull request.

### What you are allowed to modify
> IMPORTANT: The following limits apply to any contributor who is not a verified repository maintainer or owner, unless a temporary grant has been given to you by a verified repository maintainer or owner.
Freely modify:
- main.cpp
- Everything inside the `classes/` directory
- CMakeLists.txt
- PKGBUILD

Only ADD to:
- NOTICE
- Directory `./codebase-licenses/`, add the original licenses and notices
- README.md - only add a section for your feature, maintainers will fix the rest

OFF Limits:
- LICENSE
- PROGRESS.md
- CHANGELOG.md
- `contributions_changelogs/` directory and everything inside
- vcpkg.json
- resources.rc
- `input_file/` & `output_file/` directories and everything inside
- .gitignore
- `.github/` directory and everything inside
- `branding/` directory and everything inside
- Any file with the `.po` or `.pot` extension and the `po/` directory and everything inside
- etc

Your pull request should only contain source code, therefore, you mustn't include build artifacts, binaries, or anything either not related or unnecessary.

## CODE OF CONDUCT
We are committed to fostering an environment that respects and promotes the dignity, rights, and contributions of all individuals, regardless of characteristics including race, ethnicity, caste, color, age, physical characteristics, neurodiversity, disability, sex or gender, gender identity or expression, sexual orientation, language, philosophy or religion, national or social origin, socio-economic position, level of education, or other status.
This project follows the [Contributor Covenant](https://www.contributor-covenant.org/) as our Code of Conduct.

### Our Pledge
We pledge to make our community welcoming, safe, and equitable for all.

### Enforcement
Instances of abusive, harassing, or otherwise unacceptable behavior may be reported by contacting the project owners at `eraldobako@gmail.com` or opening an issue `Report: descriptive-title`. All complaints will be reviewed and investigated as soon as possible and fairly.

### You are expected to read the full text of our expectations before participating: [CODE OF CONDUCT](./CODE_OF_CONDUCT.md)