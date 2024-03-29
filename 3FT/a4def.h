/*--------------------------------------------------------------------*/
/* a4def.h                                                            */
/* Authors: Michael Garcia and Ellen Su                               */
/*--------------------------------------------------------------------*/

#ifndef A4DEF_INCLUDED
#define A4DEF_INCLUDED

/* Return statuses */
enum { SUCCESS,
       INITIALIZATION_ERROR, PARENT_CHILD_ERROR , ALREADY_IN_TREE,
       NO_SUCH_PATH, CONFLICTING_PATH, NOT_A_DIRECTORY, NOT_A_FILE,
       MEMORY_ERROR
};

/* In lieu of a proper boolean datatype */
enum bool { FALSE, TRUE };
/* Declaration of boolean datatype */
typedef enum bool boolean;

/* Declares if Node is a file or directory. */
enum type { DIRECTORY, FT_FILE };
/* Declaration of nodeType datatype */
typedef enum type nodeType;

#endif
