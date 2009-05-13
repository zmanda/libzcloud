# SYNOPSIS
#
#   ZCLOUD_CHECK_GLIB
#
# OVERVIEW
#
#   Search for glib.  This is basically a wrapper for AM_PATH_GLIB_2_0, with
#   the addition of system-specific configuration to convince Amanda to compile
#   "out of the box" on more boxes.
#
AC_DEFUN([ZCLOUD_CHECK_GLIB], [
    # search for pkg-config, which the glib configuration uses, adding a few
    # system-specific search paths.
    AC_PATH_PROG(PKG_CONFIG, pkg-config, [], /opt/csw/bin:/usr/local/bin:/opt/local/bin)

    case "$target" in
	sparc-sun-solaris2.8) # Solaris 8
	    # give the linker a runtime search path; pkg-config doesn't supply this.
	    # Users could also specify this with LD_LIBRARY_PATH to both ./configure
	    # and make.  Adding this support here makes straight './configure; make'
	    # "just work" on Solaris 8
	    if test -n "$PKG_CONFIG"; then
            glib_R_flag=`$PKG_CONFIG glib-2.0 --libs-only-L 2>/dev/null | sed -e 's/-L/-R/g'`
            LDFLAGS="$LDFLAGS $glib_R_flag"
	    fi
	    ;;
    esac

    AM_PATH_GLIB_2_0(2.2.0, , [ AC_MSG_ERROR(glib not found or too old) ], gmodule gobject gthread)
])
