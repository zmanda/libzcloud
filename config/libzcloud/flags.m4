#  -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
#  vi: set tabstop=4 shiftwidth=4 expandtab: */
#  ***** BEGIN LICENSE BLOCK *****
#  Version: LGPL 2.1/GPL 2.0
#  This file is part of libzcloud.
# 
#  libzcloud is free software: you can redistribute it and/or modify
#  it under the terms of the GNU Lesser General Public License (the LGPL)
#  as published by the Free Software Foundation, either version 2.1 of
#  the LGPL, or (at your option) any later version.
# 
#  libzcloud is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU Lesser General Public License for more details.
# 
#  The Original Code is Zmanda Incorporated code.
# 
#  The Initial Developer of the Original Code is
#   Zmanda Incorporated
#  Portions created by the Initial Developer are Copyright (C) 2009
#  the Initial Developer. All Rights Reserved.
# 
#  Contributor(s):
#    Nikolas Coukouma <atrus@zmanda.com>
# 
#  Alternatively, the contents of this file may be used under the terms of
#  the GNU General Public License Version 2 or later (the "GPL"),
#  in which case the provisions of the GPL are applicable instead
#  of those above. If you wish to allow use of your version of this file only
#  under the terms of either the GPL and not to allow others to
#  use your version of this file under the terms of the LGPL, indicate your
#  decision by deleting the provisions above and replace them with the notice
#  and other provisions required by the GPL. If you do not delete
#  the provisions above, a recipient may use your version of this file under
#  the terms of either the the GPL or the LGPL.
# 
#  You should have received a copy of the GNU Lesser General Public License
#  and GNU General Public License along with libzcloud. If not, see
#  <http://www.gnu.org/licenses/>.
# 
#  ***** END LICENSE BLOCK ***** */

# SYNOPSIS
#
#   ZCLOUD_INIT_COMPILER_FLAGS
#
# DESCRIPTION
#
#   Set up extra compiler flags, mostly for GCC.  Sets up ZC_WARNING_CPPFLAGS,
#   which can be added to AM_CPPFLAGS in Makefiles.  Sets up ZC_LDFLAGS, also for
#   inclusion in Makefiles
#
AC_DEFUN([ZCLOUD_INIT_COMPILER_FLAGS], [
    AC_REQUIRE([AC_PROG_GCC_TRADITIONAL])

    # Warn for just about everything
    AX_CFLAGS_GCC_OPTION(-Wall, ZC_WARNING_CPPFLAGS)
    
    AC_SUBST([ZC_WARNING_CPPFLAGS])
    AC_SUBST([ZC_LDFLAGS])
])

# SYNOPSIS
#
#   ZCLOUD_ADD_GCC_WARNING_OPTION(option)
#
# DESCRIPTION
#
#   Adds OPTION to ZC_WARNING_CPPFLAGS if the compiler is gcc and if it accepts
#   the option.
AC_DEFUN([ZCLOUD_ADD_GCC_WARNING_OPTION], [
    AC_REQUIRE([ZCLOUD_INIT_COMPILER_FLAGS])
    AX_CFLAGS_GCC_OPTION($1, ZC_WARNING_CPPFLAGS)
])
