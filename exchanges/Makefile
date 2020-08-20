#MAKEFILE
DEBUG=0

SUBDIRS = server_objects smtp ex_common ex_db nyse complex fix_server tsx inet bats lx arca edg knight cscf ise csalgo redi mock_connector async_test pcx phlx nomura globex knightopt utpdirect utp_driver arcadirect ad_driver test ouch nsdqopt rash boe boeopt liffe-us box chix cboe newedge bofaml ice psql_db

.PHONY: subdirs $(SUBDIRS)

subdirs: $(SUBDIRS)

SUBDIR_CLEAN = $(patsubst %,%.clean,$(SUBDIRS))

.PHONY: $(SUBDIR_CLEAN)
$(SUBDIR_CLEAN):
	@make -C $(@:.clean=) clean

.PHONY: clean
clean: $(SUBDIR_CLEAN)

$(SUBDIRS):
	$(MAKE) -C $@ DEBUG=$(DEBUG) $(TARGET)

nyse: ex_common

ex_common: ex_db

ex_db: smtp

smtp: server_objects
