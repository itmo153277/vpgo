TEST_FILES = ut-test.test
EXTRA_DIST = $(TEST_FILES)
TESTS = $(TEST_FILES)
check_PROGRAMS = tester
TEST_LOG_COMPILER = $(top_builddir)/tests/tester

tester_SOURCES = tester.cpp ut-test.cpp
tester_LDADD = @CHECK_LIBS@  $(top_builddir)/src/libvpgo.a
tester_CXXFLAGS = @CHECK_CFLAGS@
tester_CFLAGS = @CHECK_CFLAGS@

noinst_HEADERS = tester.h
