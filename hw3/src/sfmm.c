#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "sfmm.h"
//#include "sfmm2.h"

/**
 * All functions you make for the assignment must be implemented in this file.
 * Do not submit your assignment with a main function in this file.
 * If you submit with a main function in this file, you will get a zero.
 */

sf_free_header* freelist_head = NULL;
int numSBRK=0;
static size_t myinternal=0;
static size_t myexternal=0;
static size_t myallocations=0;
static size_t myfrees=0;
static size_t mycoalesce=0;
/**
* This is your implementation of malloc. It creates dynamic memory which
* is aligned and padded properly for the underlying system. This memory
* is uninitialized.
* @param size The number of bytes requested to be allocated.
* @return If successful, the pointer to a valid region of memory to use is
* returned, else the value NULL is returned and the ERRNO is set  accordingly.
* If size is set to zero, then the value NULL is returned.
*/
void *sf_malloc(size_t size){
	size_t adjustedSize;
	char* bp;
	int doubleWordSize= 2*SF_HEADER_SIZE;
	sf_footer *footer;
	sf_free_header *currentBlock;
	sf_footer *currentFooter;
	sf_free_header *newBlock;
	sf_footer *newFooter;

	if (size==0) 
		return NULL;

	if(freelist_head==NULL){	//if no heap
		freelist_head=sf_sbrk(0);
		sf_sbrk(1);
		numSBRK=numSBRK+1;
		myexternal=myexternal+4096;
		freelist_head->header.block_size=4096;
		freelist_head->header.alloc=0;
		freelist_head->header.unused_bits=0;
		freelist_head->header.padding_size=0;
		freelist_head->next=NULL;
		freelist_head->prev=NULL;
		footer = (void*)freelist_head + (4096) - 8;
		footer->alloc=0;
		footer->block_size=4096;
	}

	if (size<= doubleWordSize){//adjust for padding + alignment
		adjustedSize = 4*SF_HEADER_SIZE;
	}else{
		adjustedSize = doubleWordSize * ((size + doubleWordSize + doubleWordSize-1) / doubleWordSize);
	}
	while(myexternal<adjustedSize){

		newBlock=sf_sbrk(0);
		newBlock->prev=NULL;
		newBlock->next=freelist_head;
		freelist_head=newBlock;

		newFooter=sf_sbrk(1)-8;
		newFooter->block_size=4096;
		newBlock->header.block_size=4096;
		
		numSBRK=numSBRK+1;
		myexternal=myexternal+4096;
		/*
		newBlock=sf_sbrk(0);
		newBlock->header.block_size=4096;
		newFooter=(void*)newBlock+(4096<<4)-8;
		newFooter->block_size=newBlock->header.block_size;
		
		currentFooter=(void*)newBlock-8;
		currentBlock=(void*)newBlock-(currentFooter->block_size<<4);
		
		sf_sbrk(1);
		myexternal=myexternal+4096;
		numSBRK=numSBRK+1;
		
		if(currentFooter->alloc==0){
			currentBlock->header.block_size= currentFooter->block_size+newBlock->header.block_size;
			newFooter->block_size=newFooter->block_size+currentFooter->block_size;
			printf("%d\n", currentBlock->header.block_size<<4);
		}
		*/
		if(numSBRK>4){
		return NULL;
		}
	}

	

	currentBlock=freelist_head;
	while((currentBlock->header.block_size<<4) <= adjustedSize){
		if(currentBlock->next != NULL){
			currentBlock=currentBlock->next;
		}//else return NULL;
	}
	if((currentBlock->header.block_size<<4) - adjustedSize < 32){
		//splinter
		if(currentBlock->prev!=NULL){
			currentBlock->prev->next=currentBlock->next;
		}
		if (currentBlock->next!=NULL){
			currentBlock->next->prev=currentBlock->prev;
		}
		freelist_head=currentBlock->next;
		/*
		currentBlock->prev = NULL;
		currentBlock->next=NULL;
		currentBlock->header.alloc=1;
		currentBlock->header.padding_size=currentBlock->header.block_size - adjustedSize;
		currentFooter=(void*)currentFooter+ adjustedSize-8;
		currentFooter*/
	}else{
		//not splinter
		newBlock = (void*)currentBlock + adjustedSize;
		newBlock->prev=currentBlock->prev;
		newBlock->next=currentBlock->next;
		newBlock->header.alloc=0;
		newBlock->header.block_size=currentBlock->header.block_size - (adjustedSize>>4);
		newBlock->header.unused_bits=0;
		newBlock->header.padding_size=0;
		newFooter=(void*)newBlock+(newBlock->header.block_size<<4)-8;
		newFooter->alloc=newBlock->header.alloc;
		newFooter->block_size = newBlock->header.block_size;

		// printf("%d %d newblock sizes",newBlock->header.block_size, newFooter->block_size );
		if(currentBlock->prev!=NULL){
			currentBlock->prev->next=newBlock;
		}
		if (currentBlock->next!=NULL){
			currentBlock->next->prev=newBlock;
		}
		freelist_head=newBlock;
	}


	currentBlock->header.block_size=adjustedSize>>4;
	currentBlock->header.alloc= 1;
	currentBlock->header.unused_bits=0;
	currentBlock->header.padding_size=(adjustedSize)-size;
	currentBlock->prev = NULL;
	currentBlock->next = NULL;
	currentFooter=(void*)currentBlock + (adjustedSize) -8;
	currentFooter->block_size = currentBlock->header.block_size;
	currentFooter->alloc=currentBlock->header.alloc;
	
	myexternal=myexternal-adjustedSize;
	myinternal= myinternal+ 2 + ((adjustedSize-size)<<4);
	myallocations=myallocations+1;
	bp = (void*)currentBlock + 8;
	return bp;
	
}
/**
* Marks a dynamically allocated region as no longer in use.
* Adds the newly freed block to the free list.
* @param ptr Address of memory returned by the function sf_malloc.
*/
void sf_free(void *ptrr){
	sf_free_header *prevHead,*nextHead,*ptr;
	sf_footer *prevFoot,*nextFoot,*myFoot;

	ptr=ptrr -8;
	myFoot=(void*)ptr+(ptr->header.block_size<<4)-8;
	myFoot->alloc=1;
	myFoot->block_size=ptr->header.block_size;
	prevFoot=(void*)ptr-8;
	prevHead=(void*)prevFoot+8-(prevFoot->block_size<<4);
	nextHead=(void*)ptr+(ptr->header.block_size<<4);
	nextFoot=(void*)nextHead+(nextHead->header.block_size<<4)-8;

	if (!ptr){
		return;
	}
	if(prevFoot->alloc==1 && nextHead->header.alloc==1){//case 1
		ptr->header.alloc=0;
		myFoot->alloc=0;
		mycoalesce=mycoalesce+1;
		ptr->header.padding_size=0;

	}else if(prevFoot->alloc==1 && nextHead->header.alloc==0){//case 2
		ptr->header.alloc=0;
		ptr->header.block_size=(ptr->header.block_size)+(nextFoot->block_size);
		nextFoot->block_size=ptr->header.block_size;
		myFoot->alloc=0;
		mycoalesce=mycoalesce+1;
		ptr->header.padding_size=0;
		/*
		nextHead->prev->next=nextHead->next;//remove me from list
		nextHead->next->prev=nextHead->prev;
		*/	
	}else if(prevFoot->alloc==0 && nextHead->header.alloc==1){//case 3
		mycoalesce=mycoalesce+1;
		ptr->header.alloc=0;
		myFoot->alloc=0;
		myFoot->block_size=ptr->header.block_size+prevFoot->block_size;
		prevHead->header.block_size=myFoot->block_size;
		ptr->header.padding_size=0;
		/*
		prevHead->prev->next=prevHead->next;//remove me from list
		prevHead->next->prev=prevHead->prev;
		*/

	}else if(prevFoot->alloc==0 && nextHead->header.alloc==0){//case 4
		mycoalesce=mycoalesce+1;

		ptr->header.alloc=0;
		myFoot->alloc=0;
		nextFoot->block_size=myFoot->block_size+prevFoot->block_size+nextFoot->block_size;
		prevHead->header.block_size=nextFoot->block_size;
		ptr->header.padding_size=0;
		/*
		prevHead->prev->next=nextHead->next;//remove me from list
		nextHead->next->prev=prevHead->prev;
		*/
	}

	if(freelist_head->prev!=NULL){//moving freelist heads
			freelist_head->prev->next=ptr;
		}
		ptr->prev=freelist_head->prev;
		ptr->next=freelist_head;
		freelist_head->prev=ptr;
		freelist_head=ptr;
		myfrees=myfrees+1;
}
/**
 * Resizes the memory pointed to by ptr to be size bytes.
 * @param ptr Address of the memory region to resize.
 * @param size The minimum size to resize the memory to.
 * @return If successful, the pointer to a valid region of memory to use is
 * returned, else the value NULL is returned and the ERRNO is set accordingly.
 *
 * A realloc call with a size of zero should return NULL and set the ERRNO
 * accordingly.
 */
void *sf_realloc(void *ptr, size_t size){
	sf_free_header *oldHead=(void*)ptr -8;//my header pointer
	sf_footer *oldFooter= (void*)oldHead+(oldHead->header.block_size<<4)-8;
	sf_free_header *newHead;
	sf_footer *newFooter;
	int doubleWordSize= 2*SF_HEADER_SIZE;
	size_t adjustedSize;

	if (size<= doubleWordSize){//adjust for padding + alignment
		adjustedSize = 4*SF_HEADER_SIZE;
	}else{
		adjustedSize = doubleWordSize * ((size + doubleWordSize + doubleWordSize-1) / doubleWordSize);
	}

	if((oldHead->header.block_size>>4)>adjustedSize){//shrink
		if((oldHead->header.block_size>>4)-adjustedSize < 32)//splinter
			return (void*)oldHead +8;
		else{//not splinter
			newHead=(void*)oldHead + adjustedSize;
			newHead->header.alloc=0;
			newHead->header.padding_size=0;
			newHead->header.block_size=(oldHead->header.block_size)-(adjustedSize>>4);
			newHead->header.unused_bits=0;

			newHead->prev=freelist_head->prev;//set as new freelisthead
			if(freelist_head->prev){
				freelist_head->prev->next=newHead;
			}
			newHead->next=freelist_head;
			freelist_head->prev=newHead;
			newHead=freelist_head;

			newFooter=(void*)oldFooter;
			newFooter->alloc=newHead->header.alloc;
			newFooter->block_size=newHead->header.block_size;

			oldHead->header.padding_size=adjustedSize-size;
			oldHead->header.block_size=adjustedSize>>4;

			oldFooter=(void*)oldHead+(oldHead->header.block_size<<4)-8;
			oldFooter->block_size=oldHead->header.block_size;
			oldFooter->alloc=oldHead->header.alloc;

			sf_free((void*)newHead+8);
			return (void*)oldHead +8;


		}
	}else if((oldHead->header.block_size>>4)<adjustedSize){//expand
		newHead=(void*)oldFooter+8;
		newFooter=(void*)newHead+(newHead->header.block_size<<4)-8;
		if(newHead->header.alloc==0){//next to a free block
			if((oldHead->header.block_size<<4)+(newHead->header.block_size<<4)>=adjustedSize){//enuf room

				if( ((oldHead->header.block_size<<4)+(newHead->header.block_size<<4)-adjustedSize)<32 ){//splinter
					oldHead->header.block_size=oldHead->header.block_size+newHead->header.block_size;
					oldFooter=(void*)oldHead+(oldHead->header.block_size<<4)-8;
					oldFooter->alloc=oldHead->header.alloc;
					oldFooter->block_size=oldHead->header.block_size;

					newHead->header.alloc=1;
					newFooter->alloc=1;
					newFooter->block_size=newHead->header.block_size+oldHead->header.block_size;

					if(newHead->prev){//remove me from list
						newHead->prev->next=newHead->next;
					}
					if(newHead->next){
						newHead->next->prev=newHead->prev;
					}
					return (void*)oldHead +8;

				}else{
					size_t leftover=(newHead->header.block_size<<4)-(adjustedSize-(oldHead->header.block_size<<4));
					newFooter->block_size=leftover>>4;//set nwfoot block size

					sf_free_header *origi=(void*)newHead;
					newHead=(void*)newFooter+8-(newFooter->block_size<<4);
					newHead->header.alloc=0;
					newHead->header.block_size=newFooter->block_size;
					newHead->header.unused_bits=0;
					newHead->header.padding_size=0;
					newHead->next=origi->next;
					newHead->prev=origi->prev;
					if(newHead->prev){
						newHead->prev->next=newHead;
					}
					if(newHead->next){
						newHead->next->prev=newHead;
					}

					oldHead->header.block_size=adjustedSize>>4;
					oldHead->header.padding_size=adjustedSize-size;

					oldFooter=(void*)oldHead+(oldHead->header.block_size<<4)-8;
					oldFooter->block_size=oldHead->header.block_size;
					oldFooter->alloc=oldHead->header.alloc;

					return (void*)oldHead +8;


				} 
			}

			
		}else{
			ptr=malloc(size);
			return ptr;
		}
	}else{
  		return ptr;
  	}
  	return NULL;
}
/**
 *  This function will copy the the correct values to the fields
 *  in the memory info struct.
 *  @param meminfo A pointer to the memory info struct passed
 *  to the function, upon return it will containt the calculated
 *  for current fragmentation
 *  @return If successful return 0, if failure return -1
 */
int sf_info(info* meminfo){
	if(!meminfo){
		return -1;
	}

	meminfo->internal=myinternal;
	meminfo->external=myexternal;
	meminfo->allocations=myallocations;
	meminfo->coalesce=mycoalesce;
	meminfo->frees=myfrees;
	return 0;
}

