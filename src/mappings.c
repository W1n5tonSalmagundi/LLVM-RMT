#include <stdio.h>
#include <stdlib.h>
#include <regex.h>
#include <string.h>
#include <stdlib.h>
#include <fnmatch.h>
#include <assert.h>

#include "mappings.h"

regex_t map_reg; 

void parse_prep(){
  int ret = regcomp(&map_reg,
		"\\[\\s*(dir|file|func|class)=(\\S+)\\s+mapTo=(\\S+)\\s*\\]",
		REG_EXTENDED);
  if (ret != 0) { fprintf(stderr, "regcomp failed\n"); exit(ret); }
}

struct mapping parse_line(char *line){
  struct mapping sret; int ret;
  
  regmatch_t matches[4];
  ret = regexec(&map_reg, line, 4, matches, REG_EXTENDED);
  if (ret == REG_NOMATCH) {
    sret.type = INVALID_MAPPING; return sret;
  }

  if (matches[1].rm_so == -1) {
    fprintf(stderr, "Invalid mapping, expected pattern description "
	    "type of 'func', 'class', 'file', or 'dir': %s", line);
    sret.type = INVALID_MAPPING; return sret;
  }

  char *mapping_type = line + matches[1].rm_so;
  // To mask the equal sign in the declaration and terminate the str in mapping_type
  line[matches[1].rm_eo] = '\0';
  // TODO: CHECK, if this is to be used with methods as well
  // ...and if this is the exact correct syntax.
  if ( strcmp(mapping_type, "func") == 0) { sret.type = FUNCTION_MAPPING; }
  else if ( strcmp(mapping_type, "class") == 0 ) { sret.type = CLASS_MAPPING; }
  else if ( strcmp(mapping_type, "file") == 0 ) { sret.type = SRC_FILE_MAPPING; }
  else { sret.type = SRC_DIR_MAPPING; }
  
  if (matches[2].rm_so == -1) {
    fprintf(stderr, "Invalid mapping, expected %s pattern specification: %s", mapping_type, line);
    sret.type = INVALID_MAPPING; return sret;
  }

  if (matches[3].rm_so == -1) {
    fprintf(stderr, "Invalid mapping, expected a system name to be specified: %s", line);
    sret.type = INVALID_MAPPING; return sret;
  }
  
  char *mapping_pattern = line + matches[2].rm_so;
  int pattern_len = matches[2].rm_eo - matches[2].rm_so;
  mapping_pattern[pattern_len] = '\0';
  sret.pattern = mapping_pattern;

  char *system_name = line + matches[3].rm_so;
  int sysname_len = matches[3].rm_eo - matches[3].rm_so;
  system_name[sysname_len] = '\0';
    sret.sys_name = system_name;
  
  return sret;
}

void init_mappings(struct mappings *ms){
  // TODO: Refactor magic number
  int init_size = 10;
  for (int i = 0; i < 5; i++) {
    ms->sizes[i] = init_size;
    ms->elems[i] = 0;
    ms->m[i] = calloc(init_size, sizeof(struct mapping));
  }
}

void insert_mapping(struct mappings *ms, struct mapping m){
  int i = m.type;
  assert(i != INVALID_MAPPING);
  
  if (ms->sizes[i] == ms->elems[i]) {
    ms->m[i] = realloc(ms->m[i], 2 * ms->sizes[i] * sizeof(struct mapping));
    if (ms->m[i] == NULL) { perror("realloc"); exit(1); }
    ms->sizes[i] *= 2;
  }
  ms->m[i][ms->elems[i]++] = m;
}

char *matches_mapping(const char *name, struct mappings *mappings, int type){
  struct mapping *ms = mappings->m[type];
  int n = mappings->elems[type];
  
  for (int i = 0; i < n; i++) {
    int ret = fnmatch(ms[i].pattern, name, 0);
    switch (ret) {
    case 0: { return ms[i].sys_name; break; }
    case FNM_NOMATCH: { continue; break; }
    default:
      fprintf(stderr,
	      "fnmatch error on pattern \"%s\" with name \"%s\"",
	      ms[i].pattern, name);
      return NULL;
    }
  }
  
  return NULL;
}
