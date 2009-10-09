Download Consumer
-----------------

.. class:: DownloadConsumer

   .. method:: write(buffer, bytes, error)

      :param buffer: storage containing the bytes that shuld be consumed
      :param bytes: the number of bytes available for consumption
      :param error: optional storage for an error
      :returns: the number of bytes actually read from ``buffer``

   .. method:: reset(error)

      Reset the consumer. All previous calls to :meth:`write <DownloadConsumer.write>` should be "forgotten".

      :param error: optional storage for an error
      :returns: true on success and false on error


