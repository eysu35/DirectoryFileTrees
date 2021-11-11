/*--------------------------------------------------------------------*/
/* ft.c                                                               */
/* Authors: Michael Garcia and Ellen Su                               */
/*--------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

#include "a4def.h"
#include "dynarray.h"
#include "ft.h"
#include "node.h"
#include "../2DT/checkerDT.h"

/*
  A File Tree is a representation of a hierarchy of directories and
  files: the File Tree is rooted at a directory, directories
  may be leaves or non-leaves, and files are always leaves. It is
  an AO with  3 state variables:
*/
/* a flag for if it is in an initialized state (TRUE) or not (FALSE) */
static boolean isInitialized;
/* a pointer to the root node in the hierarchy */
static Node_T root;
/* a counter of the number of nodes in the hierarchy */
static size_t count;


static Node_T FT_getEndofPathNode(char *path, Node_T curr) {
    Node_T found;
    size_t i;

    assert(curr != NULL);

    if(curr == NULL)
        return NULL;

    /* If query path and path to current node are equivalent (and the current
    node is a directory), return the currrent node. If they match and
    are not a directory, return NOT_A_DIRECTORY. */
    else if(!strcmp(path,Node_getPath(curr))) { 
        if (Node_getType(curr) == DIRECTORY) {
            return curr;
        }
        else {
            return NOT_A_DIRECTORY;
        }
    }
    /* Otherwise, if the current path to the node matches the query path to
    that point, examine the current node's children to see if the query path exists there. */
    else if(!strncmp(path, Node_getPath(curr), strlen(Node_getPath(curr)))) {
        for(i = 0; i < Node_getNumChildren(curr); i++) {
            found = FT_getEndOfPathNode(path,
                                    Node_getChild(curr, i));
            if(found != NULL)
                return found;
        }
        return curr;
    }
    return NULL;
}

/*
   Inserts a new path into the tree rooted at parent, or, if
   parent is NULL, as the root of the data structure.

   If a node representing path already exists, returns ALREADY_IN_TREE

   If there is an allocation error in creating any of the new nodes or
   their fields, returns MEMORY_ERROR

   If there is an error linking any of the new nodes,
   returns PARENT_CHILD_ERROR

   Otherwise, returns SUCCESS
*/
static int FT_insertRestOfPath(char* path, Node_T parent) {
    Node_T curr = parent;
    Node_T firstNew = NULL;
    Node_T new;
    char* copyPath;
    char* restPath = path;
    char* dirToken;
    int result;
    size_t newCount = 0;

    assert(path != NULL);

    /* Conditional checks for invariants. If invariant tests pass,
    increment restPath to point to the end of the current path we're on.
    For example, if the currPath is 'a/b/c/' and path is 'a/b/c/d/e/', 
    restPath would now point to d. */
    if(curr == NULL) {
        if(root != NULL) {
            return CONFLICTING_PATH;
        }
    }
    else {
        if(!strcmp(path, Node_getPath(curr)))
            return ALREADY_IN_TREE;
        restPath += (strlen(Node_getPath(curr)) + 1);
    }

    /* Allocates memory for defensive copy, copies restPath -> 
    copyPath, and gets first instance of a non-'/' character. */
    copyPath = malloc(strlen(restPath)+1);
    if(copyPath == NULL)
        return MEMORY_ERROR;
    strcpy(copyPath, restPath);
    dirToken = strtok(copyPath, "/");

    while(dirToken != NULL) {
        new = Node_create(dirToken, curr, 0);

        if(new == NULL) {
            if(firstNew != NULL)
                (void) Node_destroy(firstNew);
            free(copyPath);
            return MEMORY_ERROR;
        }

        newCount++;

        if(firstNew == NULL)
            firstNew = new;
        else {
            result = FT_linkParentToChild(curr, new);
            if(result != SUCCESS) {
                (void) Node_destroy(new);
                (void) Node_destroy(firstNew);
                free(copyPath);
                return result;
            }
        }
        curr = new;
        dirToken = strtok(NULL, "/");
    }

    free(copyPath);

    if(parent == NULL) {
        root = firstNew;
        count = newCount;
        return SUCCESS;
    }
    else {
        result = FT_linkParentToChild(parent, firstNew);
        if(result == SUCCESS)
            count += newCount;
        else
            (void) Node_destroy(firstNew);

        return result;
   }
}

/*
   Inserts a new directory into the tree at path, if possible.
   Returns SUCCESS if the new directory is inserted.
   Returns INITIALIZATION_ERROR if not in an initialized state.
   Returns CONFLICTING_PATH if path is not underneath existing root.
   Returns NOT_A_DIRECTORY if a proper prefix of path exists as a file.
   Returns ALREADY_IN_TREE if the path already exists (as dir or file).
   Returns MEMORY_ERROR if unable to allocate any node or any field.
   Returns PARENT_CHILD_ERROR if a parent cannot link to a new child.
*/
int FT_insertDir(char *path) {
    Node_T curr;
    int result;

    assert(CheckerFT_isValid(isInitialized,root,count));
    assert(path != NULL);

    if(!isInitialized)
        return INITIALIZATION_ERROR;
    
    /* Gets node at the end of the query path, so we can insert directory
    at the end of this path. */
    curr = FT_getEndOfPathNode(path);
    
    /* Inserts path at the farthest node in the current path. */
    result = FT_insertRestOfPath(path, curr);
    assert(CheckerFT_isValid(isInitialized,root,count));
    return result;
}

/*
  Returns TRUE if the tree contains the full path parameter as a
  directory and FALSE otherwise.
*/
boolean FT_containsDir(char *path) {
    Node_T curr;
    boolean result;

    assert(CheckerDT_isValid(isInitialized,root,count));
    assert(path != NULL);

    if(!isInitialized)
        return FALSE;

    curr = FT_getEndOfPathNode(path);

    if(curr == NULL)
        result = FALSE;
    else if(strcmp(path, Node_getPath(curr)))
        result = FALSE;
    else
        result = TRUE;

    assert(CheckerDT_isValid(isInitialized,root,count));
    return result;
}

/*
  Removes the directory hierarchy rooted at path starting from
  curr. If curr is the data structure's root, root becomes NULL.

  Returns NO_SUCH_PATH if curr is not the node for path,
  and SUCCESS otherwise.
 */
static int FT_rmPathAt(char* path, Node_T curr) {
   Node_T parent;

   assert(path != NULL);
   assert(curr != NULL);

   parent = Node_getParent(curr);

   if(!strcmp(path,Node_getPath(curr))) {
        if(parent == NULL)
            root = NULL;
        else
            Node_unlinkChild(parent, curr);

        /* Taken from DT_removePathFrom. */
        if(curr != NULL) {
            count -= Node_destroy(curr);
        }
      return SUCCESS;
   }
   else
      return NO_SUCH_PATH;

}

/*
  Removes the FT hierarchy rooted at the directory path.
  Returns SUCCESS if found and removed.
  Returns INITIALIZATION_ERROR if not in an initialized state.
  Returns NOT_A_DIRECTORY if path exists but is a file not a directory.
  Returns NO_SUCH_PATH if the path does not exist in the hierarchy.
*/
int FT_rmDir(char *path) {
    Node_T curr;
    int result;

    assert(CheckerDT_isValid(isInitialized,root,count));
    assert(path != NULL);

    if(!isInitialized)
        return INITIALIZATION_ERROR;

    curr = FT_getEndOfPathNode(path);
    if(curr == NULL)
        result =  NO_SUCH_PATH;
    else
        result = FT_rmPathAt(path, curr);

    assert(CheckerDT_isValid(isInitialized,root,count));
    return result;
}

/*
   Inserts a new file into the hierarchy at the given path, with the
   given contents of size length bytes.
   Returns SUCCESS if the new file is inserted.
   Returns INITIALIZATION_ERROR if not in an initialized state.
   Returns CONFLICTING_PATH if path is not underneath existing root,
                            or if path would be the FT root.
   Returns NOT_A_DIRECTORY if a proper prefix of path exists as a file.
   Returns ALREADY_IN_TREE if the path already exists (as dir or file).
   Returns MEMORY_ERROR if unable to allocate any node or any field.
   Returns PARENT_CHILD_ERROR if a parent cannot link to a new child.
*/
int FT_insertFile(char *path, void *contents, size_t length);

/*
  Returns TRUE if the tree contains the full path parameter as a
  file and FALSE otherwise.
*/
boolean FT_containsFile(char *path);

/*
  Removes the FT file at path.
  Returns SUCCESS if found and removed.
  Returns INITIALIZATION_ERROR if not in an initialized state.
  Returns NOT_A_FILE if path exists but is a directory not a file.
  Returns NO_SUCH_PATH if the path does not exist in the hierarchy.
*/
int FT_rmFile(char *path);

/*
  Returns the contents of the file at the full path parameter.
  Returns NULL if the path does not exist or is a directory.

  Note: checking for a non-NULL return is not an appropriate
  contains check -- the contents of a file may be NULL.
*/
void *FT_getFileContents(char *path);

/*
  Replaces current contents of the file at the full path parameter with
  the parameter newContents of size newLength.
  Returns the old contents if successful. (Note: contents may be NULL.)
  Returns NULL if the path does not already exist or is a directory.
*/
void *FT_replaceFileContents(char *path, void *newContents,
                             size_t newLength);

/*
  Returns SUCCESS if path exists in the hierarchy,
  returns NO_SUCH_PATH if it does not, and
  returns INITIALIZATION_ERROR if the structure is not initialized.

  When returning SUCCESSS,
  if path is a directory: *type is set to FALSE, *length is unchanged
  if path is a file: *type is set to TRUE, and
                     *length is set to the length of file's contents.

  When returning a non-SUCCESS status, *type and *length are unchanged.
 */
int FT_stat(char *path, boolean *type, size_t *length);

/*
  Sets the data structure to initialized status.
  The data structure is initially empty.
  Returns INITIALIZATION_ERROR if already initialized,
  and SUCCESS otherwise.
*/
int FT_init(void) {
    assert(CheckerDT_isValid(isInitialized,root,count));
    if(isInitialized)
        return INITIALIZATION_ERROR;
    isInitialized = 1;
    root = NULL;
    count = 0;
    assert(CheckerDT_isValid(isInitialized,root,count));
    return SUCCESS;
}

/*
  Removes all contents of the data structure and
  returns it to uninitialized status.
  Returns INITIALIZATION_ERROR if not already initialized,
  and SUCCESS otherwise.
*/
int FT_destroy(void);

/*
  Returns a string representation of the
  data structure, or NULL if the structure is
  not initialized or there is an allocation error.

  Allocates memory for the returned string,
  which is then owned by client!
*/
char *FT_toString(void);
