Upload Producer
---------------

.. class:: UploadProducer

   .. method:: read(buffer, bytes, error)

      :param buffer: storage for the read bytes
             (large enough to hold the requested number of bytes)
      :param bytes: the requested number of bytes to read
      :param error: optional storage for an error
      :returns: the number of bytes actually read and copied into ``buffer``

   .. method:: reset(error)

      Reset the producer. The next call to :meth:`read <UploadProducer.read>` should return the
      same data as the first one.

      :param error: optional storage for an error
      :returns: true on success and false on error

   .. method:: get_size(error)

      :param error: storage for an error
      :returns: the number of bytes that this object will produce

   .. method:: calculate_md5(error)

      :param error: optional storage for an error
      :returns: the MD5 hash of the data that this object will produce, or
                null on error

