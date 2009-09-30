.. _uploading_and_downloading:

Uploading and Downloading
=========================

Uploading
---------

To assign a value to a particular key in the store, use the :meth:`upload
<ZCloudStore.upload>` method.  The method takes a key name, an
:class:`UploadProducer`, and a :class:`ProgressListener` (which can be NULL if
none is required).   Any upload producer will work, but libzcloud includes a
number of common producers for your convenience -- see :ref:`upload_producers`.

A typical upload of an in-memory buffer looks something like this::

    gboolean
    upload_buffer(ZCloudStore *store, gchar *key,
                  gpointer buf, gsize len, GError **error)
    {
        ZCloudUploadProducer *up_prod;
        gboolean op_ok;

        up_prod = ZCLOUD_UPLOAD_PRODUCER(
            zcloud_memory_upload_producer(buf, len));

        op_ok = zcloud_store_upload(store, key, up_prod, NULL, error);

        g_object_unref(up_prod);

        return op_ok;
     }

Note that the :meth:`create <ZCloudStore.create>` method will create a new,
empty key.  It's not clear how this is useful.

Downloading
-----------

The :meth:`download <ZCloudStore.download>` method fetches the value associated
with a particular key, feeding it to a :class:`DownloadConsumer`.

Code to download into a buffer might look like::

    gboolean
    get_key(ZCloudStore *store, gchar *key,
            gpointer **bufp, gsize *lenp, GError **error)
    {
        ZCloudGrowingMemoryDownloadConsumer *down_con;
        gboolen op_ok;

        down_con = zcloud_growing_memory_download_consumer(MAX_VAL_LENGTH);

        op_ok = zcloud_store_download(store, key,
            ZCLOUD_DOWNLOAD_CONSUMER(down_con), NULL, error);

        if (op_ok) {
            *bufp = zcloud_growing_memory_download_consumer_get_contents(
                            down_con, lenp);
        } else {
            *bufp = NULL;
            *lenp = 0;
        }

        g_object_unref(down_con);

        return op_ok;
    }
