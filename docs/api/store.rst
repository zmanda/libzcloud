Store
-----

.. class:: ZCloudStore

   Stores represent a service that provides persistence of arbitrary blobs.
   An example is Amazon S3.

   .. method:: setup(suffix, n_parameters, parameters, error)

      Protected method - used during object construction.

   .. method:: create(address, progress, error)

      Creates whatever is represented by the address.

      :param address: the address to create
      :type address: String
      :param progress: an optional :class:`ProgressListener`
      :param error: optional storage for an error
      :returns: true (on success) or false (on failure)
      :rtype: boolean

   .. method:: upload(address, producer, progress, error)

      Upload some data.  ``producer`` will be consulted for metadata
      (e.g. MD5 hash and size) and is responsible for actually reading the data
      to be uploaded. The :class:`Store` handles actually uploading the data
      to the storage service.

      :param address: the address to store the data at
      :type address: String
      :param producer: an :class:`UploadProducer`
      :param progress: an optional :class:`ProgressListener`
      :param error: optional storage for an error
      :returns: true (on success) or false (on failure)
      :rtype: boolean

   .. method:: download(address, consumer, progress, error)

      Download data from ``address``. The ``consumer`` is responsible for
      "doing something useful" with the data received from the storage service.
      For example, it could save it to a file.

      :param address: the address to fetch data from
      :type address: String
      :param consumer: a :class:`DownloadConsumer`
      :param progress: an optional :class:`ProgressListener`
      :param error: optional storage for an error
      :returns: true (on success) or false (on failure)
      :rtype: boolean

   .. method:: exists(address, progress, error)

      Checks if the given ``address`` exists

      :param address: the address to check
      :type address: String
      :param progress: an optional :class:`ProgressListener`
      :param error: optional storage for an error
      :returns: true (on success) or false (on failure)
      :rtype: boolean

   .. method:: delete(address, progress, error)

      Delete whatever is represented by ``address``

      :param address: the address to delete
      :type address: String
      :param progress: an optional :class:`ProgressListener`
      :param error: optional storage for an error
      :returns: true (on success) or false (on failure)
      :rtype: boolean

   .. method:: list(template, consumer, progress, error)

      List all addresses matching ``template``, calling
      List all :class:`addresses <Address>` matching ``template``, calling
      :meth:`ListConsumer.got_result` for each one. The ``template`` should
      just use ``%s`` as a placeholder and ``%%`` to represent ``%``.

      :param template: the template to list matches for
      :param consumer: a :class:`DownloadConsumer`
      :param progress: an optional :class:`ProgressListener`
      :param error: optional storage for an error
      :returns: true (on success) or false (on failure)
      :rtype: boolean


