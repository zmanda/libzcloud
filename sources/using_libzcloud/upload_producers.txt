.. _upload_producers:

Upload Producers
================

A upload producer is an object which provides data received for a store to upload.
Any subclass of :class:`UploadProducer` will work with any :class:`ZCloudStore`
instance, but libzcloud provides a few common producers which cover most uses.

Memory Upload Producer
----------------------

This consumer reads a fixed-length, preallocated buffer.

.. cfunction:: ZCloudMemoryUploadProducer *zcloud_memory_upload_procuer(gpointer buffer, gsize buffer_length)

    :param buffer: the buffer to read data from.
    :returns: an upload producer

FD Upload Producer
------------------

This consumer simply reads a file descriptor, which the caller is
responsible for opening and closing. The file descriptor *must* be blocking.
It may or may not be seekable. If it isn't, :meth:`reset <UploadConsumer.reset>`
and :meth:`calculate_md5 <UploadConsumer.calculate_md5>` will return an error;
this implies that uploads can not be retried or the integrity verified.

.. cfunction:: ZCloudFDUploadProducer *zcloud_fd_upload_procuer(int fd)

    :param fd: the file descriptor to use
    :returns: an upload producer
