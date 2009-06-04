Plugin XML Definitions
======================

Each module is accompanied by an XML file describing the plugins it contains.
The name of the XML file is not critical, but should match the name of the
shared object without the "lib" prefix.  For example, ``libs3.so`` is paired
with ``s3.xml``.  The XML file has the following structure::

    <zcloud-module basename="mycloud">
      <store-plugin prefix="mycl">
      </store-plugin>
    </zcloud-module>

Note that, at present, the XML is parsed with the GLib *Simple XML Parser*,
which does not handle non-UTF8 encodings, user entities, or processing
instructions.

.. index:: zcloud-module

.. describe:: zcloud-module

    :arg basename: the base name of this module

    The ``zcloud-module`` element contains all other elements provided by the
    module.  It can contain the following elements:

       * ``store-plugin``

    The base name is used to find the shared object, and should name the object
    without any suffixes or prefixes.  The shared object is assumed to reside
    in the same directory as the XML file.

.. index:: store-plugin

.. describe:: store-plugin

    :arg prefix: the store-specifier prefix that identifies this plugin

    The ``store-plugin`` element identifies a particular plugin within a module,
    and contains elements defining the characteristics of this plugin.  It can
    not contain any other elements.
