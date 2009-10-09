.. _library_setup:

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

Logging
-------

Libzcloud use glib's message logging facility, with the domain ``"ZCLOUD"``.
Glib's default handling for log messages is to print them to stdout or stderr,
so simple applications may wish to silence the logging::

    static void
    null_log(const gchar *log_domain G_GNUC_UNUSED,
         GLogLevelFlags log_level G_GNUC_UNUSED,
         const gchar *message G_GNUC_UNUSED,
         gpointer user_data G_GNUC_UNUSED)
    {
    }

    /* ... */
    g_log_set_handler(G_LOG_DOMAIN, G_LOG_LEVEL_MASK, null_log, NULL);
