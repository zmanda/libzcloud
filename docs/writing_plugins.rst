Writing libzcloud Plugins
=========================

This section gives an "insider" view of writing a libzcloud plugin.
When beginning a new plugin, look at the existing libzcloud plugins for
examples.

.. note::
   
   Libzcloud is designed to allow plugins compiled apart from the libzcloud
   source tree, but this support is not yet complete.  This section describes
   writing new plugins *within* the libzcloud source tree (under ``plugins/``).

.. toctree::

   writing_plugins/xml_definition
   writing_plugins/initialization
   writing_plugins/store_subclass
