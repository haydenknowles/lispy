#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([lispy], [0.0.0.0.1], [hayden.knowles@live.com])
AM_INIT_AUTOMAKE([foreign -Wall])
AM_SILENT_RULES([yes])
AC_CONFIG_SRCDIR([prompt.c])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC_C99

# Checks for libraries.
AC_SEARCH_LIBS([readline], [edit])
AC_SEARCH_LIBS([add_history], [edit])

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h readline.h])

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
