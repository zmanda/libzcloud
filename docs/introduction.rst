.. _introduction:

************
Introduction
************

This is an initial draft of the libzcloud interfaces, which aim to provide an
abstraction for the various cloud storage services.

The core of libzcloud's abstraction is the "store". The methods that the
store must support are:

* create
* upload
* download
* delete
* exists
* list

Most of the operations above take an "address" to operate on. Addresses are
specific to an individual store (e.g bucket name and key for Amazon S3) and
are represented by a simple string, which parse_address should interpret.

Address templates (used when listing addresses) are simply a sprintf-style
pattern (which only accepts strings, "%s", and the escape sequence, "%%").
They allow programs using libzcloud to specicfy simple patterns and still have
the store (probably) be able to list everything matching that pattern. Only
permitting strings in the pattern is only meant to simplify the parsing of the
pattern by the store; programs using libzcloud might only substitute, say,
a number.

Independent of particular stores are the download consumer, upload producer,
progress listener, and list consumer. They are the concern of programs using libzcloud.

libzcloud will eventually include some implementations of its abstractions.
An implementation of an Amazon S3 store is expected along with consumers
(and producers) that write to (and read from) memory buffers and files.
