
AC_DEFUN([GENERATE_OPENCL_SPECIFIC_HEADERS],
[
   mkdir -p "./include-staging/opencl_HEADERS"
   cp ${srcdir}/config/preinclude-opencl.h ./include-staging/opencl_HEADERS
])

AC_DEFUN([ROSE_SUPPORT_OPENCL],
[
  ROSE_CONFIGURE_SECTION([OpenCL Environment])

  ROSE_ARG_WITH(
    [opencl],
    [for an installation of the OpenCL library],
    [support the OpenCL library],
    []
  )
  if test "x$CONFIG_HAS_ROSE_WITH_OPENCL" != "xno"; then
    OPENCL_BIN_PATH="${ROSE_WITH_OPENCL}/bin"
    OPENCL_LIBRARY_PATH="${ROSE_WITH_OPENCL}/lib"
    OPENCL_INCLUDE_PATH="${ROSE_WITH_OPENCL}/include"
  else
    OPENCL_BIN_PATH=
    OPENCL_LIBRARY_PATH=
    OPENCL_INCLUDE_PATH=
  fi

  ROSE_ARG_WITH(
    [opencl-bin],
    [if the OpenCL runtime bin directory was specified],
    [use this OpenCL bin directory],
    []
  )
  if test "x$CONFIG_HAS_ROSE_WITH_OPENCL_BIN" != "xno"; then
      OPENCL_BIN_PATH="$ROSE_WITH_OPENCL_BIN"
  fi

  ROSE_ARG_WITH(
    [opencl-lib],
    [if the OpenCL runtime library directory was specified],
    [use this OpenCL runtime library directory],
    []
  )
  if test "x$CONFIG_HAS_ROSE_WITH_OPENCL_LIB" != "xno"; then
      OPENCL_LIBRARY_PATH="$ROSE_WITH_OPENCL_LIB"
  fi

  ROSE_ARG_WITH(
    [opencl-include],
    [if the OpenCL runtime header directory was specified],
    [use this OpenCL runtime header directory],
    []
  )
  if test "x$CONFIG_HAS_ROSE_WITH_OPENCL_INCLUDE" != "xno"; then
      OPENCL_LIBRARY_PATH="$ROSE_WITH_OPENCL_INCLUDE"
  fi

echo "OPENCL_BIN_PATH     = "$OPENCL_BIN_PATH
echo "OPENCL_LIBRARY_PATH = "$OPENCL_LIBRARY_PATH
echo "OPENCL_INCLUDE_PATH = "$OPENCL_INCLUDE_PATH

  # TODO check directories

  #============================================================================
  # Set Automake Conditionals and Substitutions
  #============================================================================
  AM_CONDITIONAL(ROSE_WITH_OPENCL,         [test "x$OPENCL_BIN_PATH" != "x" && test "x$OPENCL_LIBRARY_PATH" != "x" && test "x$OPENCL_INCLUDE_PATH" != "x"])
  AM_CONDITIONAL(ROSE_WITH_OPENCL_BIN,     [test "x$OPENCL_BIN_PATH" != "x"])
  AM_CONDITIONAL(ROSE_WITH_OPENCL_LIB,     [test "x$OPENCL_LIBRARY_PATH" != "x"])
  AM_CONDITIONAL(ROSE_WITH_OPENCL_INCLUDE, [test "x$OPENCL_INCLUDE_PATH" != "x"])

  AC_SUBST(OPENCL_BIN_PATH)
  AC_SUBST(OPENCL_LIBRARY_PATH)
  AC_SUBST(OPENCL_INCLUDE_PATH)

  #============================================================================
  # Set CPP #defines
  #============================================================================
  AC_DEFINE_UNQUOTED(
    OPENCL_BIN_PATH,
    ["$OPENCL_BIN_PATH"],
    [Absolute path of the OPENCL installation binary directory])
  AC_DEFINE_UNQUOTED(
    OPENCL_LIBRARY_PATH,
    ["$OPENCL_LIBRARY_PATH"],
    [Absolute path of the OPENCL installation library directory])
  AC_DEFINE_UNQUOTED(
    OPENCL_INCLUDE_PATH,
    ["$OPENCL_INCLUDE_PATH"],
    [Absolute path of the OPENCL installation include directory])

if test "x$OPENCL_BIN_PATH" != "x" && test "x$OPENCL_LIBRARY_PATH" != "x" && test "x$OPENCL_INCLUDE_PATH" != "x"; then
   AC_DEFINE([USE_ROSE_OPENCL_SUPPORT], [1], [Controls use of ROSE support for OpenCL.])
fi

])

