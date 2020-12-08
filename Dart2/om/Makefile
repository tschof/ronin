#MAKEFILE
DEBUG=0

SUBDIRS = server_objects ex_db utpdirect arca_opt dom_con jigglypuff oats glob ouch ice globex boe phaser nfx box ise 2sigma phlx mldma drop citadel inet pillfix gset gsetopt fidessa test

.PHONY: subdirs $(SUBDIRS)

subdirs: $(SUBDIRS)

SUBDIR_CLEAN = $(patsubst %,%.clean,$(SUBDIRS))
SUBDIR_INDENT = $(patsubst %,%.indent,$(SUBDIRS))

.PHONY: $(SUBDIR_CLEAN)
$(SUBDIR_CLEAN):
	@make -C $(@:.clean=) clean

.PHONY: clean
clean: $(SUBDIR_CLEAN)
	
.PHONY: $(SUBDIR_INDENT)
$(SUBDIR_INDENT):
	@make -C $(@:.indent=) indent

.PHONY: indent
indent: $(SUBDIR_INDENT)

$(SUBDIRS):
	$(MAKE) -C $@ DEBUG=$(DEBUG) $(TARGET)


jigglypuff: ex_db

ex_db: server_objects

tags:
	ctags -a jigglypuff/*.[ch]
	ctags -a ex_db/*.[ch]
	ctags -a server_objects/*.[ch]
	ctags -a glob/*.[ch]
	ctags -a arcadirect/*.[ch]
	ctags -a dom_con/*.[ch]
	ctags -a ice/*.[ch]
	ctags -a oats/*.[ch]
	ctags -a utpdirect/*.[ch]
	ctags -a ouch/*.[ch]
	ctags -a nfx/*.[ch]
	ctags -a phaser/*.[ch]
