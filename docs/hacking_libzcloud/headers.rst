.. _headers:

*******
Headers
*******

There are four kinds of code: plugin, internal, test, and user code.  Plugins and
user code should contain ::

  #include "zcloud.h"

while internal code should start with ::

  #include "internal.h"

and test code should start with ::

  #include "test.h"

``internal.h`` includes all of the public headers, as well as ``config.h`` and
a number of common system headers (but not ``stdio.h``).  In turn, ``test.h``
includes ``internal.h`` and adds some testing-specific functions.

Individual internal headers are located under ``lib/internal``, while individual public
headers (for use by user code and plugins) are under ``lib/zcloud``.

Symbols defined in internal headers should be flagged with ``G_GNUC_INTERNAL``,
which (with compatible compilers) hides the symbols from visibility outside of
``libzcloud.la``.  This forms a kind of library-wide *static* declaration: any
internal functions can be called from anywhere inside libzcloud.la, but cannot
be called by users or plugins.
