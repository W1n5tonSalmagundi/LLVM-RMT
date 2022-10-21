#include "mappings.h"

#define MTCH_NAME_MAX_LEN 127
/* #define MTCH_NAME_BLK_SIZE (sizeof(long)/sizeof(char)) */
/* #define MTCH_L_TAB_LEN (MTCH_NAME_MAX_LEN / MTCH_NAME_BLK_SIZE) */
/* #define MTCH_L_TAB_TRAIL_LEN ( MTCH_NAME_MAX_LEN - MTCH_L_TAB_LEN * MTCH_NAME_BLK_SIZE) */
#define MTCHS_DEF_LOAD 0.5
#define MTCHS_DEF_SIZE 100

struct matching {
  double hash;
  char name[MTCH_NAME_MAX_LEN + 1], *system1, *system2;
};

struct matchings {
  double load_factor;
  int sizes, elems;
  struct matching *m;
};

void init_matchings(struct matchings *ms);
  
double hash_matching(struct matching *m);
double hash_matching2(struct matching *m);
void insert_matching(struct matchings *ms, struct matching *m);
void reload_matchings(struct matchings *ms);

char *matches_a_mapping(const char *func_name, struct mappings mappings, int type);

char *match_in_dir(const char *dir_name, struct mappings *mappings);
char *match_in_file(const char *file_name, struct mappings *mappings);

char *match_on_methods(const char *meth_name, struct mappings *mappings);
char *match_on_func(const char *func_name, struct mappings *mappings);

char *match_on_name(const char *name, struct mappings *mappings);

