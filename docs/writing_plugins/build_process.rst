Plugin Build Process
====================

Local Plugins
-------------

Local plugins are stored in subdirectories of ``plugins/``, and are integrated
into libzcloud's compilation process.  They should include a ``configure.m4``,
which will be automatically linked into the top-level ``configure.ac`` by
``autogen``.  This M4 file should invoke :cmacro:`ZCLOUD_DEFINE_PLUGIN`:

.. index:: ZCLOUD_DEFINE_PLUGIN (autoconf macro)

.. describe:: ZCLOUD_DEFINE_PLUGIN(name, bydefault, description, if-enabled)

    :param name: plugin name (name of the ``plugins/`` subdirectory)
    :param bydefault: installed by default (``yes`` or ``no``)
    :param description: description for ``./configure --help``
    :param if-enabled: shell code to execute if the plugin is enabled

    Define a plugin.  This adds ``--enable-NAME`` and ``--disable-NAME``
    options, with the appropriate default.  At configure time, if the plugin is
    enabled, *if-enabled* is executed.

    This macro adds the plugin subdirectory's Makefile to ``AC_CONFIG_FILES``,
    and adds the subdirectory itself to the list of directories to be built by
    the top-level Makefile.

The plugin's ``Makefile.am`` should look something like::

    INCLUDES = -I $(top_srcdir)/lib -I $(top_srcdir)/config
    AM_CPPFLAGS = $(GLIB_CFLAGS)

    zcplugin_DATA = mymodule.xml
    EXTRA_DIST = $(zcplugin_DATA)

    zcplugin_LTLIBRARIES = libmymodule.la
    libmymodule_la_LDFLAGS = -avoid-version -module
    libmymodule_la_SOURCES = \
        mymodule.h \
        mymodule.c

The ``INCLDUES`` point to ``lib/``, which contains ``zcloud.h``, and to
``config/``, which contains ``config.h``.  The ``AM_CPPFLAGS`` include the
flags required to compile against GLib.

The next stanza installs the module's XML file (``mymodule.xml``) into the
plugin directory, and ensures that it is included in the distribution tarball.

The final stanza defines the plugin shared object, including the required
libtool flags and the list of header and source files
(``libmymodule_la_SOURCES``).  Any additional required libraries would appear
in ``libmymodule_la_LIBADD``.

Because the plugin is configured along with the rest of libzcloud, any C
preprocessor symbols are added to libzcloud's ``config.h``.  This file is not
included by ``zcloud.h``, so it must be named explicitly in every source file.
Such files should begin with:

.. code-block:: c

    #include "config.h"
    #include "zcloud.h"

followed by any plugin-specific header files.

External Plugins
----------------

(to be written)
