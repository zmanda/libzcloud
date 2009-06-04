.. _plugins:

*******
Plugins
*******

Plugins are contained in shared objects (modules) which are found in plugin
directories.  The default plugin directory is ``$libdir/zcloud-plugins``, but
this can be overridden at compile time (``--with-zcplugindir``) and at runtime
(``$ZCPLUGINPATH``).

Each plugin defines one prefix that can appear in store names, e.g., ``s3``.
There can be more than one plugin per module.
