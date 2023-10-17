--------*** CS525 - Advanced Database Organization ***--------
-----------*** Assignment - 1 - Storage Manager ***-----------

---------------*** Implemented By Group - 49 ***--------------
- Sarthak Raghuvanshi (A20543280)
- Amarthya Parvatha Reddy (A20550000)
- Neel Patel (A20524638)


----------------------------------------------------------------------------------------------------------------------------------


==> Steps to test the Storage Manager :-

-> We have implemented all the tests in "test_assign1_1.c" file.

-> Following command will compile all the C files into it's consecutive O (object) files and consequently makes a single executable file "test_assign1.exe" :-
$ make

-> Following command will run the executable file "test_assign1.exe" :-
$ ./test_assign1


----------------------------------------------------------------------------------------------------------------------------------

==> We have added additional error codes in dberror.h :-

RC_FILE_READ_ERROR 5
RC_STORAGE_MGR_NOT_INIT 6
RC_FILE_PRESENT 7
RC_WRITE_OUT_OF_BOUND 8
RC_FILE_SEEK_ERROR 9
RC_FILE_NOT_CLOSE 10

==> We have also added additional Test case in test_assign1_1.c :-

void extra_test_cases(void)

-----------------------------------------------------------------------------------------------------------------------------------


==> Our Implemente  d Function's Description :-

1. Functions to manipulate page files (i.e Create, Open, Close, Destroy) :-

a) initStorageManager():
    - We have set the status of StorageManagerStatus variable as false 
    - This function first checks wheather the status is false.
    - If yes, then this function will change the status to true which indicates that the storage manager is initialized.

b) createPageFile():
    - This function creates a new page file of 1 page (PAGE_SIZE).
    - Here we have used fopen() function to create the file.
    - The file will be opened in the write mode and filled with '\0' bytes.

c) openPageFile():
    - First it checks that input file is present or not.
    - If file is present then it opens the file otherwise it throws error "RC_FILE_NOT_FOUND".
    - After that we are calculating the total number of pages by pointing the file pointer to end and then dividing it by the page size.
    - After that we are updating the file handle with current page position, file name and total number of pages.

d) closePageFile():
    - This function closes the already opened page file.
    - Here we have used fclose() function to close the file.
    - It returns "RC_OK" if the file is closed successfully or else it returns "RC_FILE_NOT_FOUND".

e) destroyPageFile():
    - This function destroys already created page file using remove function.
    - It returns "RC_OK" if the file destroys successfully or else it returns "RC_FILE_NOT_FOUND" if remove fails.



2. Read Functions :-

a) readBlock():
    - It checks if the pageNum is within the limits of the total number of pages and throws an error if it is not within the limits.
    - Then we will set the pointer position to the page which is to be read.
    - Then we read the block to the "memPage" using fread function.
    - And then we set the current page position to the page number that we just read and returns "RC_OK".

b) getBlockPos():
    - This functions returns the current page position in the page file by using "fhandle".

c) readFirstBlock():
    - First it checks storage manager's status then it checks the fhandle's status.
    - Then we set the file position to the beginning (First Page).
    - Then we read the first block in the page file using fread to the "memPage".
    - At last we set the current page position to the first block and return "RC_OK".
    - If any errors occur during reading, it returns "RC_FILE_READ_ERROR".

d) readPreviousBlock():
   - This function is used to fetch a block of data from the file that is one block before the current page position.
   - First it checks if the storage manager is initialized and if the file handle is valid.
   - It calculates the position of the previous block and ensures that it is within valid bounds and returns "RC_READ_NON_EXISTING_PAGE" if not.
   - Otherwise, it seeks to the beginning of the previous block, reads the block into "memPage", updates the current page position, and returns "RC_OK".
   - If any errors occur during reading, it returns "RC_FILE_READ_ERROR".

e) readCurrentBlock():
   - This function is used to fetch a block of data from the file that is pointed to, by the current page position.
   - It calculates the offset to the current block based on the current page position and seeks to that position in the file.
   - It reads the block into "memPage", updates the current page position (incrementing it by 1), and returns "RC_OK".
   - If any errors occur during reading, it returns "RC_FILE_READ_ERROR".

f) readNextBlock():
   - This function is used to fetch a block of data from the file that is one block after the current page position.
   - It calculates the position of the next block and ensures that it is within valid bounds and returns "RC_READ_NON_EXISTING_PAGE" if not.
   - Otherwise, it seeks to the beginning of the next block, reads the block into "memPage", updates the current page position, and returns "RC_OK".
   - If any errors occur during reading, it returns "RC_FILE_READ_ERROR".



3. Write Functions :-

a) writeBlock():
   - This function writes a block of data from "memPage" into the file at the specified "pageNum".
   - It first checks if the file handle is initialized and if "pageNum" is within valid bounds.
   - It opens the file for writing in binary mode.
   - Then, it seeks to the position where we have to write, writes the data.
   - Finally,it sets the page number to current position, closes the file and returns "RC_OK".

b) writeCurrentBlock():
   - This function is similar to "writeBlock()" but writes the content of "memPage" to the current page position.
   - It checks if the file handle is initialized, and opens the file for writing.
   - Then, it seeks to the current page position, writes the data, closes the file and returns "RC_OK".

c)  appendEmptyBlock():
   - This function adds an empty block (a page filled with '\0' bytes) at the end of the file.
   - It first checks if the file handle is initialized and opens the file for appending and reading in binary mode.
   - Then, it seeks to the end of the file, creates an empty block, writes it to the file, and updates the file handle information.
   - Finally, it returns "RC_OK" and closes the file.

d) ensureCapacity():
   - This function checks if the file has a sufficient number of pages ("numberOfPages") and adds empty pages if needed.
   - If the total number of pages in the file is less than the required number of pages, it appends new empty pages to the end of the file until the capacity is met.
   - It calls "appendEmptyBlock()" to add each empty page and returns "RC_OK".
   - If the current capacity is already greater than or equal to the required capacity, it returns "RC_WRITE_FAILED".

