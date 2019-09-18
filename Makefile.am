ACLOCAL_AMFLAGS = -I m4
AUTOMAKE_OPTIONS = subdir-objects

VPGO_MAIN = \
	src/board.cc
noinst_HEADERS = \
	src/vpgo.hpp \
	src/board.hpp \
	src/colour.hpp
VPGO_UNITTESTS = \
	tests/board.cc \
	tests/colour.cc
VPGO_M4 = \
	m4/deps.m4 \
	m4/tests.m4

bin_PROGRAMS = vpgo
vpgo_SOURCES = \
	main.cpp \
	$(VPGO_MAIN)
vpgo_CPPFLAGS = $(AM_CPPFLAGS) $(BOOST_CPPFLAGS)
vpgo_LDADD = $(BOOST_LDFLAGS) $(BOOST_PROGRAM_OPTIONS_LIB)

if UNITTESTS_ENABLED
UNITTESTS=unittests
else
UNITTESTS=
endif
TESTS = $(UNITTESTS)
LOG_DRIVER = env AM_TAP_AWK='$(AWK)' $(SHELL) \
	 $(top_srcdir)/auxdir/tap-driver.sh --ignore-exit
unittests_CPPFLAGS = $(AM_CPPFLAGS) $(BOOST_CPPFLAGS) $(BOOST_UNIT_TEST_FRAMEWORK_CPPFLAGS)
unittests_LDADD = $(BOOST_LDFLAGS) $(BOOST_UNIT_TEST_FRAMEWORK_LIB)
unittests_SOURCES = \
	tests/init.cc \
	$(VPGO_MAIN) \
	$(VPGO_UNITTESTS)

check_PROGRAMS = $(UNITTESTS)

@DX_RULES@

EXTRA_DIST = \
	autogen.sh \
	README.md \
	$(DX_CONFIG) \
	$(VPGO_M4)

MOSTLYCLEANFILES = $(DX_CLEANFILES)

if MAKE_MAN

dist_man3_MANS = $(top_builddir)/doc/man/man3/*.3
$(dist_man3_MANS): doxygen-doc

endif
