# autoconf sample

AC_PREREQ([2.69])

AC_INIT([exsample], [1.0])

AM_INIT_AUTOMAKE([foreign subdir-objects])

#AC_CONFIG_SRCDIR([src/exsample.cpp])

AC_CONFIG_HEADERS([include/config.h])

AC_CONFIG_MACRO_DIR([m4])


AC_ARG_ENABLE([userdebug],
  [AS_HELP_STRING([--enable-userdebug], [Enable user debuging function (default is no)])],
  [:],
  [enable_userdebug=no])
AM_CONDITIONAL([ENABLE_USERDEBUG], [test "$enable_userdebug" = "yes"])

AC_ARG_ENABLE([printfdebug],
  [AS_HELP_STRING([--enable-printfdebug], [Enable user printf debug (default is no)])],
  [:],
  [enable_printfdebug=no])
AM_CONDITIONAL([ENABLE_PRINTFDEBUG], [test "$enable_printfdebug" = "yes"])

# Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL

AC_DISABLE_STATIC

# Checks for libraries.
LT_INIT
AM_PROG_LIBTOOL 

PKG_PROG_PKG_CONFIG
PKG_CHECK_MODULES([GLIB2], [glib-2.0])
PKG_CHECK_MODULES([GIO2], [gio-2.0])



# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

AC_CONFIG_FILES([Makefile exsample.pc 
				lib/Makefile
				src/Makefile ])
AC_OUTPUT

