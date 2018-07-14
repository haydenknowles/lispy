
AC_PREREQ([2.69])
AC_INIT([lispy], [0.0.0.0.1], [hayden.knowles@live.com])
AM_INIT_AUTOMAKE([foreign -Wall])
AM_SILENT_RULES([yes])
AC_CONFIG_SRCDIR([src/lispy.c])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC_C99

# Checks for libraries.
AC_SEARCH_LIBS([readline], [edit],, AC_MSG_ERROR["could not find function readline()"])
AC_SEARCH_LIBS([add_history], [edit],, AC_MSG_ERROR["could not find function add_history()"])

# Checks for header files.
AC_CHECK_HEADERS([editline/readline.h mpc.h math.h])

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT
