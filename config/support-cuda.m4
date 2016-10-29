
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
    CUDA_PATH="${ROSE_WITH_CUDA}"
  else
    CUDA_PATH=
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

  echo "From configuration options:"
  echo "  CUDA_PATH         = "$CUDA_PATH
  echo "  CUDA_BIN_PATH     = "$CUDA_BIN_PATH
  echo "  CUDA_LIBRARY_PATH = "$CUDA_LIBRARY_PATH
  echo "  CUDA_INCLUDE_PATH = "$CUDA_INCLUDE_PATH

  USE_ROSE_CUDA_SUPPORT=1

  #============================================================================
  # Set binary, library, and include path
  #============================================================================
  if test "x$CUDA_BIN_PATH" == "x"; then
    if test "x$CUDA_PATH" != "x"; then
      CUDA_BIN_PATH="$CUDA_PATH/bin"
    elif which nvcc > /dev/null 2> /dev/null; then
      CUDA_BIN_PATH="`dirname `which nvcc``"
      CUDA_PATH="`dirname $CUDA_BIN_PATH`"
    elif test -d "/usr/local/cuda/bin"; then
      CUDA_BIN_PATH="/usr/local/cuda/bin"
      CUDA_PATH="`dirname $CUDA_BIN_PATH`"
    else
      USE_ROSE_CUDA_SUPPORT=0
    fi
  fi
  if test "x$CUDA_LIBRARY_PATH" == "x"; then
    if test "x$CUDA_PATH" != "x"; then
      if test -d "$CUDA_PATH/lib64"; then
        CUDA_LIBRARY_PATH="$CUDA_PATH/lib64"
      else
        CUDA_LIBRARY_PATH="$CUDA_PATH/lib"
      fi
    else
      USE_ROSE_CUDA_SUPPORT=0
    fi
  fi
  if test "x$CUDA_INCLUDE_PATH" == "x"; then
    if test "x$CUDA_PATH" != "x"; then
      CUDA_INCLUDE_PATH="$CUDA_PATH/include"
    else
      USE_ROSE_CUDA_SUPPORT=0
    fi
  fi

  echo "After completion:"
  echo "  CUDA_BIN_PATH         = "$CUDA_BIN_PATH
  echo "  CUDA_LIBRARY_PATH     = "$CUDA_LIBRARY_PATH
  echo "  CUDA_INCLUDE_PATH     = "$CUDA_INCLUDE_PATH
  echo "  USE_ROSE_CUDA_SUPPORT = "$USE_ROSE_CUDA_SUPPORT

  #============================================================================
  # Check binary, library, and include path
  #============================================================================
  if test "x$CUDA_BIN_PATH" != "x"; then
    if test ! -x "$CUDA_BIN_PATH/nvcc"; then
      AC_MSG_ERROR([Invalid directory for CUDA binaries: $CUDA_BIN_PATH])
      CUDA_BIN_PATH=
      USE_ROSE_CUDA_SUPPORT=0
    fi
  fi
  if test "x$CUDA_LIBRARY_PATH" != "x"; then
    if test ! -e "$CUDA_LIBRARY_PATH/libcudart.so"; then
      AC_MSG_ERROR([Invalid directory for CUDA libraries: $CUDA_LIBRARY_PATH])
      CUDA_LIBRARY_PATH=
      USE_ROSE_CUDA_SUPPORT=0
    fi
  fi
  if test "x$CUDA_INCLUDE_PATH" != "x"; then
    if test ! -e "$CUDA_INCLUDE_PATH/cuda.h"; then
      AC_MSG_ERROR([Invalid directory for CUDA includes: $CUDA_INCLUDE_PATH])
      CUDA_INCLUDE_PATH=
      USE_ROSE_CUDA_SUPPORT=0
    fi
  fi

  echo "After verification:"
  echo "  CUDA_BIN_PATH         = "$CUDA_BIN_PATH
  echo "  CUDA_LIBRARY_PATH     = "$CUDA_LIBRARY_PATH
  echo "  CUDA_INCLUDE_PATH     = "$CUDA_INCLUDE_PATH
  echo "  USE_ROSE_CUDA_SUPPORT = "$USE_ROSE_CUDA_SUPPORT

  #============================================================================
  # Set Automake Conditionals and Substitutions
  #============================================================================
  AM_CONDITIONAL(USE_ROSE_CUDA_SUPPORT,  [$USE_ROSE_CUDA_SUPPORT])

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

