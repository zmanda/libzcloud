Introduction
============

Resources
---------

When beginning a new plugin, look at the existing libzcloud plugins for
examples.  In particular, the *disk* plugin is intended as an example.

At the moment, there is no libzcloud mailing list, but you are encouraged
contact the developers directly by email or visit the IRC channel
``#libzcloud`` on Freenode with any questions.

.. index:: local plugin, external plugin, plugins; local, plugins; external

Local and External Plugins
--------------------------

A *local* plugin is one which is built alongside libzcloud.  *External* plugins
are compiled separately, against an installed copy of libzcloud.  Both
varieties of plugins are loaded with the same mechanism; the difference is only
in how they are compiled and installed.

.. note::
   
   Support for external plugins is not yet complete.  Libzcloud will eventually
   provide autoconf macros and other support to make development of external
   plugins very easy.
