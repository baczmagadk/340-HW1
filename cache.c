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
  /* TODO: Completed */

  int retVal=OK;
  unsigned int x=*hex_addr;
  unsigned int tagPlaceDM=x>>(NUM_BLOCK_OFFSET_BITS+NUM_OF_LINE_BITS);
  unsigned int lineNumDM=(x>>NUM_BLOCK_OFFSET_BITS)&((unsigned int)pow(2,NUM_OF_LINE_BITS)-1);

  unsigned int tagPlaceSA=x>>(NUM_BLOCK_OFFSET_BITS+(addr_bits-(NUM_BLOCK_OFFSET_BITS+NUM_OF_TAG_BITS_SA)));
  unsigned int setNum=(x>>NUM_BLOCK_OFFSET_BITS)&((unsigned int)pow(2,(addr_bits-(NUM_BLOCK_OFFSET_BITS+NUM_OF_TAG_BITS_SA)))-1);
  unsigned int line0OfSet=(((1+setNum)*2)-2);
  unsigned int line1OfSet=(((1+setNum)*2)-1);

  switch (cmf) {
      case DM:  // Direct Mapping
        if (tagPlaceDM==cache[lineNumDM]->tag) {
          cache[lineNumDM]->hit_count++;
          *found=1;
          *replace=0;
          retVal=phy_memory[*hex_addr];
        }
        else {
          cache[lineNumDM]->tag=tagPlaceDM;
          cache[lineNumDM]->hit_count=0;
          *found=0;
          *replace=1;
          retVal=phy_memory[*hex_addr];
        }
        break;

      // end case DM

      /*
       * In set associative you get the set bits from which in the case of 00001100 would be the 1s.
       * Then once you have the set that its going in to, you have to decide the line it will go into
       * and in order to determine the line you have to check if either is empty and
       * if neither line is empty then you have to replace the one with the lower hit count and do hit_count++
       */

      case SA:    // Set Associative
        if (cache[line0OfSet]->tag==-1 && cache[line1OfSet]->tag==-1) { // if both tags are -1, set the first tag
          cache[line0OfSet]->tag=tagPlaceSA;
          *found=0;
          *replace=1;
          retVal=phy_memory[*hex_addr];
        }
        else if (cache[line0OfSet]->tag!=-1 && cache[line1OfSet]->tag==-1) { // if the second tag is -1, set the second tag
          cache[line1OfSet]->tag=tagPlaceSA;
          *found=0;
          *replace=1;
          retVal=phy_memory[*hex_addr];
        }
        else if (cache[line0OfSet]->tag!=-1 && cache[line1OfSet]->tag!=-1) { // if both tags are not equal to -1
          if (cache[line0OfSet]->hit_count < cache[line1OfSet]->hit_count) { // if the second line's hit count is greater, increment hit on line 1
            cache[line0OfSet]->hit_count++;
            *found=1;
            *replace=0;
            retVal=phy_memory[*hex_addr];
          }
          else if (cache[line0OfSet]->hit_count > cache[line1OfSet]->hit_count) { // if the first line's hit count is greater, increment hit on line 2
            cache[line1OfSet]->hit_count++;
            *found=1;
            *replace=0;
            retVal=phy_memory[*hex_addr];
          } else {  // else the both hit counts are the same, increment hit on line 1
            cache[line0OfSet]->hit_count++;
            *found=1;
            *replace=0;
            retVal=phy_memory[*hex_addr];
          }
        } else {
          retVal=phy_memory[*hex_addr];
        }
        break;
      //end case SA

      default:
        retVal = FAIL;
      // end switch statement
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
