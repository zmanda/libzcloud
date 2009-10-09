.. _error_handling:

Error Handling
==============

Errors are reported using GLib's GError structs. The errors from libzcloud
should use ``ZCLOUD_ERROR`` as their domain and a code from ``ZCloudError``.

The currently defined ``ZCloudError`` codes are:

ZCERR_NONE
        No Error
ZCERR_UNKNOWN
        An unknown error occurred .
ZCERR_MODULE
        Error loading a module
ZCERR_PLUGIN
        Error from a plugin, if nothing else is more suitable
ZCERR_PARAMETER
        An invalid parameter was given to the store or method.
ZCERR_EPHEMERAL
        An ephemeral network or service error. For example, if a service is busy
        then requests might time out; just retrying at a later time is likely to
        succeed.
ZCERR_LASTING
        Lasting network or service error. These usually require some action to
        be taken for them to go away. For example, if the authentication
        credentials for the store are incorrect.
ZCERR_MISSING
        The requested object (or perhaps its container) is missing. Callers may
        choose to handle this case differently from other lasting errors.

Other codes may be added in the future.
