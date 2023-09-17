#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/stat.h>
#include "storage_mgr.h"
#include "dberror.h"
#ifndef F_OK
#define F_OK 0 /* Define F_OK if it's not already defined */
#endif

bool StorageManagerStatus = false;
bool fileIsOpen = false;
void initStorageManager(void)
{
    if (StorageManagerStatus == false)
    {
        StorageManagerStatus = true;
        printf("\n <----- Starting Storage Manager------->\n");
        printf("       <--------BY Group 49-------->      \n");
    }
}

RC createPageFile(char *fileName)
{

    RC memorySize = PAGE_SIZE * sizeof(char);
    char *memoryBlock = malloc(memorySize);

    FILE *fp = fopen(fileName, "w+");

    if (fp == 0)
    {
        return RC_FILE_NOT_FOUND;
    }

    fileIsOpen = true;
    memset(memoryBlock, '\0', PAGE_SIZE);
    fwrite(memoryBlock, sizeof(char), PAGE_SIZE, fp);
    fclose(fp);

    free(memoryBlock);
    return RC_OK;
}

RC openPageFile(char *fileName, SM_FileHandle *fHandle)
{
     if (StorageManagerStatus != true)
    {
        return RC_STORAGE_MGR_NOT_INIT;
    }

    if (access(fileName, F_OK) == -1)
    {
        return RC_FILE_NOT_FOUND;
    }
    FILE *fp = fopen(fileName, "r+");

    fseek(fp, 0, SEEK_END); // pointing to end

    int LastByte = ftell(fp);
    int length = LastByte + 1;

    fHandle->totalNumPages = length / PAGE_SIZE;
    
    fHandle->curPagePos = 0;
    fHandle->fileName = fileName;
    fHandle->mgmtInfo = fp;

    fseek(fp, 0, SEEK_SET); // pointing back to start;

    return RC_OK;
}

// create file in Storage Manger

extern RC closePageFile(SM_FileHandle *fHandle)
{
    if (StorageManagerStatus != true)
    {
        return RC_STORAGE_MGR_NOT_INIT;
    }
    if (fHandle == NULL || fHandle->mgmtInfo == NULL)
        return RC_FILE_HANDLE_NOT_INIT;
    FILE *fp = fHandle->mgmtInfo;

    if (fclose(fp) == 0)
    {
        fHandle->mgmtInfo = NULL;
        fileIsOpen = false;
        return RC_OK;
    }
    else
    {
        return RC_FILE_NOT_FOUND;
    }
}

RC destroyPageFile(char *fileName)
{
    // Use the remove function to delete the file
    if (fileIsOpen == true)
    {
        return RC_FILE_NOT_CLOSE;
    }
    int fileflag = remove(fileName);
    if (fileflag != 0)
    {
        // If remove fails, return an error code
        return RC_FILE_NOT_FOUND; // You can define this error code in dberror.h
    }
    // printf("\n in Destroy function \n");
    return RC_OK;
}

RC readBlock(int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPage)
{
    printf("in read block");
    if (StorageManagerStatus != true)
        return RC_STORAGE_MGR_NOT_INIT;

    if (fHandle == NULL || fHandle->fileName == NULL)
        return RC_FILE_NOT_FOUND;

    if (pageNum < 0 || pageNum >= fHandle->totalNumPages)
        return RC_READ_NON_EXISTING_PAGE;
  
    FILE *fp= fHandle->mgmtInfo;
    RC position = pageNum * PAGE_SIZE;
    fseek(fp, position, SEEK_SET);
    RC ReadErrorFlag = fread(memPage, sizeof(char), PAGE_SIZE, fp);
  
    if (ReadErrorFlag < PAGE_SIZE || ReadErrorFlag > PAGE_SIZE)
    {
       
        return RC_READ_NON_EXISTING_PAGE;
    }
    fHandle->curPagePos = pageNum;
   
    return RC_OK;
}

RC getBlockPos(SM_FileHandle *fHandle)
{
    if (fileIsOpen == true)
    {
        RC block_pos = fHandle->curPagePos;
        return block_pos;
    }
}

extern RC readFirstBlock(SM_FileHandle *fHandle, SM_PageHandle memPage)
{   
    printf("in readFirstBlock ");
    if (StorageManagerStatus != true)
    {
        return RC_STORAGE_MGR_NOT_INIT;
    }
    // Check if the file handle is initialized
    if (fHandle == NULL || fHandle->mgmtInfo == NULL)
    {
        return RC_FILE_HANDLE_NOT_INIT;
    }

    // Set the file position to the beginning (first page)
    FILE *fp = fHandle->mgmtInfo;
    RC position = 0;
    fseek(fp, position, SEEK_SET);

    // Read the first block (page) into memPage
    RC ReadErrorFlag = fread(memPage, sizeof(char), PAGE_SIZE, fp);

    if (ReadErrorFlag != PAGE_SIZE)
    {
        return RC_FILE_READ_ERROR;
    }

    // Update the current page position
    fHandle->curPagePos = 0;

    return RC_OK;
}

extern RC readPreviousBlock(SM_FileHandle *fHandle, SM_PageHandle memPage)
{
    printf("in readPreviousBlock ");
    if (StorageManagerStatus != true)
    {
        return RC_STORAGE_MGR_NOT_INIT;
    }
    // Check if the file handle is initialized
   
    FILE *fp = fHandle->mgmtInfo;
    // Calculate the position of the previous block
    RC position = fHandle->curPagePos - 1;

    // Ensure the previous block is within bounds
    if (position < 0)
    {   
        printf("inside if IMP");
        
        return RC_READ_NON_EXISTING_PAGE;
    }

   printf("OUTSIDE if");

    // Set the file position to the beginning of the previous block
    fseek(fp, position * PAGE_SIZE, SEEK_SET);

    // Read the previous block (page) into memPage
    RC ReadErrorFlag = fread(memPage, sizeof(char), PAGE_SIZE, fp);

    if (ReadErrorFlag != PAGE_SIZE)
    {
        return RC_FILE_READ_ERROR;
    }

    // Update the current page position
    fHandle->curPagePos = position;

    return RC_OK;
}

RC readCurrentBlock(SM_FileHandle *fHandle, SM_PageHandle memPage)
{
    printf("in readCurrentBlock ");
    if (StorageManagerStatus != true)
    {
        return RC_STORAGE_MGR_NOT_INIT;
    }

    // Check if the file handle is initialized
     if (fHandle == NULL || fHandle->mgmtInfo == NULL)
    {
        return RC_FILE_HANDLE_NOT_INIT;
    }
    FILE *fp = fHandle->mgmtInfo;
    // Calculate the offset to the current block
    RC position = fHandle->curPagePos * PAGE_SIZE;
    fseek(fp, position, SEEK_SET);
    RC ReadErrorFlag = fread(memPage, sizeof(char), PAGE_SIZE, fp);

    if (ReadErrorFlag != PAGE_SIZE)
    {

        return RC_FILE_READ_ERROR;
    }

    // Update the current page position
    fHandle->curPagePos = position + 1;

    return RC_OK;
}

RC readNextBlock(SM_FileHandle *fHandle, SM_PageHandle memPage)
{
    printf("in readNextBlock ");
    if (StorageManagerStatus != true)
        return RC_STORAGE_MGR_NOT_INIT;

    if (fHandle == NULL || fHandle->fileName == NULL)
        return RC_FILE_NOT_FOUND;

    RC pageNum = fHandle->curPagePos + 1;

    if (pageNum < 0 || pageNum >= fHandle->totalNumPages)
        return RC_READ_NON_EXISTING_PAGE;

    FILE *fp = fHandle->mgmtInfo;
    fseek(fp, pageNum * PAGE_SIZE, SEEK_SET);
    RC read_size = fread(memPage, sizeof(char), PAGE_SIZE, fp);
    if (read_size < PAGE_SIZE || read_size > PAGE_SIZE)
    {

        return RC_READ_NON_EXISTING_PAGE;
    }
    fHandle->curPagePos = pageNum;
    return RC_OK;
}

RC readLastBlock(SM_FileHandle *fHandle, SM_PageHandle memPage)
{
    printf("in readLastBlock ");
    if (StorageManagerStatus != true)
        return RC_STORAGE_MGR_NOT_INIT;

    if (fHandle == NULL || fHandle->fileName == NULL)
        return RC_FILE_NOT_FOUND;

    RC pageNum = fHandle->totalNumPages - 1;

    if (pageNum < 0 || pageNum >= fHandle->totalNumPages)
        return RC_READ_NON_EXISTING_PAGE;

    FILE *fp;
    fseek(fp, pageNum * PAGE_SIZE, SEEK_SET);
    RC read_size = fread(memPage, sizeof(char), PAGE_SIZE, fp);
    if (read_size < PAGE_SIZE || read_size > PAGE_SIZE)
    {

        return RC_READ_NON_EXISTING_PAGE;
    }
    fHandle->curPagePos = pageNum;
    return RC_OK;
}

// Implementation of the writeBlock function
RC writeBlock(int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPage)
{
    // Check if the file handle is initialized
    // Check if the file handle is initialized
    if (fHandle == NULL || fHandle->mgmtInfo == NULL)
    {
        printf("file not handle found");
        return RC_FILE_HANDLE_NOT_INIT;
    }

    // Check if the pageNum is valid
    if (pageNum < 0 || pageNum >= fHandle->totalNumPages)
    {
        printf("write out of bound found");
        return RC_WRITE_OUT_OF_BOUND;
    }

    // Calculate the offset for fseek
    long offset = (long)pageNum * PAGE_SIZE;

    // Open the file for writing in binary mode
    FILE *file = fopen(fHandle->fileName, "rb+"); // "rb+" for read and write in binary mode

    if (file == NULL)
    {
        printf("file not found");
        return RC_FILE_NOT_FOUND;
    }

    // Seek to the appropriate position
    if (fseek(file, offset, SEEK_SET) != 0)
    {
        fclose(file);
        printf("file seek error found");
        return RC_FILE_SEEK_ERROR;
    }

    // Write the page data
    size_t bytesWritten = fwrite(memPage, sizeof(char), PAGE_SIZE, file);

    if (bytesWritten != PAGE_SIZE)
    {
        fclose(file);
        printf("file failed");
        return RC_WRITE_FAILED;
    }

    // Update the current page position
    fHandle->curPagePos = pageNum;

    // Close the file
    fclose(file);
    printf("write block completed");
    return RC_OK;
}

// Implementation of the writeCurrentBlock function
RC writeCurrentBlock(SM_FileHandle *fHandle, SM_PageHandle memPage)
{
    // Check if the file handle is initialized
    if (fHandle == NULL || fHandle->mgmtInfo == NULL)
    {
        return RC_FILE_HANDLE_NOT_INIT;
    }

    // Calculate the offset for fseek based on the current page position
    long offset = (long)fHandle->curPagePos * PAGE_SIZE;

    // Open the file for writing in binary mode
    FILE *file = fopen(fHandle->fileName, "rb+"); // "rb+" for read and write in binary mode

    if (file == NULL)
    {
        return RC_FILE_NOT_FOUND;
    }

    // Seek to the appropriate position
    if (fseek(file, offset, SEEK_SET) != 0)
    {
        fclose(file);
        return RC_FILE_SEEK_ERROR;
    }

    // Write the page data
    size_t bytesWritten = fwrite(memPage, sizeof(char), PAGE_SIZE, file);

    if (bytesWritten != PAGE_SIZE)
    {
        fclose(file);
        return RC_WRITE_FAILED;
    }

    // Close the file
    fclose(file);

    return RC_OK;
}

// Implementation of the appendEmptyBlock function
RC appendEmptyBlock(SM_FileHandle *fHandle)
{
    // Check if the file handle is initialized
    if (fHandle == NULL || fHandle->mgmtInfo == NULL)
    {
        return RC_FILE_HANDLE_NOT_INIT;
    }

    // Open the file for writing in binary mode
    FILE *file = fopen(fHandle->fileName, "ab+"); // "ab+" for append and read in binary mode

    if (file == NULL)
    {
        return RC_FILE_NOT_FOUND;
    }

    // Seek to the end of the file
    if (fseek(file, 0, SEEK_END) != 0)
    {
        fclose(file);
        return RC_FILE_SEEK_ERROR;
    }

    // Create an empty block (PAGE_SIZE bytes filled with '\0')
    char emptyBlock[PAGE_SIZE] = {0};

    // Write the empty block to the file
    size_t bytesWritten = fwrite(emptyBlock, sizeof(char), PAGE_SIZE, file);

    if (bytesWritten != PAGE_SIZE)
    {
        fclose(file);
        return RC_WRITE_FAILED;
    }

    // Update the file handle information
    fHandle->totalNumPages++;                         // Increment the total number of pages
    fHandle->curPagePos = fHandle->totalNumPages - 1; // Set current page position to the newly appended page

    // Close the file
    fclose(file);
    return RC_OK;
}

////
RC ensureCapacity(int numberOfPages, SM_FileHandle *fHandle)
{

    if (fHandle->totalNumPages < numberOfPages)
    { // If the new capacity is greater than current capacity

        for (int i = 0; i < numberOfPages - fHandle->totalNumPages; i++)
        {
            appendEmptyBlock(fHandle);
        }
        return RC_OK;
    }

    else
        return RC_WRITE_FAILED;
}
