.. _creating_a_store:

Creating a Store
================

.. index:: store specifier

A store object is described by a *store specifier*.  Each store
specifier begins with a *prefix*, which identifies the plugin to use to
access the store.  For example, Amazon S3 stores are specified with the
prefix ``s3``.

The behavior of an object is also governed by parameters which are given
during construction.  These parameters may specify cloud access
credentials, for example.

The documentation for each plugin (see :ref:`included_plugins`) specifies the
format of the store specifier and the plugin's parameters.  This information is
also available at runtime - see :ref:`module_info`.

Calling the Constructor
-----------------------

To create a store object, call :cfunc:`zcloud_store_new`:

.. cfunction:: ZCloudStore *zcloud_store_new(const gchar *storespec, GError **error, const gchar *first_param_name, ...)

    Create a new :class:`ZCloudStore` object with the parameters
    specified by the NULL-terminated arguments.  Parameters are
    specified with a parameter name and value::

        store = zcloud_store_new(spec, &error,
            "verbose", TRUE,
            "password", "s3kr1t",
            "concurrency", 7,
            NULL);

    :param storespec: the store specifier
    :param error: error information
    :param first_param_name: beginning of variadic arguments
    :returns: a store object, or NULL on error

Specifying Parameters at Runtime
--------------------------------

A program calling :cfunc:`zcloud_store_new` must hard-code the parameter
names, and must convert any parameter values to the appropriate type.
While this is convenient for applications designed to use a particular
plugin, it is not appropriate to more general applications that wish to
support arbitrary plugins.

Such applications should use :cfunc:`zcloud_store_newv`, instead:

.. cfunction:: ZCloudStore *zcloud_store_newv(const gchar *storespec, gint n_parameters, GParameter *parameters, GError **error)

    Create a new :class:`ZCloudStore` object with the parameters
    specified in *parameters*.

    :param storespec: the store specifier
    :param n_parameters: number of elements in *parameters*
    :param parameters: array of GParameter objects
    :param error: error information
    :returns: a store object, or NULL on error

The ``GParameter`` structs passed to this function should contain all of
the parameters specified for this store.  The ``name`` members will be
canonicalized (case lowered, non-alphanumeric characters replaced with
'-'), and the ``value`` members will be transformed to the types
required by the store plugin.

Note that GLib does not have a facility to transform strings into other
types.  An application supplying parameters to
:cfunc:`zcloud_store_newv` from a configuration file or command-line
arguments will need to handle this conversion itself.  (actually,
eventually libzcloud will need to handle this, so some support is
forthcoming - TODO)
