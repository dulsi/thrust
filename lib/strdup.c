
/* Written by Peter Ekberg, peda@lysator.liu.se */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <string.h>

char *
strdup(const char *s)
{
  char *buf;
  size_t len;

  len = strlen(s)+1;
  buf = (char *)malloc(sizeof(char) * len);

  if(buf!=NULL)
    memcpy((void *)buf, (const void *)s, len);

  return(buf);
}
