---
layout: development
title: Window.js | Maintenance
---

Maintenance
===========

Manual tasks to keep the project and site going.


Roll dependencies
-----------------

Roll the main [dependencies](/dev/dependencies) about once per month:

{: .strings}
| GLFW |
| libuv |
| Skia |
| v8 |

Verify that all of the
[build workflows](https://github.com/windowjs/windowjs/actions) pass after
the updates.


Site certificate
----------------

Verify if the SSL certificate for the site is due for a renewal.

The certificate is currently valid until **2022-03-01**.

The certificate is generated automatically by GitHub Pages using Let's Encrypt,
and should renew automatically. See the
[GitHub documentation](https://docs.github.com/en/pages/getting-started-with-github-pages/securing-your-github-pages-site-with-https).


Domain renewal
--------------

Verify if the domain name registration for `windowjs.org` is due for a renewal.

The domain is registered with [hostpoint.ch](https://hostpoint.ch) and is
currently registered until **2022-11-29**.


Windows code signing certificate renewal
----------------------------------------

The certificate to sign Windows binaries was created with
[Comodo SSLStore](https://comodosslstore.com/) and is valid until
**2023-01-14**.


macOS code signing certificate renewal
--------------------------------------

TODO: this is still open as
[issue #31](https://github.com/windowjs/windowjs/issues/31).
