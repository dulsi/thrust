
/* Written by Peter Ekberg, peda@lysator.liu.se */

#ifndef CONF_H
#define CONF_H

extern int scancode[];
extern char *keynames[];

#ifdef __STDC__
void initkeys(void);
void writekeys(void);
char *underscore(char *keystring);
char *get_data_path();
char *get_user_path();
char *getuserfile(char *name);
char *getthrustrc(void);
void conf(void);
int getscancode(int old, int x, int y);
#endif

#endif /* CONF_H */
