.. _listing_keys:

Listing Keys
============

To list the keys in a store, use the store's :meth:`list <ZCloudStore.list>`
method.

This method takes a "key template", which some plugins can use to minimize the
data transferred from the cloud.  In the template, `%s` will match any string.
All other characters following a `%` character are reserved.  A literal `%`
character can be escaped as `%%`.  For example:

* `"My Files/Mostly.doc"` matches only a file with the given name.
* `"My Files/%s"` matches any key beginning with `My Files/`, enabling a sort
  of directory structure.
* `"My Files/100%%.doc"` matches only `My Files/100%.doc`.

The template system is not yet finalized.

The `list` method also takes a :class:`ListConsumer` instance to handle each
returned key - see :ref:`list_consumers` for a list of ready-made
:class:`ListConsumer` subclasses.

A typical use of the `list` method might look like::

    gint
    count_keys(ZCloudStore *store, gchar *template)
    {
        ZCloudSListListConsumer *list_con;
        GSList *l;
        gint count = -1;

        /* (another list consumer subclass might be more efficient..) */
        list_con = zcloud_slist_list_consumer();

        op_ok = zcloud_store_list(store, template,
                ZCLOUD_LIST_CONSUMER(list_con), NULL, &error);
        if (op_ok) {
            l = zcloud_slist_list_consumer_grab_contents(list_con);
            count = g_slist_length(l);
        }

        g_object_unref(list_con);
        return count;
    }
