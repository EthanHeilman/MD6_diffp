SRC_DIR=src

INCLUDE_PATH=-I$(SRC_DIR)
GCC=gcc $(CFLAGS) -Wall -O2 $(INCLUDE_PATH) 

compile : 
	$(GCC) -o diffp $(SRC_DIR)/diffp.c 

run_test : compile_test
	./testdiff

compile_test :
	$(GCC) -o testdiff $(SRC_DIR)/test.c 

clean :
	rm testdiff diffp
