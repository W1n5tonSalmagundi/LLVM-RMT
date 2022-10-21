#include <regex.h>

#define MAPPING_LINE_MAX_LEN 100
#define SYSTEM_MAX_NAME_LEN 50

#define MAPPING_NUM_TYPES 4
enum mapping_types {INVALID_MAPPING=-1, FUNCTION_MAPPING=0, CLASS_MAPPING, SRC_FILE_MAPPING, SRC_DIR_MAPPING};


struct mapping {
  char type, *pattern, *sys_name;
};

regex_t map_reg; void parse_prep();

struct mapping parse_line(char *line);

// -----------------------------------------------------------------------------

#define MAPPINGS_INIT_SIZE 50

/*
  It is possible to condense all the mappings that would be produced from the configuration file into a specialized directed acyclic word graph.

 It would even be plausible to implement such a DAWG that references into one continuous block of memory that indexes each node, and they all refer to each other within the block.

 It would also be absolutely awful to implement.
*/

// This is just a dynamic expanding list implementation that bundles all the info
// for the size and element count of the arrays together.
// Speed of random access outdoes that of linked lists, especially on modern hardware.
struct mappings {
  int sizes[MAPPING_NUM_TYPES], elems[MAPPING_NUM_TYPES];
  struct mapping *m[MAPPING_NUM_TYPES];
};

void init_mappings(struct mappings *ms);
void insert_mapping(struct mappings *ms, struct mapping m);

char *matches_mapping(const char *name, struct mappings *mappings, int type);

