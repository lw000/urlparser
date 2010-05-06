#include <stdio.h>
#include <regex.h>
#include <strings.h>
#include <stdlib.h>

#include "urlparser.h"

/* 
  This library implements the following regular expression from    
  http://tools.ietf.org/html/rfc3986

  /^(([^:\/?#]+):)?(\/\/([^\/?#]*))?([^?#]*)(\?([^#]*))?(#(.*))?/
    12              3   4           5       6  7        8 9
*/

int parseURL(const char *url, URL *storage) {
  URLGroup groups[10];
  const char *c = url;

  bzero(groups, sizeof(URLGroup)*10);

  if(!c || *c == '\0') {
    return 1;
  } 

  /* URLGroups 1 and 2
  */
  while(*c != ':' && *c != '/' && *c != '?' && *c != '#' && *c != '\0') {
    groups[1].end++;
    groups[2].end++;
    c++;
  }
  if(*c == ':') {
    groups[1].end++;
    c++;
  } else {
    groups[1].start = groups[2].start = 0;
    groups[1].end   = groups[2].end   = 0;
    c = url;
  }

  /* URLGroups 3 and 4 
  */
  groups[3].end  = groups[3].start = groups[1].end; 
  groups[4].end  = groups[4].start = groups[1].end; 

  if(*c == '/' && *(c+1) == '/') {
    groups[4].start += 2;
    groups[4].end += 2;
    c += 2; 
    while(*c != '/' && *c != '?' && *c != '#' && *c != '\0') {
      groups[3].end++;
      groups[4].end++;
      c++;
    }
  }
  /* URLGroup 5
  */ 
  groups[5].start = groups[5].end = groups[4].end; 
  while(*c != '?' && *c != '#' && *c != '\0') {
    groups[5].end++;
    c++;
  }

  /* URLGroups 6 and 7
  */ 
  groups[6].end  = groups[6].start = groups[5].end; 
  groups[7].end  = groups[7].start = groups[5].end; 
  if(*c == '?') {
    groups[7].start++;
    groups[7].end++;
    groups[6].end++;
    c++;
    while(*c != '#' && *c != '\0') {
      groups[6].end ++;
      groups[7].end ++;
      c++;
    } 
  } 

  /* URLGroups 8 and 9
  */
  groups[8].end  = groups[8].start = groups[7].end; 
  groups[9].end  = groups[9].start = groups[7].end; 
  if(*c == '#') {
    groups[9].start++;
    groups[9].end++;
    groups[8].end++;
    c++;
    while(*c != '\0' && *c != '\0') {
      groups[8].end ++;
      groups[9].end ++;
      c++;
    } 
  } 

  storage->scheme    = groups[2];
  storage->authority = groups[4];
  storage->path      = groups[5];
  storage->query     = groups[7];
  storage->fragment  = groups[9];
  return 0;
}


char *readURLField(const char* url, URLGroup field) {
  char *response;

  if(field.end - field.start <= 0) 
    return 0;

  response = (char*) malloc((sizeof(char)) * (field.end - field.start + 1));
  bcopy(url + field.start, response, field.end - field.start);
  response[field.end - field.start] = '\0';

  return response;
}