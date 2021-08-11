
/* Written by Peter Ekberg, peda@lysator.liu.se */

#ifdef HAVE_CONFIG_H
#include "../src/config.h"
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <sys/types.h>
#include <stdio.h>

#define BUFSIZE (2050)

void quit(char *str);
int writebuf(int flag);

char buf[BUFSIZE];

void
quit(char *str)
{
  perror(str);
  exit(1);
}

int
writebuf(int flag)
{
  int stat,len,i;
  char out[3];
  
  if(!flag) {
#if PRINTF_RETURN >= 0
    if(
#endif
       printf("\"")
#if PRINTF_RETURN == 1
                    != 1
#endif
#if PRINTF_RETURN >= 0
                        )
      return(1)
#endif
	;

    len=strlen(buf);
    if(len)
      if(buf[len-1]=='\n')
	buf[--len]=0;
    for(i=0; i<len; i++) {
      switch(buf[i]) {
      case '\t':
	strcpy(out,"\\t");
	break;
      case '\b':
	strcpy(out,"\\b");
	break;
      case '\f':
	strcpy(out,"\\f");
	break;
      case '\'':
	strcpy(out,"\\\'");
	break;
      case '\"':
	strcpy(out,"\\\"");
	break;
      case '\\':
	strcpy(out,"\\\\");
	break;
      default:
	out[0]=buf[i];
	out[1]=0;
      }
      stat=fputs(out,stdout);
      if(stat==EOF)
	return(1);
      if(stat<0)
	return(1);
    }
#if PRINTF_RETURN >= 0
    if(
#endif
       printf("\"")
#if PRINTF_RETURN == 1
                    != 1
#endif
#if PRINTF_RETURN >= 0
                        )
      return(1)
#endif
	;
  }
  else {
#if PRINTF_RETURN >= 0
    if(
#endif
       printf("NULL")
#if PRINTF_RETURN == 1
                      != 4
#endif
#if PRINTF_RETURN >= 0
                          )
      return(1)
#endif
	;
  }

  return(0);
}

int main(int argc, char *argv[])
{
  char *stat;
  int end=0;

  if(argc!=2) {
    fprintf(stderr, "%s: Usage '%s variable_name'\n",
	    argv[0],
	    argv[0]);
    exit(1);
  }
#if PRINTF_RETURN >= 0
  if(
#endif
     printf("\n#include <stddef.h>\n\nchar *%s[] = {\n", argv[1])
#if PRINTF_RETURN == 1
                                                   != (int)(35 + strlen(argv[1]))
#endif
#if PRINTF_RETURN >= 0
                                                                            )
    quit(argv[0])
#endif
      ;

  while(!end) {
    stat=fgets(buf, BUFSIZE, stdin);
    if(stat==NULL) {
      if(ferror(stdin))
	quit(argv[0]);
      end=1;
      if(writebuf(1))
	quit(argv[0]);
    }
    else {
      if(writebuf(0))
	quit(argv[0]);
#if PRINTF_RETURN >= 0
      if(
#endif
         printf(",\n")
#if PRINTF_RETURN == 1
                       != 2
#endif
#if PRINTF_RETURN >= 0
                           )
	quit(argv[0])
#endif
	  ;
    }
  }  
#if PRINTF_RETURN >= 0
  if(
#endif
     printf(" };\n")
#if PRINTF_RETURN == 1
                     != 4
#endif
#if PRINTF_RETURN >= 0
                         )
    quit(argv[0])
#endif
      ;

  return(0);
}
