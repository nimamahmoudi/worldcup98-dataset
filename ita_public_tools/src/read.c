/*
 
          Copyright (C) 1997, 1998, 1999 Hewlett-Packard Company
                         ALL RIGHTS RESERVED.
 
  The enclosed software and documentation includes copyrighted works
  of Hewlett-Packard Co. For as long as you comply with the following
  limitations, you are hereby authorized to (i) use, reproduce, and
  modify the software and documentation, and to (ii) distribute the
  software and documentation, including modifications, for
  non-commercial purposes only.
      
  1.  The enclosed software and documentation is made available at no
      charge in order to advance the general development of
      the Internet, the World-Wide Web, and Electronic Commerce.
 
  2.  You may not delete any copyright notices contained in the
      software or documentation. All hard copies, and copies in
      source code or object code form, of the software or
      documentation (including modifications) must contain at least
      one of the copyright notices.
 
  3.  The enclosed software and documentation has not been subjected
      to testing and quality control and is not a Hewlett-Packard Co.
      product. At a future time, Hewlett-Packard Co. may or may not
      offer a version of the software and documentation as a product.
  
  4.  THE SOFTWARE AND DOCUMENTATION IS PROVIDED "AS IS".
      HEWLETT-PACKARD COMPANY DOES NOT WARRANT THAT THE USE,
      REPRODUCTION, MODIFICATION OR DISTRIBUTION OF THE SOFTWARE OR
      DOCUMENTATION WILL NOT INFRINGE A THIRD PARTY'S INTELLECTUAL
      PROPERTY RIGHTS. HP DOES NOT WARRANT THAT THE SOFTWARE OR
      DOCUMENTATION IS ERROR FREE. HP DISCLAIMS ALL WARRANTIES,
      EXPRESS AND IMPLIED, WITH REGARD TO THE SOFTWARE AND THE
      DOCUMENTATION. HP SPECIFICALLY DISCLAIMS ALL WARRANTIES OF
      MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
  
  5.  HEWLETT-PACKARD COMPANY WILL NOT IN ANY EVENT BE LIABLE FOR ANY
      DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES
      (INCLUDING LOST PROFITS) RELATED TO ANY USE, REPRODUCTION,
      MODIFICATION, OR DISTRIBUTION OF THE SOFTWARE OR DOCUMENTATION.
 
*/
/**************************************************************************/
/* READ.C                                                                 */
/**************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "definitions.h"
#include "endian.h"
#include "request.h"

/**************************************************************************/
/* GLOBAL VARIABLES                                                       */
/**************************************************************************/
int Endian = NO_ENDIAN;

/**************************************************************************/
/* READ LOG                                                               */
/**************************************************************************/
void ReadLog()
{
  struct request BER, LER, *R;
  int value;
  int count=0;

  /* status indicator */
  fprintf(stderr,"Reading Access Log\n");

  /* read the initial request */
  if((fread(&BER, sizeof(struct request), 1, stdin)) != 1)
    {
      fprintf(stderr,"Error: Failed to read initial request!\n");
      exit(0);
    }

  while(!feof(stdin))
    {
      /* status indicator */
      if(count %1000000 == 0)
	fprintf(stderr,"%d\n", count);
      count++;

      switch(Endian)
	{
	case LITTLE_ENDIAN:
	  {
	    LittleEndianRequest(&BER, &LER);
	    R = &LER;
	  }
	  break;
	  
	case BIG_ENDIAN:
	  {
	    R = & BER;
	  }
	  break;

	default:
	  {
	    fprintf(stderr,"Error: unknown Endian!\n");
	    exit(-1);
	  }
	}
#ifdef VERBOSE
      fprintf(stdout,"%u %u %u %u %d %d %d %d\n", 
	      R->timestamp, R->clientID, R->objectID, R->size,
	      (int)R->method,(int)R->status,(int)R->type,(int)R->server);
#endif

      /* read the next request */
      value = fread(&BER, sizeof(struct request), 1, stdin);
    }

  /* final count */
  fprintf(stderr,"%d\n", count);
}

/**************************************************************************/
/* INITIALIZE                                                             */
/**************************************************************************/
void Initialize(int argc, char **argv)
{
  /* status indicator */
  fprintf(stderr,"Initializing\n");

  /* determine which endian platform uses */
  Endian = CheckEndian();
}

/**************************************************************************/
/* TERMINATE                                                              */
/**************************************************************************/
void Terminate()
{
  /* status indicator */
  fprintf(stderr,"Terminating\n");
}

/**************************************************************************/
/* MAIN PROGRAM                                                           */
/**************************************************************************/
int main(int argc, char **argv)
{
  Initialize(argc, argv);

  ReadLog();

  Terminate();

  return(1);
}
