test_assign1: storage_mgr.o dberror.o test_assign1_1.o
	gcc storage_mgr.o dberror.o test_assign1_1.o -o test_assign1

storage_mgr.o: storage_mgr.c storage_mgr.h
	gcc -c storage_mgr.c

dberror.o: dberror.c dberror.h
	gcc -c dberror.c

test_assign1_1.o: test_assign1_1.c
	gcc -c test_assign1_1.c

clean:
	rm *.o test_assign1