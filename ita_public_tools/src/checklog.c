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
/* CHECKLOG.C                                                             */
/**************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "definitions.h"
#include "endian.h"
#include "request.h"

#define NUM_OF_SERVERS 256
#define MAX_INT 0x7fffffff

int Server[NUM_OF_SERVERS];
int UniqueServers=0;

uint32_t TotalRequests=0;
double TotalBytes = 0.0;
uint32_t MinObject= MAX_INT;
uint32_t MaxObject=0;
uint32_t MinClient= MAX_INT;
uint32_t MaxClient=0;
uint32_t MinTime = MAX_INT;
uint32_t MaxTime = 0;
uint32_t MinSize = MAX_INT;
uint32_t MaxSize = 0;
uint32_t StartTime;
uint32_t LastTime = 0;
int OutOfOrder=0;
int Endian = NO_ENDIAN;

/**************************************************************************/
/* READ LOG                                                               */
/**************************************************************************/
void ReadLog()
{
  struct request BER, LER, *R;
  int value;
  int count=0;
  int raw, part1,part2;
  int region,server;
  int http, status;
  int method, type;

  /* status indicator */
  fprintf(stderr,"Reading Access Log\n");

  /* read the initial request */
  if((fread(&BER, sizeof(struct request), 1, stdin)) != 1)
    {
      fprintf(stderr,"Error: Failed to read initial request!\n");
      exit(0);
    }

  switch(Endian)
    {
    case LITTLE_ENDIAN:
      LittleEndianRequest(&BER, &LER);
      R = &LER;
      break;

    case BIG_ENDIAN:
      R = &BER;
      break;
      
    default:
      fprintf(stderr,"Error: unknown Endian!\n");
      exit(-1);
    }
	
  /* save start time */
  StartTime = R->timestamp;

  /* read through access log */
  while((!feof(stdin)) && (1))
    {
      /* status indicator */
      if(count % 1000000 == 0)
	fprintf(stderr,"%d\n", count);
      count++;

      /* update statistics */
      TotalRequests++;
      if(R->size != NO_SIZE)
	TotalBytes+= R->size;

      /* check timestamp */
      if(R->timestamp < LastTime)
	OutOfOrder++;

      /* update minima and maxima */
      MinTime = MINIMUM(MinTime, R->timestamp);
      MaxTime = MAXIMUM(MaxTime, R->timestamp);
      MinObject = MINIMUM(MinObject, R->objectID);
      MaxObject = MAXIMUM(MaxObject, R->objectID);
      MinClient = MINIMUM(MinClient, R->clientID);
      MaxClient = MAXIMUM(MaxClient, R->clientID);
      if(R->size != NO_SIZE)
	{
	  MinSize = MINIMUM(MinSize, R->size);
	  MaxSize = MAXIMUM(MaxSize, R->size);
	}

      /* update last time */
      LastTime = R->timestamp;

      /* determine the server and region */
      raw = (int)R->server;

      /* update server statistics */
      if((raw >=0) && (raw < NUM_OF_SERVERS))
	{
	  if(Server[raw] == 0)
	    UniqueServers++;
	  Server[raw]++;
	}
      else
	{
	  fprintf(stderr,"Error: invalid server info! %d\n", raw);
	  exit(0);
	}

      part1 = raw & 0xe0;
      part1 = part1 >> 5;
      part2 = raw & 0x1f;
      region = part1;
      server = part2;

      /* safety checks */
      if(region >= NUM_OF_REGIONS)
	{
	  fprintf(stderr,"Error: invalid region! %d\n", region);
	  exit(0);
	}

      if(server >= 32)
	{
	  fprintf(stderr,"Error: invalid server! %d\n", server);
	  exit(0);
	}

      /* determine http version and status */
      raw = (int)R->status;
      
      /* safety check */
      if((raw < 0) || (raw > 255))
	{
	  fprintf(stderr,"Error: invalid status info! %d\n", raw);
	  exit(0);
	}

      part1 = raw & 0xc0;
      part1 = part1 >> 6;
      part2 = raw & 0x3f;
      http = part1;
      status = part2;

      /* safety checks */
      if(http >= NUM_OF_PROTOCOLS)
	{
	  fprintf(stderr,"Error: invalid protocol! %d\n", http);
	  exit(0);
	}

      if(status >= NUM_OF_CODES)
	{
	  fprintf(stderr,"Error: invalid status code! %d\n", status);
	  exit(0);
	}

      /* determine the method */
      method = (int)R->method;

      /* safety check */
      if((method < 0) || (method >= NUM_OF_METHODS))
	{
	  fprintf(stderr,"Error: invalid method! %d\n", method);
	  exit(0);
	}

      /* determine the filetype */
      type = (int)R->type;

      /* safety check */
      if((type < 0) || (type >= NUM_OF_FILETYPES))
	{
	  fprintf(stderr,"Error: invalid filetype! %d\n", type);
	  exit(0);
	}

      /* read the next request */
      value = fread(&BER, sizeof(struct request), 1, stdin);

      if(value == 1)
	{
	  switch(Endian)
	    {
	    case LITTLE_ENDIAN:
	      LittleEndianRequest(&BER, &LER);
	      R = &LER;
	      break;
	      
	    case BIG_ENDIAN:
	      R = &BER;
	      break;
	      
	    default:
	      fprintf(stderr,"Error: unknown Endian!\n");
	      exit(-1);
	    }
	}
    }

  /* final count */
  fprintf(stderr,"%d\n", count);
}

/**************************************************************************/
/* PRINT RESULTS                                                          */
/**************************************************************************/
void PrintResults()
{
  int i;
  uint32_t total=0;
  int unique = 0;

  /* status indicator */
  fprintf(stderr,"Printing Results\n");

  fprintf(stdout,"    Total Requests: %u\n", TotalRequests);
  fprintf(stdout,"       Total Bytes: %.0f\n", TotalBytes);
  fprintf(stdout,"Mean Transfer Size: %f\n", 
	  TotalBytes/(double)TotalRequests);
  fprintf(stdout,"     Min Client ID: %u\n", MinClient);
  fprintf(stdout,"     Max Client ID: %u\n", MaxClient);
  fprintf(stdout,"     Min Object ID: %u\n", MinObject);
  fprintf(stdout,"     Max Object ID: %u\n", MaxObject);
  fprintf(stdout,"          Min Size: %u\n", MinSize);
  fprintf(stdout,"          Max Size: %u\n", MaxSize);
  fprintf(stdout,"          Min Time: %u\n", MinTime);
  fprintf(stdout,"          Max Time: %u\n", MaxTime);

  fprintf(stdout,"        Start Time: %u\n", StartTime);
  fprintf(stdout,"       Finish Time: %u\n", LastTime);
  fprintf(stdout,"      Out of Order: %d\n", OutOfOrder);

  fprintf(stdout,"    Unique Servers: %d\n", UniqueServers);
  
  for(i=0; i<NUM_OF_SERVERS; i++)
    if(Server[i] > 0)
      {
	fprintf(stdout,"Server %d: %d\n", i, Server[i]);
	total += Server[i];
	unique++;
      }
  fprintf(stdout,"Check:\n");
  fprintf(stdout,"Total Requests: %u\n", total);
  fprintf(stdout,"Unique Servers: %d\n", unique);
}

/**************************************************************************/
/* INITIALIZE                                                             */
/**************************************************************************/
void Initialize(int argc, char **argv)
{
  int i;

  /* status indicator */
  fprintf(stderr,"Initializing\n");

  /* determine which endian platform uses */
  Endian = CheckEndian();

  /* initialize server statistics */
  for(i=0; i<NUM_OF_SERVERS; i++)
    Server[i]=0;
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

  PrintResults();

  Terminate();

  return(1);
}
