SUBDIRS = 					\
	alternate-language			\
	charmap					\
	codeview				\
	fullscreen				\
	insert-params				\
	translation-memory

if USE_LIBJSON_GLIB
SUBDIRS += open-tran
endif

if USE_DICTIONARY
SUBDIRS += dictionary
endif

-include $(top_srcdir)/git.mk
