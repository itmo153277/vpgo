AC_DEFUN([VPGO_INIT_THREADS], [
    m4_define([VPGO_CPP_THREADS_BODY], [[
        #include <thread>

        int main() {
            std::thread t([]() {
                // Do nothing;
            });
            t.join();
            return 0;
        }
    ]])
    can_use_threads="no"
    AC_MSG_CHECKING([whether can use threads with no flags])
    AC_LINK_IFELSE([AC_LANG_SOURCE([VPGO_CPP_THREADS_BODY])], [can_use_threads="yes"])
    AC_MSG_RESULT([$can_use_threads])
    if test "x$can_use_threads" = "xno" ; then
        has_pthreads="no"
        AX_PTHREAD([has_pthreads="yes"])
        if test "x$has_pthreads" = "xyes" ; then
            AC_MSG_CHECKING([whether can use threads with pthreads])
            LIBS_SAVE=$LIBS
            CXXFLAGS_SAVE=$CXXFLAGS
            LIBS="$LIBS $PTHREAD_LIBS"
            CXXFLAGS="$CXXFLAGS $PTHREAD_CFLAGS"
            AC_LINK_IFELSE([AC_LANG_SOURCE([VPGO_CPP_THREADS_BODY])], [can_use_threads="yes"])
            LIBS=$LIBS_SAVE
            CXXFLAGS=$CXXFLAGS_SAVE
            AC_MSG_RESULT([$can_use_threads])
            if test "x$can_use_threads" = "xyes" ; then
                LIBS="$LIBS $PTHREAD_LIBS"
                CXXFLAGS="$CXXFLAGS $PTHREAD_CFLAGS"
            fi
        fi
    fi
    if test "x$can_use_threads" = "xno" ; then
        AC_MSG_ERROR([cannot use threads])
    fi
])

AC_DEFUN([VPGO_INIT_CPP], [
    dnl Use C++
    AC_LANG([C++])

    dnl C++17
    AX_CXX_COMPILE_STDCXX_17([], [mandatory])

    dnl We use threads
    VPGO_INIT_THREADS
])
