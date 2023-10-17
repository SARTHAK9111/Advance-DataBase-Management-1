#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "storage_mgr.h"
#include "dberror.h"
#include "test_helper.h"

// test name
char *testName;

/* test output files */
#define TESTPF "test_pagefile.bin"

/* prototypes for test functions */
static void testCreateOpenClose(void);
static void testSinglePageContent(void);
static void extra_test_cases(void);

/* main function running all tests */
int main(void)
{
  testName = "";

  initStorageManager();

  // testCreateOpenClose();
  // testSinglePageContent();

  extra_test_cases();

  return 0;
}

/* check a return code. If it is not RC_OK then output a message, error description, and exit */
/* Try to create, open, and close a page file */
void testCreateOpenClose(void)
{
  SM_FileHandle fh;

  testName = "test create open and close methods";

  TEST_CHECK(createPageFile(TESTPF));

  TEST_CHECK(openPageFile(TESTPF, &fh));
  ASSERT_TRUE(strcmp(fh.fileName, TESTPF) == 0, "filename correct");
  ASSERT_TRUE((fh.totalNumPages == 1), "expect 1 page in new file");
  ASSERT_TRUE((fh.curPagePos == 0), "freshly opened file's page position should be 0");

  TEST_CHECK(closePageFile(&fh));
  printf("tring to destroy file");
  TEST_CHECK(destroyPageFile(TESTPF));

  // after destruction trying to open the file should cause an error
  ASSERT_TRUE((openPageFile(TESTPF, &fh) != RC_OK), "opening non-existing file should return an error.");

  TEST_DONE();
}

/* Try to create, open, and close a page file */
void testSinglePageContent(void)
{
  SM_FileHandle fh;
  SM_PageHandle ph;
  int i;

  testName = "test single page content";

  ph = (SM_PageHandle)malloc(PAGE_SIZE);

  // create a new page file
  TEST_CHECK(createPageFile(TESTPF));
  TEST_CHECK(openPageFile(TESTPF, &fh));
  printf("created and opened file\n");

  // read first page into handle
  TEST_CHECK(readFirstBlock(&fh, ph));
  // the page should be empty (zero bytes)
  for (i = 0; i < PAGE_SIZE; i++)
    ASSERT_TRUE((ph[i] == 0), "expected zero byte in first page of freshly initialized page");

  printf("first block was empty\n");

  // change ph to be a string and write that one to disk
  for (i = 0; i < PAGE_SIZE; i++)

    ph[i] = (i % 10) + '0';
  TEST_CHECK(writeBlock(0, &fh, ph));
  printf("writing first block\n");

  // read back the page containing the string and check that it is correct
  TEST_CHECK(readFirstBlock(&fh, ph));

  TEST_CHECK(readCurrentBlock(&fh, ph));
  for (i = 0; i < PAGE_SIZE; i++)
    ASSERT_TRUE((ph[i] == (i % 10) + '0'), "character in page read from disk is the one we expected.");
  printf("reading first block\n");
  TEST_CHECK(closePageFile(&fh));
  // destroy new page file
  TEST_CHECK(destroyPageFile(TESTPF));

  TEST_DONE();
}

void extra_test_cases(void)
{
  SM_FileHandle fh;
  SM_PageHandle ph;
  int i;
  printf("\n**********Extra case*****************\n");
  int currentpage;
  testName = "test page create read";

  char DataToWrite[]="Storage Manger By Sarthak, Amarthya And Neel.";
  ph = (SM_PageHandle)malloc(PAGE_SIZE);

  // create a new page file
  TEST_CHECK(createPageFile(TESTPF));
  TEST_CHECK(openPageFile(TESTPF, &fh));
  printf("\ncreated and opened file\n");

  // read first page into handle
  TEST_CHECK(readFirstBlock(&fh, ph));
  // the page should be empty (zero bytes)
  for (i = 0; i < PAGE_SIZE; i++)
    ASSERT_TRUE((ph[i] == 0), "expected zero byte in first page of freshly initialized page");
  printf("\nfirst block was empty\n");

  ensureCapacity(10, &fh); // created total 10 pages in the file (File size - 40 KB)

  fseek(fh.mgmtInfo, 0, SEEK_END);
  printf("\nFile Size after adding 10 pages (ensureCapacity):%d", (int)(ftell(fh.mgmtInfo)));
  fseek(fh.mgmtInfo, 0, SEEK_SET);

  currentpage = getBlockPos(&fh);

  printf("\nCurrent Page: %d", currentpage);

  readPreviousBlock(&fh, ph);

  printf("\nCurrent Page after reading previous block: %d", getBlockPos(&fh));

  readNextBlock(&fh, ph);

  printf("\nCurrent Page after reading next block: %d", getBlockPos(&fh));

  printf("\nTotal number of pages: %d\n", fh.totalNumPages);

 for (i = 0; i < PAGE_SIZE; i++)
  {
    int itr = i % 100;
    if(itr > strlen(DataToWrite)){
      itr=itr%(strlen(DataToWrite));
      ph[itr]=DataToWrite[itr];
   
    }
  }
  TEST_CHECK(writeCurrentBlock(&fh, ph));
  printf("\nwriting the current block\n");

  TEST_CHECK(readCurrentBlock(&fh, ph));
  printf("\nReading characters in the current block that were just written: ");
  for (i = 0; i < PAGE_SIZE; i++)
    printf("%c", ph[i]);
  printf("\n");
  TEST_CHECK(closePageFile(&fh));
  // destroy new page file
  ASSERT_TRUE(destroyPageFile(TESTPF) == RC_OK, "File has been destroyed");

  TEST_DONE();
}
