Upload Producer
---------------

.. class:: UploadProducer

   .. method:: read(buffer, bytes, error)

      This method handles reading data from whatever the source actually is,
      storing ``bytes`` number of bytes in ``buffer``. ``buffer`` will be a
      provided by the caller and will be large enough to hold the requested
      number of bytes.

      To indicate an error, zero is returned and ``error`` must be set.

      :param buffer: storage for the read bytes
      :param bytes: the requested number of bytes to read
      :param error: optional storage for an error
      :returns: the number of bytes actually read and copied into ``buffer`` or
            zero on error.

   .. method:: reset(error)

      Reset the producer. After this is finished, the next call to 
      :meth:`read <UploadProducer.read>` should return the same data as the
      first one.

      If an error occurs, ``error`` must be set.

      :param error: optional storage for an error
      :returns: true on success and false on error

   .. method:: get_size(error)

      This method should indicate how many bytes are available from this upload
      producer. The :class:`store <ZCloudStore>` should not try to read more
      bytes than are reported here.

      A special case is the size of zero; if ``error`` is set, then the :class:`store <ZCloudStore>`
      may assume that the number of bytes is simply not known up-front. If ``error``
      is not set, then the store must interpret the zero as the number of bytes
      available

      :param error: storage for an error
      :returns: the number of bytes that this object will produce

   .. method:: calculate_md5(error)

      If null is returned, ``error`` must be set.

      The MD5 hash is represented as a GByteArray with exactly
      ``ZCLOUD_MD5_HASH_BYTE_LEN`` bytes. Note that these are bytes and not
      hexadecimal characters representing bytes.

      :param error: optional storage for an error
      :returns: the MD5 hash of the data that this object will produce, or
                null on error

