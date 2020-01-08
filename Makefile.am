ACLOCAL_AMFLAGS = -I m4
AUTOMAKE_OPTIONS = subdir-objects

VPGO_MAIN = \
	src/board.cc \
	src/hash.cc
noinst_HEADERS = \
	src/vpgo.hpp \
	src/board.hpp \
	src/colour.hpp \
	src/hash.hpp
VPGO_UNITTESTS = \
	tests/board.cc \
	tests/colour.cc
VPGO_M4 = \
	m4/deps.m4 \
	m4/tests.m4

noinst_LIBRARIES = libvpgomain.a
libvpgomain_a_SOURCES = $(VPGO_MAIN)

bin_PROGRAMS = vpgo
vpgo_SOURCES = \
	main.cpp
vpgo_CPPFLAGS = $(AM_CPPFLAGS) $(BOOST_CPPFLAGS)
vpgo_LDADD = $(BOOST_LDFLAGS) $(BOOST_PROGRAM_OPTIONS_LIB) libvpgomain.a

if UNITTESTS_ENABLED
UNITTESTS=unittests
else
UNITTESTS=
endif
TESTS = $(UNITTESTS)
LOG_DRIVER = env AM_TAP_AWK='$(AWK)' BOOST_TEST_REPORT_LEVEL=no $(SHELL) \
	 $(top_srcdir)/auxdir/tap-driver.sh --ignore-exit
unittests_CPPFLAGS = $(AM_CPPFLAGS) $(BOOST_CPPFLAGS) $(BOOST_UNIT_TEST_FRAMEWORK_CPPFLAGS)
unittests_LDADD = $(BOOST_LDFLAGS) $(BOOST_UNIT_TEST_FRAMEWORK_LIB) libvpgomain.a
unittests_SOURCES = \
	tests/init.cc \
	$(VPGO_UNITTESTS)

check_PROGRAMS = $(UNITTESTS)

@DX_RULES@

$(DX_CONFIG): doxygen.cfg
	$(AM_V_GEN)(cp $< $@ ; $(DX_DOXYGEN) -s -u $@ > /dev/null)

EXTRA_DIST = \
	autogen.sh \
	README.md \
	doxygen.cfg \
	$(VPGO_M4)

MOSTLYCLEANFILES = $(DX_CLEANFILES) $(DX_CONFIG)

if MAKE_DOXYGEN_MAN
man3_MANS = $(top_builddir)/doc/man/man3/*.3
$(man3_MANS): doxygen-run
endif
