#include "matchings.h"

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <ftw.h>

void init_matchings(struct matchings *ms){
  ms->load_factor = MTCHS_DEF_LOAD;
  ms->sizes = MTCHS_DEF_SIZE;
  ms->elems = 0;
  ms->m = calloc(ms->sizes, sizeof(struct matching));
}

double hash_matching(struct matching *m){
  int len = strlen(m->name);
  double ret_i = 0;

  for (int i = 0; i < len; i++) {
    ret_i += pow(m->name[i], i);
  }

  return ret_i;
}

double hash_matching2(struct matching *m){
  int len = strlen(m->name);
  double ret_i = 0;

  for (int i = 0; i < len; i++) {
    ret_i += pow(i, m->name[i]);
  }

  return ret_i;
}

void insert_matching(struct matchings *ms, struct matching *m){
  m->hash = hash_mapping(m);
  
  reinsert_matching(ms, m);

  if ((ms->sizes * ms->load_factor) < ++ms->elems) {
    reload_matchings(ms);
  } 
}

int reinsert_matching(struct matchings *ms, struct matching *m){
  unsigned int i, j, k;
  struct mapping *table = ms->m;

  j = 0;
  i = ((int) round( m->hash )) % (k = ms->sizes );
  if (table != 0) {
    double second_hash = hash_matching2(m);
    do {
      i = ((int) round( m->hash + j++ * second_hash )) % k;
    } while (table != 0);
  }

  memcpy(table + i, m, sizeof(struct matching));
}

void reload_matchings(struct matchings *ms){
  struct matchings new_ms;
  new_ms.sizes = ms->sizes;
  new_ms.elems = ms->elems;
  new_ms.load_factor = ms->load_factor;
  new_ms.m = calloc(ms->elems, sizeof(struct matching));

  for (int i = 0; i < ms->sizes; i++) {
    struct matching *m = ms->m + i;
    if (m->hash != 0) {
      reinsert_matching(&new_ms, m); 
    }
  }

  struct mapping *old_ms_table = ms->m;
  memmove(ms, &new_ms, sizeof(struct matchings));
  free(old_ms_table);
}

char *_match_in_dir(const char *fpath, const struct stat *stat_buf,
		  int typeflag, struct FTW *ftwbuf){

  switch (typeflag) {
  case FTW_F: {
    //fprintf(stdout, "fpath: %s \n", fpath);
    
    break;
  }
    
  case FTW_D: {

    // eargs.matchings[SRC_FILE] =
    // matching_mapping(fpath + 2, ms->m[SRC_FILE], ms->elems[SRC_FILE]);
    
    break;
  }
    
  case FTW_DNR: {
    const char *abs_path = realpath(fpath, NULL);
    if (abs_path == NULL) {
      perror("realpath");
      fprintf(stderr,
	      "realpath failed to resolve the unreadable"
	      "directory at relative path \"%s\"\n",
	      abs_path);
    } else {
      fprintf(stderr,
	      "The directory at \"%s\" cannot be read.\n", 
	      abs_path);
      free(abs_path);
    }
    fprintf(stderr, "Do you have rx permissions on the directory?\n");
  }
    
  default:
    break;
  }
}

char *match_in_dir(const char *dir_name, struct mappings *mappings){
  nftw(dir_name, _match_in_dir, 30, 0); 
}


/* int map_functions(const char *fpath, const struct stat *stat_buf, */
/* 		  int typeflag, struct FTW *ftwbuf){ */

/*   struct mappings *ms = &eargs.ms; */

/*   switch (typeflag) { */
/*   case FTW_F: { */
/*     //fprintf(stdout, "fpath: %s \n", fpath); */

/*     eargs.matchings[SRC_FILE] = */
/*       matching_mapping(fpath + 2, ms->m[SRC_FILE], ms->elems[SRC_FILE]); */
      
      
/*     if (eargs.matchings[SRC_FILE] != NULL) { */
/*       printf("Mapping pattern: %s \n", eargs.matchings[SRC_FILE]->pattern); */
/*       printf("To system named: %s \n\n", eargs.matchings[SRC_FILE]->sys_name); */
/*     } else { */
/*       printf("NULL matching.\n\n"); */
/*     } */
/*     // Get */

/*     // Final parse call */
    

/*     break; */
/*   } */
    
/*   case FTW_D: { */

/*     // eargs.matchings[SRC_FILE] = */
/*     // matching_mapping(fpath + 2, ms->m[SRC_FILE], ms->elems[SRC_FILE]); */
    
/*     break; */
/*   } */
    
/*   case FTW_DNR: { */
/*     const char *abs_path = realpath(fpath, NULL); */
/*     if (abs_path == NULL) { */
/*       perror("realpath"); */
/*       fprintf(stderr, */
/* 	      "realpath failed to resolve the unreadable" */
/* 	      "directory at relative path \"%s\"\n", */
/* 	      abs_path); */
/*     } else { */
/*       fprintf(stderr, */
/* 	      "The directory at \"%s\" cannot be read.\n",  */
/* 	      abs_path); */
/*       free(abs_path); */
/*     } */
/*     fprintf(stderr, "Do you have rx permissions on the directory?\n"); */
/*   } */
    
/* default: */
/*     break; */
/*   } */

/*   return 0; */
/* } */
