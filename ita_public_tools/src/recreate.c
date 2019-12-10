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
/* RECREATE.C                                                             */
/* This program recreates the Common Log Format from the reduced binary   */
/* log format.                                                            */
/**************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "inttypes.h"
#include "definitions.h"
#include "endian.h"
#include "request.h"

#define NUM_ENTRIES 90000

char *ObjectNames[NUM_ENTRIES];
FILE *object_mappings;

int PrintLog=TRUE;
uint32_t TotalRequests=0;
double TotalBytes = 0.0;
uint32_t MaxObject=0;
uint32_t MaxClient=0;
uint32_t StartTime;
uint32_t LastTime = 0;
int OutOfOrder=0;
int Endian = NO_ENDIAN;

/**************************************************************************/
/* DEFINITIONS                                                            */
/**************************************************************************/
char ProtocolNames[NUM_OF_PROTOCOLS][20] = {"HTTP/0.9",
					    "HTTP/1.0",
					    "HTTP/1.1",
					    "HTTP/X.X"};

char MethodNames[NUM_OF_METHODS][20] = {"GET",
					"HEAD",
					"POST",
					"PUT",
					"DELETE",
					"TRACE",
					"OPTIONS",
					"CONNECT",
					"OTHER"};

char CodeNames[NUM_OF_CODES][5] = {"100",
				   "101",
				   "200",
				   "201",
				   "202",
				   "203",
				   "204",
				   "205",
				   "206",
				   "300",
				   "301",
				   "302",
				   "303",
				   "304",
				   "305",
				   "400",
				   "401",
				   "402",
				   "403",
				   "404",
				   "405",
				   "406",
				   "407",
				   "408",
				   "409",
				   "410",
				   "411",
				   "412",
				   "413",
				   "414",
				   "415",
				   "500",
				   "501",
				   "502",
				   "503",
				   "504",
				   "505",
				   "-"};

char Month[12][5] = {"Jan",
		     "Feb",
		     "Mar",
		     "Apr",
		     "May",
		     "Jun",
		     "Jul",
		     "Aug",
		     "Sep",
		     "Oct",
		     "Nov",
		     "Dec"};

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
  time_t ts;
  struct tm *time_info;
  char date[1024], day[10], hour[10], min[10],sec[10];
  int year;

  /* status indicator */
  fprintf(stderr,"Reading Access Log\n");

  /* read the initial request */
  if((fread(&BER, sizeof(struct request), 1, stdin)) != 1)
    {
      fprintf(stderr,"Error: Failed to read initial request!\n");
      exit(-1);
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

      /* update last time */
      LastTime = R->timestamp;

      /* check for max client */
      MaxClient = MAXIMUM(MaxClient, R->clientID);

      /* check for max object */
      MaxObject = MAXIMUM(MaxObject, R->objectID);

      /* determine the server and region */
      raw = (int)R->server;
      
      part1 = raw & 0xe0;
      part1 = part1 >> 5;
      part2 = raw & 0x1f;
      region = part1;
      server = part2;

      /* determine http version and status */
      raw = (int)R->status;

      part1 = raw & 0xc0;
      part1 = part1 >> 6;
      part2 = raw & 0x3f;
      http = part1;
      status = part2;

      /* safety check */
      if((http < 0) || (http >= NUM_OF_PROTOCOLS))
	{
	  fprintf(stderr,"Error: incorrect protocol version! (%d)\n",
		  http);
	  exit(-1);
	}

      /* safety check */
      if((status < 0) || (status >= NUM_OF_CODES))
	{
	  fprintf(stderr,"Error: incorrect status code! (%d)\n",
		  status);
	  exit(-1);
	}

      
      /* determine the method */
      method = (int)R->method;
      
      /* safety check */
      if((method < 0) || (method >= NUM_OF_METHODS))
	{
	  fprintf(stderr,"Error: incorrect method! (%d)\n", 
		  method);
	  exit(-1);
	}

      /* determine the filetype */
      type = (int)R->type;
      
      /* safety check */
      if((type < 0) || (type >= NUM_OF_FILETYPES))
	{
	  fprintf(stderr,"Error: incorrect type! (%d)\n",
		  type);
	  exit(-1);
	}

      /* safety check */
      if(R->objectID >= NUM_ENTRIES)
	{
	  fprintf(stderr,"Error: ObjectID is invalid! (%u)\n",
		  R->objectID);
	  exit(-1);
	}
      else if(ObjectNames[R->objectID]==NULL)
	{
	  fprintf(stderr,"Error: No URL found for given ObjectID! (%u)\n",
		  R->objectID);
	  exit(-1);
	}

      if(PrintLog == TRUE)
	{
	  /* determine timestamp info */
	  ts = (time_t)R->timestamp;
	  time_info = gmtime(&ts);
	  
	  if(time_info->tm_sec < 10)
	    sprintf(sec,"0%d", time_info->tm_sec);
	  else
	    sprintf(sec,"%d", time_info->tm_sec);
	  
	  if(time_info->tm_min < 10)
	    sprintf(min,"0%d", time_info->tm_min);
	  else
	    sprintf(min,"%d", time_info->tm_min);
	  
	  if(time_info->tm_hour < 10)
	    sprintf(hour,"0%d", time_info->tm_hour);
	  else
	    sprintf(hour,"%d", time_info->tm_hour);
	  
	  year = 1900 + time_info->tm_year;
	  
	  if(time_info->tm_mday < 10)
	    sprintf(day,"0%d", time_info->tm_mday);
	  else
	    sprintf(day,"%d", time_info->tm_mday);
	  
	  sprintf(date,"%s/%s/%d:%s:%s:%s +0000",
		  day, Month[time_info->tm_mon], year, hour, min, sec);
	  
	  /* print info in Common Log Format */
	  if(R->size != NO_SIZE)
	    fprintf(stdout,"%u - - [%s] \"%s %s %s\" %s %u\n",
		    R->clientID, date, MethodNames[method],
		    ObjectNames[R->objectID], ProtocolNames[http], 
		    CodeNames[status],R->size);
	  else
	    fprintf(stdout,"%u - - [%s] \"%s %s %s\" %s -\n",
		    R->clientID, date, MethodNames[method],
		    ObjectNames[R->objectID], 
		    ProtocolNames[http], CodeNames[status]);
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
  /* status indicator */
  fprintf(stderr,"Printing Results\n");

  fprintf(stderr,"    Total Requests: %u\n", TotalRequests);
  fprintf(stderr,"       Total Bytes: %.0f\n", TotalBytes);
  fprintf(stderr,"Mean Transfer Size: %f\n", 
	  TotalBytes/(double)TotalRequests);
  fprintf(stderr,"     Max Client ID: %u\n", MaxClient);
  fprintf(stderr,"     Max Object ID: %u\n", MaxObject);

  fprintf(stderr,"        Start Time: %u\n", StartTime);
  fprintf(stderr,"       Finish Time: %u\n", LastTime);
  fprintf(stderr,"      Out of Order: %d\n", OutOfOrder);

}

/**************************************************************************/
/* INITIALIZE                                                             */
/**************************************************************************/
void Initialize(int argc, char **argv)
{
  int i, ID, value;
  char nextline[8192], name[8192];

  /* check command line */
  if(argc!=2)
    {
      fprintf(stderr,"usage: %s file_mappings\n", argv[0]);
      exit(-1);
    }

  /* status indicator */
  fprintf(stderr,"Initializing\n");

  /* determine which endian platform uses */
  Endian = CheckEndian();
  
  /* open input file of object mappings */
  if((object_mappings = fopen(argv[1], "r")) == NULL)
    {
      fprintf(stderr,"Error: failed to open %s\n", argv[1]);
      exit(-1);
    }
  
  /* initialize array of names */
  for(i=0; i<NUM_ENTRIES; i++)
    ObjectNames[i]=NULL;

  /* read in names */
  fgets(nextline, 8192, object_mappings);

  while(!feof(object_mappings))
    {
      value = sscanf(nextline, "%d %s", &ID, name);

      if((value == 2) && (ID >= 0) && (ID < NUM_ENTRIES))
	{
	  if((ObjectNames[ID] = (char *)malloc(strlen(name)+2))==NULL)
	    {
	      fprintf(stderr,"Error: malloc failed!\n");
	      exit(-1);
	    }

	  /* store the name */
	  strcpy(ObjectNames[ID],name);
	}

      else if((value == 2) && (ID >= NUM_ENTRIES))
	{
	  fprintf(stderr,"Error: Name array is too small! (%d)\n",
		  ID);
	  exit(-1);
	}

      fgets(nextline, 8192, object_mappings);
    }
}

/**************************************************************************/
/* TERMINATE                                                              */
/**************************************************************************/
void Terminate()
{
  int i;

  /* status indicator */
  fprintf(stderr,"Terminating\n");

  /* close data files */
  fclose(object_mappings);

  /* clean up array of names */
  for(i=0; i<NUM_ENTRIES; i++)
    {
      if(ObjectNames[i] != NULL)
	{
	  free(ObjectNames[i]);
	  ObjectNames[i]=NULL;
	}
    }
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

  return(0);
}
