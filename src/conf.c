
/* Written by Peter Ekberg, peda@lysator.liu.se */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "compat.h"
#include "thrust_t.h"
#include "keyboard.h"
#include "conf.h"
#include "font5x5.h"
#include "thrust.h"
#include "gr_drv.h"
#include "graphics.h"

#ifdef _WIN32
#define mkdir(x,y) mkdir(x)
#endif

char *conffield[5] = {
  "counterclockwise",
  "clockwise",
  "thrust",
  "fire",
  "pickup"
};

int
getscancode(int old, int x, int y)
{
  int result;
  static char str[40];
  int oldcolor;

  oldcolor=chcolor;
  sprintf(str, "%s", keystring(old));
  printgs(x, y, str);

  displayscreen(0UL);
  result = getonemultiplekey();
  if(!strcasecmp("Escape", keystring(result)))
    result = old;
  chcolor=BLACK;
  printgs(x, y, str);
  chcolor=oldcolor;
  sprintf(str, "%s", keystring(result));
  printgs(x, y, str);

  return(result);
}

#define DESCRIPTIONS (5)

void
conf()
{
  static char *descriptions[DESCRIPTIONS] = {
    "Turn left",
    "Turn right",
    "Thrust",
    "Fire",
    "Pickup & Shield"
  };
  int i, end;
  char *keyname;

  chcolor = HIGHLIGHT;
  gcenter(45, "Configuration");
  chcolor = TEXTCOLOR;
  for(i=0; i<DESCRIPTIONS; i++) {
    printgs(160-gstrlen(descriptions[i]), 65+i*8, descriptions[i]);
    printgs(161, 65+i*8, ":");
    printgs(167, 65+i*8, keystring(scancode[i]));
  }
  gcenter(125, "Press Enter to change a key");
  gcenter(132, "(don't use Q or P, these are taken)");
  gcenter(139, "Use Cursor Up/Down to move around");
  gcenter(148, "Press Escape for the main menu");

  i=0;
  end=0;
  chcolor = HIGHLIGHT;
  printgs(160-gstrlen(descriptions[i]), 65+i*8, descriptions[i]);
  chcolor = TEXTCOLOR;

  fade_in(0UL);

  do {
    keyname = keystring(getonemultiplekey());

    if(!strcasecmp("Cursor Up", keyname)
       || !strcasecmp("Keypad Up", keyname)
       || !strcasecmp("Up", keyname)
       || !strcasecmp("KP Up", keyname)) {
      printgs(160-gstrlen(descriptions[i]), 65+i*8, descriptions[i]);
      i=(i+DESCRIPTIONS-1)%DESCRIPTIONS;
    }

    if(!strcasecmp("Cursor Down", keyname)
       || !strcasecmp("Keypad Down", keyname)
       || !strcasecmp("Down", keyname)
       || !strcasecmp("KP Down", keyname)) {
      printgs(160-gstrlen(descriptions[i]), 65+i*8, descriptions[i]);
      i=(i+1)%DESCRIPTIONS;
    }

    if(!strcasecmp("Enter", keyname)
       || !strcasecmp("Keypad Enter", keyname)
       || !strcasecmp("KP Enter", keyname)
       || !strcasecmp("Return", keyname)) {
      printgs(160-gstrlen(descriptions[i]), 65+i*8, descriptions[i]);

      chcolor = HIGHLIGHT;
      scancode[i]=getscancode(scancode[i], 167, 65+i*8);
      printgs(160-gstrlen(descriptions[i]), 65+i*8, descriptions[i]);
      chcolor = TEXTCOLOR;

      printgs(167, 65+i*8, keystring(scancode[i]));
      displayscreen(0UL);
    }

    if(!strcasecmp("Escape", keyname)
       || !strcasecmp("Q", keyname)) {
      end=1;
    }

    chcolor = HIGHLIGHT;
    printgs(160-gstrlen(descriptions[i]), 65+i*8, descriptions[i]);
    chcolor = TEXTCOLOR;

    displayscreen(0UL);
  } while(!end);

  chcolor = TEXTCOLOR;

  writekeys();

  fade_out(0UL);
}

static char *
get_combine_path(char *dirs, char *subdir)
{
  struct stat statbuf;
  char *result = malloc(strlen(dirs) + strlen(subdir) + 2);
  char *end = dirs;
  do
  {
    int i;
    for (i = 0; (*end != ':') && (*end != 0); i++, end++)
      result[i] = *end;
    if (result[i - 1] != '/')
      result[i++] = '/';
    strcpy(result + i, subdir);
    if (0 == stat(result, &statbuf))
    {
      return result;
    }
    while (*end == ':')
      end++;
  } while (*end);
  strcpy(result, "./");
	return result;
}

char *
get_data_path()
{
#ifdef WIN32
	char *file_path = strdup("./");
#else
#ifdef DATA_DIR
	char *file_path = DATA_DIR;
	char *data_home = getenv("XDG_DATA_DIRS");
	if (data_home)
		file_path = data_home;
	file_path = get_combine_path(file_path, "inertiablast/");
#else
	char *file_path = strdup("./");
#endif
#endif
	return file_path;
}

char *
get_user_path()
{
  static char userPath[255] = "";
  struct stat statbuf;
  if (userPath[0] == 0) {
#ifdef WIN32
    strcpy(userPath, "save/");
#else
    //Temp variable that is used to prevent NULL assignement.
    char* env;

    //First get the $XDG_DATA_HOME env var.
    env=getenv("XDG_DATA_HOME");
    //If it's null set userPath to $HOME/.local/share/.
    if(env!=NULL){
      strcpy(userPath, env);
    }
    else {
      strcpy(userPath, getenv("HOME"));
      strcat(userPath, "/.local/share");
    }
    strcat(userPath, "/inertiablast/");
#endif
    if (0 != stat(userPath, &statbuf))
    {
      int len = strlen(userPath);
      for (int i = 1; i < len; i++)
      {
        if (userPath[i] == '/')
        {
          userPath[i] = 0;
          if (0 != stat(userPath, &statbuf))
          {
            mkdir(userPath, S_IRWXU);
          }
          userPath[i] = '/';
        }
      } 
    }
  }
  return userPath;
}

char *
getuserfile(char *name)
{
  char *home;
  char *thrustrc;

  home = get_user_path();
  if(home==NULL)
    home = "";

  thrustrc = malloc(strlen(home) + strlen(name) + 2);
  if(thrustrc == NULL) {
    printf("Out of memory when trying to read .thrustrc.\n");
    return NULL;
  }

  strcpy(thrustrc, home);
#if defined(_WIN32)
  if(thrustrc[0]) {
    unsigned int i;

    for(i=0; i<strlen(thrustrc); i++)
      thrustrc[i] = thrustrc[i]=='/' ? '\\' : thrustrc[i];
    if(thrustrc[strlen(thrustrc)-1]!='\\')
      strcat(thrustrc, "\\");
  }
  strcat(thrustrc, name);
#else
  if(thrustrc[0])
    if(thrustrc[strlen(thrustrc)-1]!='/')
      strcat(thrustrc, "/");
  strcat(thrustrc, name);
#endif

  return thrustrc;
}

char *
getthrustrc(void)
{
  return getuserfile("thrustrc");
}

char *underscore(char *keystring)
{
  static char keybuffer[100];
  int i;

  if(keystring == NULL)
    return NULL;

  strncpy(keybuffer, keystring, 99);
  keybuffer[99] = '\0';

  for(i=0; i<strlen(keybuffer); i++)
    if(keybuffer[i] == ' ')
      keybuffer[i] = '_';

  return keybuffer;
}

void writekeys(void)
{
  char *thrustrc;
  char *thrustrctmp;
  char *keyboarddriver;
  FILE *f;
  FILE *g;
  int rows=0;
  int res;
  ui8 row[256], driver[256], field[256], value[256];
  int found[5] = {0, 0, 0, 0, 0};

  thrustrc = getthrustrc();

  if(thrustrc == NULL)
    return;

  f = fopen(thrustrc, "rt");
  if(f == NULL) {
    f = fopen(thrustrc, "wt");
    if (f == NULL) {
#if defined(_WIN32)
      strcpy(thrustrc, "thrustrc");
      f = fopen(thrustrc, "rt");
      if(f == NULL) {
        free(thrustrc);
        return;
      }
#else
      free(thrustrc);
      return;
#endif
    }
  }

  thrustrctmp = strdup(thrustrc);
  if(thrustrctmp == NULL) {
    fclose(f);
    free(thrustrc);
    return;
  }

  thrustrctmp[strlen(thrustrctmp) - 2] = 't';
  thrustrctmp[strlen(thrustrctmp) - 1] = 'p';

  g = fopen(thrustrctmp, "wt");
  if(g == NULL) {
    fclose(f);
    free(thrustrctmp);
    free(thrustrc);
    return;
  }

  keyboarddriver = keyname();

  while(!feof(f)) {
    rows++;
    if(fgets(row, 255, f) == NULL) {
      if (ferror(f)) {
        perror("Error while parsing row %d of \"%s\"\n");
        break;
      }
      else
        break;
    }
    if(row[0] == '\0')
      break;
    if(!feof(f)) {
      if(row[strlen(row)-1] != '\n') {
        printf("Row %d of \"%s\" is too long.\n", rows, thrustrc);
        break;
      }
      else
        row[strlen(row)-1] = '\0';
    }

    if(row[0] == '#')
      res = 0;
    else
      res = sscanf(row, "%[^-]-%s %s", driver, field, value);

    if(res>=3 && !strcasecmp(driver, keyboarddriver)) {
      int foundIt = 0;
      for (int i = 0; i < 5; i++) {
        if(!strcasecmp(field, conffield[i])) {
          fprintf(g, "%s-%-18s %s\n", driver, field, underscore(keystring(scancode[i])));
          foundIt = 1;
          found[i] = 1;
          break;
        }
      }
      if (foundIt)
        continue;
    }

    fprintf(g, row);
    fprintf(g, "\n");
  }
  for (int i = 0; i < 5; i++) {
    if(!found[i]) {
      fprintf(g, "%s-%-18s %s\n", keyboarddriver, conffield[i], underscore(keystring(scancode[i])));
    }
  }

  fclose(f);
  fclose(g);

  remove(thrustrc);
  rename(thrustrctmp, thrustrc);

  free(thrustrc);
  free(thrustrctmp);
}

void initkeys(void)
{
  char *thrustrc;
  char *keyboarddriver;
  FILE *f;
  int rows=0;
  int res;
  ui8 row[256], driver[256], field[256], value[256];

  thrustrc = getthrustrc();
  if(thrustrc == NULL)
    return;

  f = fopen(thrustrc, "rt");
  if(f == NULL) {
#if defined(_WIN32)
    strcpy(thrustrc, "thrustrc");
    f = fopen(thrustrc, "rt");
    if(f == NULL) {
      free(thrustrc);
      return;
    }
#else
    free(thrustrc);
    return;
#endif
  }

  keyboarddriver = keyname();

  while(!feof(f)) {
    rows++;
    if(fgets(row, 255, f) == NULL) {
      if(ferror(f)) {
        perror("Error while parsing row %d of \"%s\"\n");
        break;
      }
      else
        row[0] = '\0';
    }
    if(!feof(f)) {
      if(row[strlen(row)-1] != '\n') {
        printf("Row %d of \"%s\" is too long.\n", rows, thrustrc);
        break;
      }
      else
        row[strlen(row)-1] = '\0';
    }
    if(row[0] != '#') {
      res = sscanf(row, "%[^-]-%s %s", driver, field, value);
      if(res==2)
        printf("Syntax error in row %d of \"%s\".\n", rows, thrustrc);
      else if(res>=3) {
        if(!strcasecmp(driver, "GGI")
           || !strcasecmp(driver, "SVGA")
           || !strcasecmp(driver, "SDL")
           || !strcasecmp(driver, "X11")
           || !strcasecmp(driver, "Win32")) {
          if(!strcasecmp(driver, keyboarddriver)) {
            int foundIt = 0;
            for (int i = 0; i < 5; i++) {
              if(!strcasecmp(field, conffield[i])) {
                scancode[i] = keycode(value);
                foundIt = 1;
                break;
              }
            }
            if (!foundIt) {
              printf("Illegal keyboard field \"%s\" specified in row %d.\n",
               field, rows);
            }
          }
        }
        else {
          printf("Illegal keyboard driver \"%s\" specified in row %d.\n",
           driver, rows);
        }
      }
    }
  }

  fclose(f);
  free(thrustrc);
}
