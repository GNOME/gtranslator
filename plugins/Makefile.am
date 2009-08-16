SUBDIRS = 					\
	alternate-language			\
	fullscreen				\
	insert-tags				\
	insert-params				\
	source-code-view

if USE_CHARMAP
SUBDIRS += charmap
endif

if USE_LIBSOUP
SUBDIRS += open-tran
endif

if USE_DICTIONARY
SUBDIRS += dictionary
endif

if BUILD_SVN
SUBDIRS += subversion
endif

-include $(top_srcdir)/git.mk
