Introduction
************

Libzcloud is an abstract interface to cloud-based key/value stores.  It
supports the basic operations: list keys, set a value (upload), get a value
(download), and delete a value.

The library provides an object called a "store," with the various operations
exposed as methods on that object.  The store is specified using a string that
gives the cloud provider and any other information required to find the data.
libzcloud will automatically find and load the appropriate plugin for the prefix;
if you're interested in adding support for a cloud storage service, read
:ref:`writing_plugins`.

Most of the store operations take a key as an index into the key/value store.
The key is generally a printable string.  Values are represented as arbitrary
byte streams.

Since values are often very large, libzcloud operates on streams rather than
pre-allocated strings.   Upload operations pull data from a "producer", and
download operations push data to a "consumer."  The library includes some
common producer and consumer implementations, but more sophisticated
applications can easily construct their own implementations.

Some cloud vendors provide sophisticated key-listing methods to filter the keys
returned.  Libzcloud abstracts those methods using "key templates" which
contain wildcards and other special characters to indicate which keys are of
interest.  Store implementations map these templates to the most efficient
operations available from the cloud vendor.
