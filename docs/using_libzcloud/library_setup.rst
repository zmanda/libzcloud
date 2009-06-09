Library Setup
=============

Before calling any other libzcloud functions, call ``zcloud_init`` to
set up the library.

.. cfunction:: gboolean zcloud_init(GError **error)

    Initializes libzcloud, including calling GLib's ``g_thread_init``
    and ``g_type_init``.  This function loads and caches information
    about the zcloud modules installed.

    An error from this function means that there was an error loading
    the module information, and is generally fatal.

    :param error: error information
    :returns: true on success
