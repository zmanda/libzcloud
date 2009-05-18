.. _plugins:

*******
Plugins
*******

Plugins are contained in shared objects (modules) which are found in plugin
directories.  The default plugin directory is ``$libdir/zcloud-plugins``, but
this can be overridden and compile time (``--with-zcplugindir``) and at runtime
(``$ZCPLUGINPATH``).

Each plugin defines one prefix that can appear in store names, e.g., ``s3``.
There can be more than one plugin per module.

Each module is accompanied by an XML file describing the plugins it contains.
The name of the XML file is not critical, but should match the name of the
shared object without the "lib" prefix.  For example, ``libs3.so`` is paired with
``s3.xml``.  The XML file has the following structure::

    <zcloud-module basename="mycloud">
      <store-plugin prefix="mycl">
      </store-plugin>
    </zcloud-module>

The ``basename`` given in the cloud-module tag is used to find the shared
object, and should name the object without any suffixes or prefixes.  The
shared object is assumed to reside in the same directory as the XML file.

This file will be expanded significantly to include machine-parsable
information on the specific characteristics of the plugin and other useful
information.

Note that, at present, the XML is parsed with the GLib *Simple XML Parser*,
which does not handle non-UTF8 encodings, user entities, or processing
instructions.
