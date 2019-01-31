// -----------------------------------
// CSCI 340 - Operating Systems
// Spring 2019
// cache.c file
// 
// Homework 1
//
// -----------------------------------

#include "cache.h"
#include "memory.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int initializeCache(unsigned int number_of_lines)
{
  int line, retVal;

  retVal = OK;

  cache = malloc(sizeof(cache_line*) * number_of_lines);

  if (cache != NULL) {

    for (line=0; line<number_of_lines; line++) {

      cache[line] = (cache_line*) malloc(sizeof(cache_line));
      cache[line]->tag = UNK;
      cache[line]->hit_count = ZERO;
      
    }
    
  } else
    retVal = FAIL;

  return retVal;

} // end initializeCache function


int cread(unsigned int cmf, unsigned int* hex_addr, unsigned int* found,
	  unsigned int* replace)
{
  /* TODO: You complete */

  int retVal = OK;
  unsigned int x=*hex_addr;
  unsigned int tagPlaceDM=x>>(NUM_BLOCK_OFFSET_BITS+NUM_OF_LINE_BITS);
  unsigned int lineNum=(x>>NUM_BLOCK_OFFSET_BITS)&((unsigned int)pow(2,NUM_OF_LINE_BITS)-1);

  unsigned int tagPlaceSA = x >> (NUM_BLOCK_OFFSET_BITS + numberOfBlocks(addr_bits,NUM_BLOCK_OFFSET_BITS));
  unsigned int blockNum = ( x >> NUM_BLOCK_OFFSET_BITS) & ((unsigned int) pow(2,numberOfBlocks(addr_bits,NUM_BLOCK_OFFSET_BITS))-1);


  switch (cmf) {
      case DM:  // Direct Mapping
        if (tagPlaceDM == cache[lineNum]->tag) {
          cache[lineNum]->hit_count++;
          *found=1;
          *replace=0;
          retVal = phy_memory[*hex_addr];
        }
        else {
          cache[lineNum]->tag=tagPlaceDM;
          cache[lineNum]->hit_count=0;
          *found=0;
          *replace=1;
          retVal = phy_memory[*hex_addr];
        }
        break;

      // end case DM

      case SA:    // Set Associative
        /*if (tagPlaceSA == cache[lineNum]->tag) {
          cache[lineNum]->hit_count++;
          *found=1;
          *replace=0;
          retVal = phy_memory[*hex_addr];
        }
        else {
          cache[lineNum]->tag=tagPlaceDM;
          cache[lineNum]->hit_count=0;
          *found=0;
          *replace=1;
          retVal = phy_memory[*hex_addr];
        }*/

        retVal = phy_memory[*hex_addr];
        break;

      //end case SA

      default:
        retVal = FAIL;
  }

  cprint();
  return retVal;

} // end cread function



void cprint()
{
  unsigned int line;

  printf("\n---------------------------------------------\n");
  printf("line\ttag\tnum of hits\n");
  printf("---------------------------------------------\n");

  for (line=0; line<NUM_OF_LINES; line++) { 

    if (cache[line]->tag == UNK) {
			
      printf("%d\t%d\t%d\n", line, cache[line]->tag, cache[line]->hit_count);

    } else {

      printf("%d\t%02X\t%d\n", line, cache[line]->tag, cache[line]->hit_count);

    }

  }

} // end cprint function
