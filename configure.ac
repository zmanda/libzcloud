AC_INIT
AC_CONFIG_AUX_DIR(config)
AC_CONFIG_MACRO_DIR(config)
AC_CONFIG_SRCDIR([lib/store.h])
AC_CONFIG_HEADER([config/config.h])
AM_INIT_AUTOMAKE(libzcloud, "1.0alpha")
AC_PREREQ(2.59)


# Checks for programs.
AC_PROG_CC

# Checks for libraries.

# Checks for header files.
AC_HEADER_DIRENT
AC_HEADER_STDC
AC_CHECK_HEADERS([stdlib.h string.h strings.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_SIZE_T
AC_STRUCT_TM

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([bzero regcomp])

AC_CONFIG_FILES([
    Makefile
])

AC_CONFIG_SUBDIRS([
])

AC_OUTPUT
