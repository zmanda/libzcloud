Store
-----

.. class:: ZCloudStore

   Stores represent a service that provides persistence of arbitrary blobs.
   An example is Amazon S3.

   All of the methods listed below will return true on success and false on failure.
   If they return false, they will set the ``error`` pointer to a GError object
   (assuming one was provided).

   .. method:: create(key, progress, error)

      Creates the key with an empty value.  This is not required by all store
      classes.  See :ref:`uploading_and_downloading`.

      :param key: the key to create
      :type key: String
      :param progress: an optional :class:`ProgressListener`
      :param error: optional storage for an error
      :returns: true (on success) or false (on failure)
      :rtype: boolean

   .. method:: upload(key, producer, progress, error)

      Upload some data.  ``producer`` will be consulted for metadata
      (e.g. MD5 hash and size) and is responsible for actually reading the data
      to be uploaded. The :class:`Store` handles actually uploading the data
      to the storage service.  See :ref:`uploading_and_downloading`.

      :param key: the key to store the data at
      :type key: String
      :param producer: an :class:`UploadProducer`
      :param progress: an optional :class:`ProgressListener`
      :param error: optional storage for an error
      :returns: true (on success) or false (on failure)
      :rtype: boolean

   .. method:: download(key, consumer, progress, error)

      Download data from `key`. The ``consumer`` is responsible for
      "doing something useful" with the data received from the storage service.
      For example, it could save it to a file.  See :ref:`uploading_and_downloading`.

      :param key: the key to fetch data from
      :type key: String
      :param consumer: a :class:`DownloadConsumer`
      :param progress: an optional :class:`ProgressListener`
      :param error: optional storage for an error
      :returns: true (on success) or false (on failure)
      :rtype: boolean

   .. method:: exists(key, progress, error)

      Checks if `key` exists. See :ref:`testing_existence`.

      :param key: the key to check
      :type key: String
      :param progress: an optional :class:`ProgressListener`
      :param error: optional storage for an error
      :returns: true (on success) or false (on failure or non-existence)
      :rtype: boolean

   .. method:: delete(key, progress, error)

      Delete whatever is represented by `key`.  See :ref:`deleting_keys`.

      :param key: the key to delete
      :type key: String
      :param progress: an optional :class:`ProgressListener`
      :param error: optional storage for an error
      :returns: true (on success) or false (on failure)
      :rtype: boolean

   .. method:: list(template, consumer, progress, error)

      List all keys, calling :meth:`ListConsumer.got_result` for each one.
      If a ``template`` is provided, only keys that fit it will be reported.
      The ``template`` should use ``%s`` as a placeholder/wildcard and ``%%``
      to represent ``%``.  See :ref:`listing_keys`.

      :param template: the template to list matches for
      :param consumer: a :class:`DownloadConsumer`
      :param progress: an optional :class:`ProgressListener`
      :param error: optional storage for an error
      :returns: true (on success) or false (on failure)
      :rtype: boolean

   .. method:: setup(suffix, n_parameters, parameters, error)

      Protected method - used during object construction.


