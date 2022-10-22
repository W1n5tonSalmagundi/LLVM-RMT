#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <getopt.h>
#include <limits.h>
#include <string.h>
#include <ftw.h>
#include <sys/types.h>
#include <sys/stat.h>

//#include "mappings.h"
#include "matchings.h"



#define INTERACTIVE_MODE (( opt_bits & (1 << 0)))
/* The significance of this is more upfront work in parsing
   mappings, and arranging data, to make it both faster to access
   and compare things, but more importantly, to arrange it for serialization
   and loading directly from a file.

   ...When we actually implement that.*/
#define DO_CACHE (( opt_bits & (1 << 1)))
int opt_bits;


/* struct open_project { */
/*   int num_files; */
/*   char **source_names, **out_names; */
/* }; */



/* This acts like our own way to pass additional arguments across nftw calls
   while still using a function type that can still be used by nftw.

   We manage it just just like how C manages the program stack, restoring as needed after
   nftw calls within nftw wrappers. */
/* struct ftw_extra_args { */
/*   struct mappings ms; */
/* }; */

int main(int argc, char *argv[]){
  int ret;
  const char *opt_str = "sic:";
  char *mappings_file_path = "./test-mappings.config";
  opt_bits = 0;
  // TODO: Add long options support so you can specify cached file name and other things

  while ( (ret = getopt(argc, argv, opt_str) ) != -1 ) {
    switch (ret) {
    case '?': {
      fprintf(stderr,
	      // The next two lines are one line thanks to
	      // C's silent string concatentation.
	      "%s: invalid option -- '%s' \n"
	      "Try the '-h' option for more information.",
	      argv[0], argv[optind]);
      break;
    }
    case 's': { opt_bits |= 1; break; }
    case 'i': { opt_bits |= 2; break; }
    case 'c': { mappings_file_path = optarg; }
    }
  }

  printf("Target config file: %s", mappings_file_path);
  FILE *mappings_file = fopen(mappings_file_path, "r");
  if (mappings_file == NULL) {
    perror("fopen"); exit(1);
  }

  parse_prep();

  // A block of memory for reading in strings out of the mapping file.
  // It is to double in size every time it cannot fit in a new line.
  // This will probably be wrapped in a struct later.
  // Realloc is such a nice syscall.
  char *slab_head;
  int str_offset = 0;
  int slab_len = MAPPING_LINE_MAX_LEN * 4;
  
  if ( (slab_head = calloc(slab_len, sizeof(char))) == NULL) {
    perror("malloc"); exit(1);
  }

  struct mappings ms; init_mappings(&ms);

  while ( (fgets(slab_head + str_offset, MAPPING_LINE_MAX_LEN, mappings_file)) != NULL) {
    // Reallocating more memory for the next iteration in advance so we can continue to not copy strings from a separate buffer.
    if ( slab_len <= (str_offset + MAPPING_LINE_MAX_LEN + 1) ) {
      slab_head = realloc(slab_head, slab_len * 2);
      if (slab_head == NULL) { perror("realloc"); exit(1); }
      slab_len *= 2;
    }

    // Saves the string's length before the parse_line call clobbers it with regex extraction.
    int s_len = strlen(slab_head + str_offset) + 1;
    struct mapping m = parse_line(slab_head + str_offset);
    // fprintf(stderr, "m: %d, %s, %s \n", m.type, m.pattern, m.sys_name);

    if (m.type != INVALID_MAPPING) {
      insert_mapping(&ms, m);
      str_offset += s_len;
    }
  }

  struct stat f_sb;
  ret = stat(argv[optind], &f_sb);
  if (ret != 0) {
    perror("stat"); return 1;
  }

  
  if ( ! S_ISDIR(f_sb.st_mode) ) {
    fprintf(stderr, "Given path is not a directory: %s", argv[optind]);
    return 1;
  }

  /*
  Chdir'ing minimizes our path name length in the nftw,
  helping us work around the fact that PATH_MAX is fundamentally broken.
  https://insanecoding.blogspot.com/2007/11/pathmax-simply-isnt.html
  
  It also lets us fnmatch directly on nftw's fpath argument
  without extracting a relative path from the base directory of a full absolute path.
  */
  ret = chdir(argv[optind]);
  if ( ret != 0 ) {
    perror("chdir"); return 1;
  }

  match_in_dir(".", &ms);
  return 0;
}
