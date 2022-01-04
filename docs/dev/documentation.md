---
layout: development
title: Window.js | Documentation
---

Documentation
=============

The documentation is built with [Jekyll](https://jekyllrb.com/), which
integrates with Github Pages.

To serve the documentation using Jekyll's web server during development:

```shell
$ cd doc
$ bundle exec jekyll serve
```

To generate the static site:

```shell
$ cd doc
$ bundle exec jekyll build
```

Any updates to the files under `docs/` triggers a rebuild of the site via the
`pages-build-deployment` workflow on Github. These changes are reflected at
[windowjs.org](https://windowjs.org) after a few seconds.
