Using libzcloud
===============

The zcloud library provides a generic C API for cloud operations.

The library is built on the GTK+ C library: GLib.  This section assumes
familiarity with GLib, and in particular on GLib's object system
(GObject), error handling (GError), and data structures.

Source files using libzcloud functionality should begin with ::

  #include <zcloud.h>

.. toctree::

   using_libzcloud/library_setup
   using_libzcloud/module_info
   using_libzcloud/creating_a_store
