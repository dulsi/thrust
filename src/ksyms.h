typedef struct {
	char **table;
	int size;
} syms_entry;

extern syms_entry syms[];

struct syn {
	char *synonym;
	char *official_name;
};
extern struct syn synonyms[];

extern const int syms_size;
extern const int syn_size;

extern int syms_iso8859(int i);
extern void set_charset(char *name);

/* this is a temporary ugly kludge */

extern char *latin2_syms[];
extern char *latin3_syms[];
extern char *latin4_syms[];
