---
layout: documentation
title: Window.js | Performance
object-name: performance
object-properties:
  - memory.jsHeapSizeLimit
  - memory.totalJSHeapSize
  - memory.usedJSHeapSize
object-methods:
  - now
---

Performance
===========

{% include property object="performance.memory" name="jsHeapSizeLimit"
   type="number"
%}

The maximum size of the heap, in bytes, that is available to the Javascript VM.


{% include property object="performance.memory" name="totalJSHeapSize"
   type="number"
%}

The total heap size, in bytes, allocated by the Javascript VM.


{% include property object="performance.memory" name="usedJSHeapSize"
   type="number"
%}

The currently active segment of the Javascript VM heap, in bytes.


{% include method object="performance" name="now" type="() => number" %}

Returns the number of milliseconds since the current process started.

The resolution of the timer is platform dependent. It is usually on the order of
a few micro- or nanoseconds. It uses the highest-resolution monotonic time
source on each supported platform.
