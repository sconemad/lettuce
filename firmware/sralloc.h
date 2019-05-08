/*****************************************************************************
 *
 *              Simple SRAM Dynamic Memory Allocation 
 *
 *****************************************************************************
 * FileName:        sralloc.c
 * Dependencies:    
 * Processor:       PIC18F with CAN
 * Compiler:       	C18 02.20.00 or higher
 * Linker:          MPLINK 03.40.00 or higher
 * Company:         Microchip Technology Incorporated
 *
 * Software License Agreement
 *
 * The software supplied herewith by Microchip Technology Incorporated
 * (the "Company") is intended and supplied to you, the Company's
 * customer, for use solely and exclusively with products manufactured
 * by the Company. 
 *
 * The software is owned by the Company and/or its supplier, and is 
 * protected under applicable copyright laws. All rights are reserved. 
 * Any use in violation of the foregoing restrictions may subject the 
 * user to criminal sanctions under applicable laws, as well as to 
 * civil liability for the breach of the terms and conditions of this 
 * license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN "AS IS" CONDITION. NO WARRANTIES, 
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED 
 * TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A 
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT, 
 * IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR 
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 *****************************************************************************/

#ifndef __SRALLOC_H
#define __SRALLOC_H

/*********************************************************************
 * Function:        unsigned char * SRAMalloc(unsigned char length)
 *
 * PreCondition:    A memory block must be allocated in the linker, 
 *					and	the memory headers and tail must already be 
 *					set via the	function SRAMInitHeap().
 *
 * Input:        	unsigned char nBytes - Number of bytes to allocate.	
 *                  
 * Output:         	unsigned char * - A pointer to the requested block
 *					of memory.
 *
 * Side Effects:    
 *
 * Overview:        This functions allocates a chunk of memory from
 *					the heap. The maximum segment size for this 
 *					version is 126 bytes. If the heap does not have
 *					an available segment of sufficient size it will
 *					attempt to create a segment; otherwise a NULL 
 *					pointer is returned. If allocation is succeessful
 *					then a pointer to the requested block is returned.
 *
 * Note:            The calling function must maintain the pointer
 *					to correctly free memory at runtime.
 ********************************************************************/
unsigned char * SRAMalloc(unsigned char nBytes);


/*********************************************************************
 * Function:        void SRAMfree(unsigned char * pSRAM)
 *
 * PreCondition:  	The pointer must have been returned from a 
 *					previously allocation via SRAMalloc().  
 *
 * Input:        	unsigned char * pSRAM - pointer to the allocated 	
 *                  
 * Output:         	void
 *
 * Side Effects:    
 *
 * Overview:       	This function de-allocates a previously allocated
 *					segment of memory. 
 *
 * Note:            The pointer must be a valid pointer returned from
 *					SRAMalloc(); otherwise, the segment may not be 
 *					successfully de-allocated, and the heap may be 
 *					corrupted.
 ********************************************************************/
void SRAMfree(unsigned char * pSRAM);


/*********************************************************************
 * Function:        void SRAMInitHeap(void)
 *
 * PreCondition:    
 *
 * Input:  			void      	
 *                  
 * Output:         	void
 *
 * Side Effects:    
 *
 * Overview:      	This function initializes the dynamic heap. It 
 *					inserts segment headers to maximize segment space.
 *
 * Note:            This function must be called at least one time. 
 *					And it could be called more times to reset the 
 *					heap.
 ********************************************************************/
void SRAMInitHeap(void);


#endif