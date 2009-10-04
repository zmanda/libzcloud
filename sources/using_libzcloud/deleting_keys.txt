.. _deleting_keys:

Deleting Keys
=============

To delete a key from a store, use the store's :meth:`delete
<ZCloudStore.delete>` method.  This method will remove the key and associated
value from the store.  Like most operations, it takes a progress listener
argument, but in most cases this is a very quick operation.

Example::

    delete_ok = zcloud_store_delete(store, old_key, NULL, &error);
    if (!delete_ok) {
        die("could not delete key '%s'" % old_key);
    }
