.. _xml-definition:

Plugin XML Definition
=====================

Each module is accompanied by an XML file describing the plugins it contains.
The name of the XML file is not critical, but should match the name of the
shared object without the "lib" prefix.  For example, ``libs3.so`` is paired
with ``s3.xml``.  The XML file has the following structure::

    <zcloud-module basename="mycloud">
      <store-plugin prefix="mycl">
        <parameter name="access_key" type="string">
          Authentication token
        </parameter>
        <parameter name="redundancy" type="int">
          Requested storage redundancy level
        </parameter>
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
    contain the following element:

       * ``parameter``

.. index:: parameter

.. describe:: parameter

    :arg name: the name of the parameter
    :arg type: the type of the parameter
    :arg nick: un-abbreviated form of *name* (optional; defaults to *name*)
    :arg blurb: a short human-readable description of the parameter

    The ``parameter`` element describes a constructor parameter of the
    store plugin.  It is always an empty element.  The parameter type is
    used for validation and to convert the type to useful data within
    the module.  It is case-insensitive and must be one of:

        * ``string``

    The parameter name must start with an ASCII lower-case letter, and all
    subsequent digits should be lower-case letters, digits, or '-' (dash).  The
    blurb may be displayed in the online help of applications linked to
    libzcloud, so it should be short (less than, say, 50 characters),
    declarative, and not begin with "the".
