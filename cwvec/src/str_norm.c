#ifndef __STR_NORM_C
#define __STR_NORM_C

#include <wctype.h>
#include <wchar.h>
#include <locale.h>
#include <string.h>

#include <stdint.h>
#define u_int32_t uint32_t

#define NUMBER L"<num>" 
#define SPACE_IN_WORD L'_' 
#define MAX_WORD_LENGTH 1024

static const u_int32_t offsetsFromUTF8[6] = {
    0x00000000UL, 0x00003080UL, 0x000E2080UL,
    0x03C82080UL, 0xFA082080UL, 0x82082080UL
};

static const char trailingBytesForUTF8[256] = {
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, 3,3,3,3,3,3,3,3,4,4,4,4,5,5,5,5
};


/* 
 * Basic UTF-8 manipulation routines
 * by Jeff Bezanson
 * placed in the public domain Fall 2005

   conversions without error checking
   only works for valid UTF-8, i.e. no 5- or 6-byte sequences
   sz = dest size in # of wide characters (max sz-1 characters are converted)

   returns # characters converted
   dest will always be L'\0'-terminated.
   if sz = srcsz+1 (i.e. 4*srcsz+4 bytes), there will always be enough space.
*/
static int u8_toucs(u_int32_t *dest, int sz, char *src)
{
    u_int32_t ch;
    int nb;
    int i=0;

    while ((*src != 0) && (i < sz-1)) {
      ch = 0 ;
      nb = trailingBytesForUTF8[(unsigned char)*src];
      if (nb == 0) ch += (unsigned char)*src++;
      else if ((nb > 0) && (nb < 4) ) {
        ch += (unsigned char)*src++; 
        ch <<= 6;
      }
      ch -= offsetsFromUTF8[nb];
      dest[i++] = ch;
    }
    dest[i] = 0;
    return i;
}

/*
   sz = size of dest buffer in bytes

   returns # characters converted
   dest will only be '\0'-terminated if there is enough space. this is
   for consistency; imagine there are 2 bytes of space left, but the next
   character requires 3 bytes. in this case we could NUL-terminate, but in
   general we can't when there's insufficient space. therefore this function
   only NUL-terminates if all the characters fit, and there's space for
   the NUL as well.
   the destination string will never be bigger than the source string.
*/
static int u8_toutf8(char *dest, int sz, u_int32_t *src)
{
    u_int32_t ch;
    int i = 0;
    char *dest_end = dest + sz;

    while (src[i] != 0) {
        ch = src[i];
        if (ch < 0x80) {
            if (dest >= dest_end)
                return i;
            *dest++ = (char)ch;
        }
        else if (ch < 0x800) {
            if (dest >= dest_end-1)
                return i;
            *dest++ = (ch>>6) | 0xC0;
            *dest++ = (ch & 0x3F) | 0x80;
        }
        else if (ch < 0x10000) {
            if (dest >= dest_end-2)
                return i;
            *dest++ = (ch>>12) | 0xE0;
            *dest++ = ((ch>>6) & 0x3F) | 0x80;
            *dest++ = (ch & 0x3F) | 0x80;
        }
        else if (ch < 0x110000) {
            if (dest >= dest_end-3)
                return i;
            *dest++ = (ch>>18) | 0xF0;
            *dest++ = ((ch>>12) & 0x3F) | 0x80;
            *dest++ = ((ch>>6) & 0x3F) | 0x80;
            *dest++ = (ch & 0x3F) | 0x80;
        }
        i++;
    }
    if (dest < dest_end)
        *dest = '\0';
    return i;
}


/* Normalize the utf8 input parameter 'token' if NORMALIZATION flag is on */
char* normalize_token( char* token_src ) {
  wchar_t wtoken_src[MAX_WORD_LENGTH + 1] ;
  wchar_t wtoken_dst[MAX_WORD_LENGTH + 1] ;
  
  static size_t NUMBER_LENGTH = 0 ; 
  if (!NUMBER_LENGTH) NUMBER_LENGTH = wcslen(NUMBER) ;
  
  // convert from utf8 to ucs
  size_t wlen = u8_toucs((u_int32_t*)wtoken_src, MAX_WORD_LENGTH + 1, token_src) ;

  // lowercase and NUMBER replacement
  size_t state = 0, si = 0, di = 0, ni = 0;
  wchar_t wch ;
  while ( ( si <= wlen ) && ( di < MAX_WORD_LENGTH ) ) {
    wch = wtoken_src[si++] ;
    switch (state) {
      case 0 :
        if ( iswdigit( wch ) ) state = 1;
        else if ( iswspace( wch ) ) wtoken_dst[di++] = SPACE_IN_WORD ;
        else wtoken_dst[di++] = towlower( wch ) ;
        break ;
      case 1 : // digit state
        if ( !iswdigit( wch ) ) {
          ni = 0 ;
          while( ( di < MAX_WORD_LENGTH ) && ( ni < NUMBER_LENGTH ) )
            wtoken_dst[di++] = NUMBER[ni++] ;
          si-- ; // go one letter back to process wtoken_src[i] whic is not a digit
          state = 0 ;
        }
        break ;
    }
  }
  wtoken_dst[di] = L'\0' ;

  // convert back to utf8
  char token_dst[MAX_WORD_LENGTH] ;
  u8_toutf8(token_dst, MAX_WORD_LENGTH + 1, (u_int32_t*)wtoken_dst) ;
  return strdup(token_dst) ;
}
#endif

