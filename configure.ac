AC_PREREQ([2.69])
AC_INIT([xtpath], [0.1],
        [MBendiksen@vizrt.com],
        [xtpath],
        [https://github.com/mortalisk/xtpath])

AC_CONFIG_MACRO_DIR([config])
AC_CONFIG_AUX_DIR([config])

AM_INIT_AUTOMAKE([1.13.4 foreign
                  dist-xz no-dist-gzip
                  color-tests parallel-tests
                  -W -Wall])
AM_SILENT_RULES([yes])

AC_LANG_CPLUSPLUS
AC_PROG_CXX
AC_PROG_LIBTOOL

AC_ARG_ENABLE([cxx11],
              AS_HELP_STRING([--disable-cxx11],
                             [Disable automatic set of -std=c++11 flag]),
              [],
              [enable_cxx11=yes])

if test x"${enable_cxx11}" = xyes; then
  AC_MSG_CHECKING([whether the C++ compiler accepts -std=c++11.])
  oldCPPFLAGS="$CPPFLAGS"
  CPPFLAGS="-std=c++0x $CPPFLAGS"
  AC_COMPILE_IFELSE([AC_LANG_SOURCE([[]])],
                    [EXTRACPPFLAGS="-std=c++11"
                     AC_MSG_RESULT([yes])],
                    [EXTRACPPFLAGS=""
                    AC_MSG_RESULT([no])])
  CPPFLAGS="$EXTRACPPFLAGS $oldCPPFLAGS"
fi

BOOST_REQUIRE
BOOST_TEST

AC_ARG_WITH([pugixml],
            AS_HELP_STRING([--with-pugixml=PATH],
                           [prefix of pugixml]),
            [], [])
AC_ARG_WITH([pugixml-headers],
            AS_HELP_STRING([--with-pugixml-headers=PATH],
                           [include path of pugixml]),
            [PUGIXML_CPPFLAGS=-I$with_pugixml_headers],
            [if test "${with_pugiml+set}" = set; then
               PUGIXML_CPPFLAGS=-I$with_pugixml/include
             fi])
AC_ARG_WITH([pugixml-ldpath],
            AS_HELP_STRING([--with-pugixml-ldpath=PATH],
                           [libpath of pugixml]),
            [PUGIXML_LDFLAGS=-L$with_pugixml_ldpath],
            [if test "${with_pugiml+set}" = set; then
               PUGIXML_LDFLAGS=-L$with_pugixml/lib
             fi])
AC_ARG_WITH([pugixml-lib],
            AS_HELP_STRING([--with-pugixml-lib=NAME],
                           [library name of pugixml]),
            [PUGIXML_LIBS=-l$with_pugixml_lib],
            [PUGIXML_LIBS=-lpugixml])

oldLDPATH="$LDPATH"
oldLIBS="$LIBS"
oldCPPFLAGS="$CPPFLAGS"
LDPATH="$LDPATH $PUGIXML_LDPATH"
LIBS="$LIBS $PUGIXML_LIBS"
CPPFLAGS="$CPPFLAGS $PUGIXML_CPPFLAGS"

AC_CHECK_HEADER([pugixml/pugixml.hpp])
AC_CHECK_LIB([pugixml], [main])
AC_SUBST([PUGIXML_LDFLAGS])
AC_SUBST([PUGIXML_LIBS])
AC_SUBST([PUGIXML_CPPFLAGS])

LDPATH="$oldLDPATH"
LIBS="$oldLIBS"
CPPFLAGS="$oldCPPFLAGS"




AC_CONFIG_FILES([Makefile
                 test/Makefile])
AC_OUTPUT
