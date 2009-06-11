.. _module-initialization:

Module Initialization
=====================

Libzcloud modules are loaded by GLib's *GModule* library, which
automatically runs `g_module_check_init` on load.  This is the
appropriate time to register all plugins supplied with the module.  The
function usually looks something like::

    #include "mycloud.h"
    #include <gmodule.h>

    const gchar *
    g_module_check_init(GModule *module)
    {
        return zcloud_register_store_plugin("mycloud",
                                            "mycloudstore",
                                            MYCLOUD_TYPE_STORE);
    }

This function should return `NULL` on success, and an error message on
failure, which is exactly the semantics of
`zcloud_register_store_plugin`.

.. cfunction:: gchar * zcloud_register_store_plugin(const gchar *module_name, const gchar *prefix, GType type)

    Register store subclass *type* as the class to instantiate for
    prefix *prefix*, defined as part of the module *module_name*.

    :param module_name: base name of the module being initialized
    :param prefix: prefix corresponding to this plugin
    :param type: type of a subclass of :class:`ZCloudStore`
    :returns: `NULL` on success, or an error message on failure
