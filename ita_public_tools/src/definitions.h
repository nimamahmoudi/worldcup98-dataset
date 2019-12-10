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
/* DEFINITIONS.H                                                          */
/* This header file contains the definitions used in statistics collection*/
/**************************************************************************/

#ifndef DEFINITIONS_DEF
#define DEFINITIONS_DEF

/**************************************************************************/
/* REGIONS                                                                */
/**************************************************************************/
#define SANTA_CLARA 0
#define PLANO 1
#define HERNDON 2
#define PARIS 3
#define NUM_OF_REGIONS 4

/**************************************************************************/
/* PROTOCOLS                                                              */
/**************************************************************************/
#define HTTP_09 0
#define HTTP_10 1
#define HTTP_11 2
#define HTTP_XX 3

#define NUM_OF_PROTOCOLS 4

/**************************************************************************/
/* METHODS                                                                */
/**************************************************************************/
#define GET 0
#define HEAD 1
#define POST 2
#define PUT 3
#define DELETE 4
#define TRACE 5
#define OPTIONS 6
#define CONNECT 7
#define OTHER_METHODS 8

#define NUM_OF_METHODS 9

/**************************************************************************/
/* STATUS CODES                                                           */
/**************************************************************************/
#define SC_100 0
#define SC_101 1
#define SC_200 2
#define SC_201 3
#define SC_202 4
#define SC_203 5
#define SC_204 6
#define SC_205 7
#define SC_206 8
#define SC_300 9
#define SC_301 10
#define SC_302 11
#define SC_303 12
#define SC_304 13
#define SC_305 14
#define SC_400 15
#define SC_401 16
#define SC_402 17
#define SC_403 18
#define SC_404 19
#define SC_405 20
#define SC_406 21
#define SC_407 22
#define SC_408 23
#define SC_409 24
#define SC_410 25
#define SC_411 26
#define SC_412 27
#define SC_413 28
#define SC_414 29
#define SC_415 30
#define SC_500 31
#define SC_501 32
#define SC_502 33
#define SC_503 34
#define SC_504 35
#define SC_505 36
#define OTHER_CODES 37

#define NUM_OF_CODES 38

/**************************************************************************/
/* FILE TYPES                                                             */
/**************************************************************************/
#define HTML 0
#define IMAGE 1
#define AUDIO 2
#define VIDEO 3
#define JAVA 4
#define FORMATTED 5
#define DYNAMIC 6
#define TEXT 7
#define COMPRESSED 8
#define PROGRAMS 9
#define DIRECTORY 10
#define ICL 11
#define OTHER_TYPES 12

#define NUM_OF_FILETYPES 13

/**************************************************************************/
/* ERROR FLAGS                                                            */ 
/**************************************************************************/
#define NO_SIZE 0xffffffff

/**************************************************************************/
/* BOOLEAN VALUES                                                         */
/**************************************************************************/
#define TRUE 1
#define FALSE 0

/**************************************************************************/
/* ENDIAN FLAGS                                                           */
/**************************************************************************/
#define NO_ENDIAN -1

#ifndef LITTLE_ENDIAN
#define LITTLE_ENDIAN 0
#endif

#ifndef BIG_ENDIAN
#define BIG_ENDIAN 1
#endif

/**************************************************************************/
/* MACRO DEFINITIONS                                                      */
/**************************************************************************/
#define MAXIMUM(a,b) (a>b)?a:b
#define MINIMUM(a,b) (a<b)?a:b

#endif /* DEFINITIONS_DEF */

