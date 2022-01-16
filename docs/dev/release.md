---
layout: development
title: Window.js | Releases
---

Releases
========

Only project maintainers can create new releases.

Signing the binaries is still a manual process at this time.


Creating a new release
----------------------

Start with `Draft a new release` on the GitHub
[Releases](https://github.com/windowjs/windowjs/releases) page.

Enter the new tag name under `Choose a tag` and choose `Create a new tag`.

Mark it as a pre-release, so that the [download](/download) links keep pointing
to the previous release as the latest.

This also triggers the build workflows.


Get binaries from the build actions
-----------------------------------

Drafting a new release with trigger the
[build actions](https://github.com/windowjs/windowjs/actions).

Once they've finished, download the build artifacts for Windows and macOS,
which contain the binaries built.


Test the binaries
-----------------

The build actions only succeed if the automated tests also pass.

As an additional step, verify manually that the binaries work as expected on
their corresponding platforms.



Sign the Windows binary
-----------------------

```shell
signtool.exe sign  /tr http://timestamp.digicert.com /td sha256 /fd sha256 /f C:\Path\To\JoaoDaSilva.pfx /p passwordgoeshere windowjs.exe
```

Verify the signature:

```shell
signtool.exe verify /pa windowjs.exe
```


Sign the macOS binary
---------------------

TODO: this is still open as
[issue #31](https://github.com/windowjs/windowjs/issues/31).


Attach the binaries to the release
----------------------------------

Package the binaries in a zip file with the appropriate name and attach them
to the release:

{: .strings}
| windowjs-macos-x64.zip |
| windowjs-windows-x64.zip |

Finally, unmark the release as a pre-released and update it.


Verify the downloads
--------------------

The links in [downloads](/download) point to the latest version released.
Verify that they get the files that were just released.
