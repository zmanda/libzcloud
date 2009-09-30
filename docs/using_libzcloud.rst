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
   using_libzcloud/uploading_and_downloading
   using_libzcloud/listing_keys
   using_libzcloud/testing_existence
   using_libzcloud/deleting_keys
   using_libzcloud/error_handling
   using_libzcloud/upload_producers
   using_libzcloud/download_consumers
   using_libzcloud/list_consumers
   using_libzcloud/progress_listeners
