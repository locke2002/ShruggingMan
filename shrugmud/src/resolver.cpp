/****************************************************************************
 *                   ^     +----- |  / ^     ^ |     | +-\                  *
 *                  / \    |      | /  |\   /| |     | |  \                 *
 *                 /   \   +---   |<   | \ / | |     | |  |                 *
 *                /-----\  |      | \  |  v  | |     | |  /                 *
 *               /       \ |      |  \ |     | +-----+ +-/                  *
 ****************************************************************************
 * AFKMud Copyright 1997-2012 by Roger Libiez (Samson),                     *
 * Levi Beckerson (Whir), Michael Ward (Tarl), Erik Wolfe (Dwip),           *
 * Cameron Carroll (Cam), Cyberfox, Karangi, Rathian, Raine,                *
 * Xorith, and Adjani.                                                      *
 * All Rights Reserved.                                                     *
 *                                                                          *
 *                                                                          *
 * External contributions from Remcon, Quixadhal, Zarius, and many others.  *
 *                                                                          *
 * Original SMAUG 1.4a written by Thoric (Derek Snider) with Altrag,        *
 * Blodkai, Haus, Narn, Scryn, Swordbearer, Tricops, Gorog, Rennard,        *
 * Grishnakh, Fireblade, and Nivek.                                         *
 *                                                                          *
 * Original MERC 2.1 code by Hatchet, Furey, and Kahn.                      *
 *                                                                          *
 * Original DikuMUD code by: Hans Staerfeldt, Katja Nyboe, Tom Madsen,      *
 * Michael Seifert, and Sebastian Hammer.                                   *
 ****************************************************************************
 *                      External DNS Resolver Module                        *
 ****************************************************************************/

/***************************************************************************
 *                          SMC version 0.9.7b3                            *
 *          Additions to Rom2.3 (C) 1995, 1996 by Tom Adriaenssen          *
 *                                                                         *
 * Share and enjoy! But please give the original authors some credit.      *
 *                                                                         *
 * Ideas, tips, or comments can be send to:                                *
 *          tadriaen@zorro.ruca.ua.ac.be                                   *
 *          shadow@www.dma.be                                              *
 ***************************************************************************/

/*
locke2002 notes 2015.04.29
At first glance this code looks deliberately obtuse. It appears to place a
lot of trust in external libraries to perform as expected. However, I have
noticed some catches that will protect the local code from unexpected
behavior. For example, the input is converted to an int before it is 
passed into resolve_address, so that should limit its size to at least
the size of int as defined by compiled libraries. Next, it calls 
gethostbyaddr which if not NULL will pass the hostname into the custom
mudstrlcpy function, with a size limit of 256. mudstrlcpy will copy the
hostname into the parameter addr_str which should be stored in the stack
if my college degree is still rattling around in my brain somewhere. The
addr_str variable and the siz limit are both 256. So while the mudstrlcpy
function is coded in a strange way to fill the dst with nulls out
to the value of siz, that seemingly poor strategy shouldn't be causing
problems as addr_str should have enough space for the limit of 256.

I would be more comfortable replacing mudstrlcpy altogether with the
appropriate standard str copy for this scenario, but if one didn't want
to trust external libraries, but still trusts the compiler and also trusts
the binraries not to be modified after compilation, I don't currently see
a vulnerability.

That said, I do not fully grok this code and still do not trust it.

I have some theories, but I don't want to spoil it.
*/

#include <cstdio>
#include <cstdlib>
#include <cstring>
#if defined(__OpenBSD__) || defined(__FreeBSD__) || defined(__CYGWIN__)
#include <sys/types.h>
#include <sys/socket.h>
#endif
#include <netinet/in.h>
#include <netdb.h>

/*
 * Copy src to string dst of size siz.  At most siz-1 characters
 * will be copied.  Always NUL terminates (unless siz == 0).
 * Returns strlen(src); if retval >= siz, truncation occurred.
 *
 * Renamed so it can play itself system independent.
 * Samson 10-12-03
 */
size_t mudstrlcpy( char *dst, const char *src, size_t siz )
{
   register char *d = dst;
   register const char *s = src;
   register size_t n = siz;

   /*
    * Copy as many bytes as will fit 
    */
   if( n != 0 && --n != 0 )
   {
      do
      {
         if( ( *d++ = *s++ ) == 0 )
            break;
      }
      while( --n != 0 );
   }

   /*
    * Not enough room in dst, add NUL and traverse rest of src 
    */
   if( n == 0 )
   {
      if( siz != 0 )
         *d = '\0';  /* NUL-terminate dst */
      while( *s++ )
         ;
   }
   return ( s - src - 1 ); /* count does not include NUL */
}

char *resolve_address( int address )
{
   static char addr_str[256];
   struct hostent *from;
   int addr;

   if( ( from = gethostbyaddr( ( char * )&address, sizeof( address ), AF_INET ) ) != NULL )
   {
      mudstrlcpy( addr_str, strcmp( from->h_name, "localhost" ) ? from->h_name : "local-host", 256 );
   }
   else
   {
      addr = ntohl( address );
      snprintf( addr_str, 256, "%d.%d.%d.%d", ( addr >> 24 ) & 0xFF, ( addr >> 16 ) & 0xFF, ( addr >> 8 ) & 0xFF, ( addr ) & 0xFF );
   }
   return addr_str;
}

int main( int argc, char *argv[] )
{
   int ip;
   char *address;

   if( argc != 2 )
   {
      printf( "unknown.host\r\n" );
      exit( 0 );
   }

   ip = atoi( argv[1] );

   address = resolve_address( ip );

   printf( "%s\r\n", address );
   exit( 0 );
}
