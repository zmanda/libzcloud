Store Subclass
==============

Each plugin is linked to a particular subclass of :class:`ZCloudStore`,
indicated by the type registered during :doc:`module initialization
<writing_plugins/initialization>`.

Store Initialization
--------------------

When a user invokes :cfunc:`zcloud_store_new` or
:cfunc:`zcloud_store_newv`, the new store object is initialized with
this process:

* Examine the store specifier, splitting at the colon into a *prefix*
  and *suffix*

* Load the module containing he plugin for the prefix, if the modules
  has not already been loaded.

* Instantiate the type registered for the prefix, using ``g_object_new``,
  but without passing any GLib parameters.

* Invoke the :meth:`~ZCloudStore.setup` method with a normalized set of
  parameters.

Parameters are passed to setup in an array of ``GParameter`` structs,
each of which has members ``name`` and ``value``.  All parameters
specified in the :ref:`XML file <xml-definition>` are present, with no
duplicates, but they are not given in any particular order.  The
``name`` member is normalized, as described in the  so it is safe to use
``strcmp``.  The ``value`` member has the type specified in the
``parameter`` XML element.
