.. _download_consumers:

Download Consumers
==================

A download consumer is an object which processes data received from a store.
Any subclass of :class:`DownloadConsumer` will work with any
:class:`ZCloudStore` instance, but libzcloud provides a few common consumers
which cover most uses.

Fixed Memory Download Consumer
------------------------------

This consumer reads data into a fixed-length, preallocated buffer.  This is
useful for small values that have a known maximum length.  If the store
provides more data than permitted, then the consumer will signal an error
condition and the store will abort the download operation.

The buffer is supplied to the constructor:

.. cfunction:: ZCloudFixedMemoryDownloadConsumer *zcloud_fixed_memory_download_consumer(gpointer buffer, gsize buffer_length)

    :param buffer: the buffer into which to read data
    :param buffer_length: the buffer size
    :returns: a download consumer

When the download is complete, the size of the data and a pointer to the original buffer are available from the ``get_contents`` method:

.. cfunction:: gpointer zcloud_fixed_memory_download_consumer_get_contents(ZCloudFixedMemoryDownloadConsumer *self, gsize *length)

    :param self: download consumer
    :param length: (output) size of the data in the buffer
    :returns: pointer to the downloaded data (same as the constructor's ``buffer`` argument)

Growing Memory Download Consumer
--------------------------------

For values with more variability in size, the growing memory download consumer
can avoid the need to allocate large amounts of memory unless the value is
actually large.  This consumer will periodically resize its buffer to
accomodate incoming data, up to the maximum buffer size specified to the
constructor:

.. cfunction:: ZCloudGrowingMemoryDownloadConsumer *zcloud_growing_memory_download_consumer(gsize max_buffer_length)

    :param max_buffer_length: maximum amount of data to accept, or 0 for no limit
    :returns: a download consumer

Like the fixed memory download consumer, this consumer has a ``get_contents``
method.  However, this method returns a *newly allocated* buffer, which the
caller must free.

.. cfunction:: gpointer zcloud_growing_memory_download_consumer_get_contents(ZCloudGrowingMemoryDownloadConsumer *self, gsize *length)

    :param self: download consumer
    :param length: (output) length of the returned data
    :returns: newly allocated buffer containing the downloaded data

.. cfunction:: gchar *zcloud_growing_memory_download_consumer_get_contents_as_string(ZCloudGrowingMemoryDownloadConsumer *self)

    Like ``get_contents``, but returns the buffer as a string.

    :param self: download consumer
    :returns: newly allocated, NUL-terminated string containing the downloaded data

FD Download Consumer
--------------------

This consumer simply writes to a file descriptor, which the caller is
responsible for opening and closing. The file descriptor *must* be blocking.
It may or may not be seekable. If it isn't, :meth:`reset <DownloadConsumer.reset>`
will return an error; this implies that downloads can not be retried.

.. cfunction:: ZCloudFDDownloadConsumer *zcloud_fd_download_consumer(int fd)

    :param fd: the file descriptor to use
    :returns: a download consumer
