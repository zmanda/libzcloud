.. _list_consumers:

List Consumers
==============

A list consumer is an object which processes each key enumerated by the
:class:`ZCloudStore` instance. Any subclass of :class:`ListConsumer` will work,
but libzcloud provides a few common consumers.

Singly-Linked-List List Consumer
--------------------------------

This consumer prepends each key to a singly-linked list.

.. cfunction:: ZCloudSListListConsumer *zcloud_slist_list_consumer(void)

    :returns: a list consumer

.. cfunction:: GSList *zcloud_slist_list_consumer_grab_contents(ZCloudSListListConsumer *self)

    :param self: a list consumer
    :returns: a pointer the the head of the list. Note that the returned list
              will be freed when the list consumer is destroyed.

FD List Consumer
----------------

This consumer simply writes each key, followed by a single character suffix,
to a file descriptor, which the caller is responsible for opening and closing.
The file descriptor *must* be blocking.

.. cfunction:: ZCloudFDListConsumer *zcloud_fd_list_consumer(int fd, gchar suffix)

    :param fd: the file descriptor to use
    :param suffix: the character to write after each key
    :returns: a list consumer
