testsrc=tokenizer.c inputstream.c op.c rdonly_csv_msg.c rw_fix_msg.c opnode.c rule.c proto-rule-parser.c grammar.c ../../server_objects/hashmap.c value.c symbol_table.c translation_engine.c util.c message_layout.c translation_unit.c

all:
	gcc -g3 -Wall -Wextra -shared -fPIC -DCOMPILE_UNIT_TESTS $(testsrc) -I ../../ -L ../../server_objects -lserver_obj -o libengine-tests.so

clean:
	rm -f libengine-tests.so

tags:
	rm -f TAGS
	etags *.[ch]
