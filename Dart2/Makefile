#MAKEFILE
DEBUG=0

SUBDIRS = rex dartlib dom_con cfeboe fix_server oats dart_direct utpdirect nfx fix_dma utp_dma eurex pillar nbconnector otto boeopt eqboe pilllib test 

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
	ctags -a dart_direct/*.[ch]
	ctags -a dartlib/*.[ch]
	ctags -a rex/*.[ch]
	ctags -a dartglob/*.[ch]
	ctags -a pillar/*.[ch]
	ctags -a dom_con/*.[ch]
	ctags -a utp_dma/*.[ch]
	ctags -a oats/*.[ch]
	ctags -a utpdirect/*.[ch]
	ctags -a eurex/*.[ch]
	ctags -a nbconnector/*.[ch]
	ctags -a pilllib/*.[ch]
	ctags -a pilldirect/*.[ch]
	ctags -a cfeboe/*.[ch]
	ctags -a c2boe/*.[ch]
	ctags -a otto/*.[ch]
	ctags -a eqboe/*.[ch]
