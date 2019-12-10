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
/* ENDIAN.C                                                               */
/**************************************************************************/
#include "endian.h"

/**************************************************************************/
/* TWIDDLE                                                                */
/**************************************************************************/
uint32_t twiddle(uint32_t in)
{
   return(( in >> 24 )  | 
          (( in & ((1<<8)-1) << 16) >> 8 ) |
          (( in & ((1<<8)-1) <<  8) << 8 ) |  
          (( in & ((1<<8)-1)) << 24));
}

/**************************************************************************/
/* CHECK ENDIAN                                                           */
/**************************************************************************/
int CheckEndian()
{
  int type = NO_ENDIAN;

  union 
  {
    int b;
    char a[4];
  } u;

  u.b=1;
  if (u.a[3]==1) 
    {
      /* big endian */
      type = BIG_ENDIAN;
    }
  else if (u.a[0]==1) 
    {
      /* little endian */
      type = LITTLE_ENDIAN;
    }
   else 
     {
       /* unknown endian */
       fprintf(stderr, "Error: Unknown Endian!\n");
       exit(-1);
     }

  return(type);
}
  
/**************************************************************************/
/* LITTLE ENDIAN REQUEST                                                  */
/**************************************************************************/
void LittleEndianRequest(const struct request *BER,
			 struct request *LER)
{
  LER->timestamp = twiddle(BER->timestamp);
  LER->clientID = twiddle(BER->clientID);
  LER->objectID = twiddle(BER->objectID);
  LER->size = twiddle(BER->size);
  LER->method = BER->method;
  LER->status = BER->status;
  LER->type = BER->type;
  LER->server = BER->server;
}

