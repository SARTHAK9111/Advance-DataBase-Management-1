test_assign1: 		
	gcc -I. -c -o storage_mgr.o storage_mgr.c
	gcc -I. -c -o test_assign1_1.o test_assign1_1.c
	gcc -I. -c -o dberror.o dberror.c
	gcc -I. -o test_assign1 test_assign1_1.o storage_mgr.o dberror.o


clean:
	rm -rf test_assign1 *.o

all: test_assign1
