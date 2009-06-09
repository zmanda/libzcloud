Creating a Store
================

TODO: fix indexing of 'store specifier'

.. index:: store specifier

A store object is described by a *store specifier*.  Each store
specifier begins with a *prefix*, which identifies the plugin to use to
access the store.  For example, Amazon S3 stores are specified with the
prefix ``s3``.

The behavior of an object is governed by properties which are set during
construction.  These properties may specify cloud access credentials,
for example.

The documentation for each plugin specifies the format of the store
specifier and the plugin's properties.  This information is also
available at runtime. TODO xref

To create a functioning store object, begin with a call to
``zcloud_store_new``, which creates the object based on the store
specifier.  Once the object is created, add properties using one of the
property functions described below.  When all of the properties have
been set, call ``zcloud_store_start`` ::

    GError *error = NULL;

    ZCloudStore *store = zcloud_store_new("s3:mybucket", &error);
    if (!store)
        return handle_error(error);

    zcloud_store_set_properties(store,
        "access_key", access_key,
        "secret_key, secret_key,
        "use_ssl", TRUE,
        NULL);

    if (!zcloud_store_start(store, &error))
        return handle_error(error);

Constructor
-----------

.. cfunction:: ZCloudStore *zcloud_store_new(const gchar *storespec, GError **error)

    Create a new :class:`ZCloudStore` object, without any properties
    set.

    :param storespec: the store specifier
    :param error: error information
    :returns: true on success

Setting Properties
------------------

.. cfunction:: zcloud_store_set_property(ZCloudStore *store, const gchar *name, GValue *value)

    Set a property on a :class:`ZCloudStore` object.  In most cases, one
    of the other property-setting methods is easier to use.  This
    function will fail if the property does not exist or if the value is
    of the wrong type.

    :param store: store object
    :param name: property name
    :param value: property value

.. cfunction:: gboolean zcloud_store_set_property_from_string(ZCloudStore *store, const gchar *name, const gchar *value, GError **error)

    Set a property on a :class:`ZCloudStore` object, converting the
    value from a string first.  If the property does not exist, or
    *value* has the wrong format, *error* is set appropriately.  This
    function is intended for applications which take property values
    directly from the user.

    :param store: store object
    :param name: property name
    :param value: property value
    :param error: error information
    :returns: true on success

.. cfunction:: void zcloud_store_set_properties(ZCloudStore *store, name, value[, name, value[ .. ]], NULL)

    Set multiple properties on *store*, using C variadic arguments.
    This uses the GLib *GParamSpec* support to parse values from the
    corresponding C types, and is suitable for applications where the
    property names and values are calculated by the calling application.
    The function will fail with an error if its arguments are invalid.

    Example::
    
        zcloud_store_set_properties(store,
            "verbose", 1,
            "username", "zmanda",
            "password", "z*test",
            NULL);

    :param store: store object
    :param name: property name
    :param value: property value (corresponding C type)

Starting the Object
-------------------

.. cfunction:: gboolean zcloud_store_start(ZCloudStore *store, GError **error)

    Start a :class:`ZCloudStore` object.  Call this when all of the
    properties have been set, to indicate that the store is now
    complete.
