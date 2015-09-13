ACLOCAL_AMFLAGS = -I m4
SUBDIRS = src tests
bin_PROGRAMS = vpgo
vpgo_SOURCES = main.cpp
vpgo_LDADD = src/libvpgo.a
EXTRA_DIST = README.md $(DX_CONFIG)
include aminclude.am
MOSTLYCLEANFILES = $(DX_CLEANFILES)

if DX_COND_html

html: doxygen-doc

endif
