.. _module_info:

Getting Module Information
==========================

Among other things, :cfunc:`zcloud_init` gathers basic information about the
available plugins.  It does this in such a way that those plugins are not
actually loaded, so it is quick and does not require that all dependencies of
all plugins be available.

Information on available plugins is available from either of these functions:

.. cfunction:: ZCloudStorePlugin *zcloud_get_store_plugin_by_prefix(const gchar *prefix)

    Get the plugin object for the given prefix.

    :param prefix: desired storage specification prefix
    :returns: a plugin object, or NULL if not found

.. cfunction:: GSList *zcloud_get_all_store_plugins(void)

    Get a list of all plugins.

    :returns: a GSList of pointers to :ctype:`ZCloudStorePlugin` objects

Plugin Information
------------------

.. ctype:: ZCloudStorePlugin

    Information about a single libzcloud plugin

    .. cmember:: gchar *ZCloudStorePlugin.prefix

        The prefix for this plugin

    .. cmember:: ZCloudModule *ZCloudStorePlugin.module

        Information about the module that will define this plugin

    .. cmember:: GType ZCloudStorePlugin.type

        The type of the store class that will be instantiated for this plugin

    .. cmember:: GPtrArray *ZCloudStorePlugin.paramspecs

        A list of ``GParamSpec`` objects describing the parameters for this plugin.
        The GLib documentation has the complete story on these objects.  Briefly,
        the parameter name is availble in ``spec.name``, the expected type is in
        ``spec.value_type``, and the nick and blurb are available via
        ``g_param_spec_nick(spec)`` and ``g_param_spec_blurb(spec)``, respectively.

Module Information
------------------

.. ctype:: ZCloudModule

    Information about a module implementing plugins

    .. cmember:: gchar *ZCloudStorePlugin.basename

        Base name of this module

    .. cmember:: gchar *ZCloudStorePlugin.module_path

        Full pathname to the loadable shared object

    .. cmember:: gchar *ZCloudStorePlugin.xml_path

        Full pathname of the :ref:`XML file <xml-definition>` defining this module

    .. cmember:: gboolean ZCloudStorePlugin.loaded

        True if this module has been loaded
