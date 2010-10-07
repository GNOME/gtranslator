SUBDIRS = 					\
	alternate-language			\
	codeview				\
	fullscreen				\
	insert-tags				\
	insert-params

if USE_CHARMAP
SUBDIRS += charmap
endif

if USE_LIBSOUP
SUBDIRS += open-tran
endif

if USE_DICTIONARY
SUBDIRS += dictionary
endif

-include $(top_srcdir)/git.mk
