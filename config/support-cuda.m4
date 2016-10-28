
AC_DEFUN([GENERATE_CUDA_SPECIFIC_HEADERS],
[
   mkdir -p "./include-staging/cuda_HEADERS"
   cp ${srcdir}/config/preinclude-cuda.h ./include-staging/cuda_HEADERS
])

AC_DEFUN([ROSE_SUPPORT_CUDA],
[
  ROSE_CONFIGURE_SECTION([CUDA Environment])

  ROSE_ARG_WITH(
    [cuda],
    [for an installation of the CUDA library],
    [support the CUDA library],
    []
  )
  if test "x$CONFIG_HAS_ROSE_WITH_CUDA" != "xno"; then
    CUDA_BIN_PATH="${ROSE_WITH_CUDA}/bin"
    CUDA_LIBRARY_PATH="${ROSE_WITH_CUDA}/lib"
    CUDA_INCLUDE_PATH="${ROSE_WITH_CUDA}/include"
  else
    CUDA_BIN_PATH=
    CUDA_LIBRARY_PATH=
    CUDA_INCLUDE_PATH=
  fi

  ROSE_ARG_WITH(
    [cuda-bin],
    [if the CUDA runtime bin directory was specified],
    [use this CUDA bin directory],
    []
  )
  if test "x$CONFIG_HAS_ROSE_WITH_CUDA_BIN" != "xno"; then
      CUDA_BIN_PATH="$ROSE_WITH_CUDA_BIN"
  fi

  ROSE_ARG_WITH(
    [cuda-lib],
    [if the CUDA runtime library directory was specified],
    [use this CUDA runtime library directory],
    []
  )
  if test "x$CONFIG_HAS_ROSE_WITH_CUDA_LIB" != "xno"; then
      CUDA_LIBRARY_PATH="$ROSE_WITH_CUDA_LIB"
  fi

  ROSE_ARG_WITH(
    [cuda-include],
    [if the CUDA runtime header directory was specified],
    [use this CUDA runtime header directory],
    []
  )
  if test "x$CONFIG_HAS_ROSE_WITH_CUDA_INCLUDE" != "xno"; then
      CUDA_LIBRARY_PATH="$ROSE_WITH_CUDA_INCLUDE"
  fi

echo "CUDA_BIN_PATH     = "$CUDA_BIN_PATH
echo "CUDA_LIBRARY_PATH = "$CUDA_LIBRARY_PATH
echo "CUDA_INCLUDE_PATH = "$CUDA_INCLUDE_PATH

  # TODO check directories

  #============================================================================
  # Set Automake Conditionals and Substitutions
  #============================================================================
  AM_CONDITIONAL(USE_ROSE_CUDA_SUPPORT,  [test "x$CUDA_BIN_PATH" != "x" && test "x$CUDA_LIBRARY_PATH" != "x" && test "x$CUDA_INCLUDE_PATH" != "x"])
  AM_CONDITIONAL(ROSE_WITH_CUDA_BIN,     [test "x$CUDA_BIN_PATH" != "x"])
  AM_CONDITIONAL(ROSE_WITH_CUDA_LIB,     [test "x$CUDA_LIBRARY_PATH" != "x"])
  AM_CONDITIONAL(ROSE_WITH_CUDA_INCLUDE, [test "x$CUDA_INCLUDE_PATH" != "x"])

  AC_SUBST(CUDA_BIN_PATH)
  AC_SUBST(CUDA_LIBRARY_PATH)
  AC_SUBST(CUDA_INCLUDE_PATH)

  #============================================================================
  # Set CPP #defines
  #============================================================================
  AC_DEFINE_UNQUOTED(
    CUDA_BIN_PATH,
    ["$CUDA_BIN_PATH"],
    [Absolute path of the CUDA installation binary directory])
  AC_DEFINE_UNQUOTED(
    CUDA_LIBRARY_PATH,
    ["$CUDA_LIBRARY_PATH"],
    [Absolute path of the CUDA installation library directory])
  AC_DEFINE_UNQUOTED(
    CUDA_INCLUDE_PATH,
    ["$CUDA_INCLUDE_PATH"],
    [Absolute path of the CUDA installation include directory])
])

