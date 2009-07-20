# SYNOPSIS
#
#   ZCLOUD_SUMMARY_INIT(text)
#
# DESCRIPTION
#
#   Initialize the summary file; this is AC_REQUIRE'd from ZCLOUD_SUMMARY_ADD,
#   and should not be otherwise used.
AC_DEFUN([ZCLOUD_SUMMARY_INIT], [
    rm -f config.summary
])

# SYNOPSIS
#
#   ZCLOUD_SUMMARY_ADD(text)
#
# DESCRIPTION
#
#   Add TEXT to the summary shown at the end of the zcloud configure run.  TEXT can
#   contain shell variables (it is output using 'cat <<__EOF')
AC_DEFUN([ZCLOUD_SUMMARY_ADD], [
    AC_REQUIRE([ZCLOUD_SUMMARY_INIT])
    cat >>config.summary <<__EOF
$1
__EOF
])

AC_DEFUN([ZCLOUD_SUMMARY], [
    if test -f "config.summary"; then 
        echo "CONFIGURATION SUMMARY:"
        sed 's/^/  /' < config.summary
        rm -f config.summary
    fi
])

