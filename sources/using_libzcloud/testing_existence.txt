.. _testing_existence:

Testing for Existence
=====================

To determine for whether or not a key is present within a store, without
downloading its contents, use the :meth:`exists <ZCloudStore.exists>` method.
Note that you need to check the error's domain and code if false is returned.

So, typically code will look something like::

    exists_ok = zcloud_store_exists(store, some_key, NULL, &error)
    if (exists_ok)
       printf("key %s exists\n", some_key)
    else if (ZCLOUD_ERROR == error->domain && ZCERR_MISSING == error->code)
       printf("key %s does not exist\n", some_key);
    else
       printf("an error occurred while seeing if key %s exists: %s\n",
           some_key, error->message);
