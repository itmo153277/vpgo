ACLOCAL_AMFLAGS = -I m4
SUBDIRS = src tests
bin_PROGRAMS = vpgo
vpgo_SOURCES = main.cpp
vpgo_LDADD = src/libvpgo.a
@DX_RULES@
EXTRA_DIST = README.md $(DX_CONFIG)
MOSTLYCLEANFILES = $(DX_CLEANFILES)

if MAKE_MAN

dist_man3_MANS = $(top_builddir)/doc/man/man3/*.3
$(dist_man3_MANS): doxygen-doc

endif