/*--------------------------------------------------------------------*/
/* node.c                                                             */
/* Authors: Ellen Su and Michael Garcia                               */
/*--------------------------------------------------------------------*/

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

#include "a4def.h"
#include "dynarray.h"
#include "node.h"
#include "checkerFT.h"

/*
   A node structure represents a directory in the directory tree
*/
struct node {
   /* the full path of this directory */
   char* path;

   /* the parent directory of this directory
      NULL for the root of the directory tree */
   Node_T parent;

   /* set type to 0 to indicate file node or 1 to
   indicate directory node. */
   nodeType type;

   /* either the children of a directory or the contents
   of a file are stored. Children will be stored in lexicographic 
   order. */
   DynArray_T contents;


   /* stores the length of the contents of a file node. Is NULL for
   a directory node */
   size_t length;
};


/*
  returns a path with contents
  n->path/dir
  or NULL if there is an allocation error.
  Allocates memory for the returned string,
  which is then owned by the caller!
*/
static char* Node_buildPath(Node_T n, const char* dir) {
   char* path;

   assert(dir != NULL);

   if(n == NULL)
      path = malloc(strlen(dir)+1);
   else
      path = malloc(strlen(n->path) + 1 + strlen(dir) + 1);

   if(path == NULL)
      return NULL;
   *path = '\0';

   if(n != NULL) {
      strcpy(path, n->path);
      strcat(path, "/");
   }
   strcat(path, dir);

   return path;
}

/* see node.h for specification */
Node_T Node_create(const char* dir, Node_T parent, nodeType type){
   Node_T new;

   assert(parent == NULL || CheckerFT_Node_isValid(parent));
   assert(dir != NULL);

   new = malloc(sizeof(struct node));
   if(new == NULL) {
      assert(parent == NULL || CheckerFT_Node_isValid(parent));
      return NULL;
   }

   new->path = Node_buildPath(parent, dir);

   if(new->path == NULL) {
      free(new);
      assert(parent == NULL || CheckerFT_Node_isValid(parent));
      return NULL;
   }

   new->type = type;

   new->parent = parent;
   new->length = (size_t)NULL;
   /* initialize DynArray size to 1 to be able to store file contents */
   new->contents = DynArray_new(1);
   if(new->contents == NULL) {
      free(new->path);
      free(new);
      assert(parent == NULL || CheckerFT_Node_isValid(parent));
      return NULL;
   }

   assert(parent == NULL || CheckerFT_Node_isValid(parent));
   assert(CheckerFT_Node_isValid(new));
   return new;
}

/* see node.h for specification */
size_t Node_destroy(Node_T n) {
   size_t i;
   size_t count = 0;
   Node_T c;

   assert(n != NULL);

   if (Node_getType(n) == DIRECTORY){
      for(i = 0; i < DynArray_getLength(n->contents); i++)
         {
            c = DynArray_get(n->contents, i);
            count += Node_destroy(c);
         }
   }
   DynArray_free(n->contents);

   free(n->path);
   free(n);
   count++;

   return count;
}

/* see node.h for specification */
/*char* Node_getPath(Node_T n) {
   char* pathCopy;
   assert(n != NULL);
   pathCopy = malloc(strlen(n->path)+1);
   if(pathCopy == NULL)
      return NULL;
   return strcpy(pathCopy, n->path);
}
*/
const char* Node_getPath(Node_T n) {
   assert(n != NULL);

   return n->path;
}

/* see node.h for specification */
int Node_getType(Node_T n) {
   assert(n != NULL);
   
   return(n->type);
}


/* see node.h for specification */
int Node_compare(Node_T node1, Node_T node2) {
   assert(node1 != NULL);
   assert(node2 != NULL);

   return strcmp(node1->path, node2->path);
}

/* see node.h for specification */
size_t Node_getNumChildren(Node_T n) {
   assert(n != NULL);

    if (n->type == DIRECTORY){
        return DynArray_getLength(n->contents);
    }
    else{
        return (size_t)(NOT_A_DIRECTORY);
    }
        
}

/* see node.h for specification */
int Node_hasChild(Node_T n, const char* path, size_t* childID) {
   size_t index;
   int result;
   Node_T checker;

   assert(n != NULL);
   assert(path != NULL);

   if (n->type == FT_FILE){
       return NOT_A_DIRECTORY;
   }

   checker = Node_create(path, NULL, n->type);
   if(checker == NULL) {
      return -1;
   }
   result = DynArray_bsearch(n->contents, checker, &index,
                    (int (*)(const void*, const void*)) Node_compare);
   (void) Node_destroy(checker);

   if(childID != NULL)
      *childID = index;

   return result;
}

/* see node.h for specification */
Node_T Node_getChild(Node_T n, size_t childID) {
   assert(n != NULL);

    if (n->type == DIRECTORY){
        if(DynArray_getLength(n->contents) > childID) {
                return DynArray_get(n->contents, childID);
            }
    }
    return NULL;
    }

/* see node.h for specification */
Node_T Node_getParent(Node_T n) {
   assert(n != NULL);

   return n->parent;
}

/* For Node_T n, updates n's old contents to contents. */
void* Node_updateFileContents(Node_T n, void *contents) {
   size_t i = 0;
   void *old_contents;

   assert(n != NULL);
   assert(CheckerFT_Node_isValid(n));

   if (n->type == DIRECTORY) {
      return (DynArray_T)NULL;
   }
   if (DynArray_getLength(n->contents) < i){
      DynArray_grow(n->contents);
   }
   old_contents = DynArray_set(n->contents, i, contents);
   assert(CheckerFT_Node_isValid(n));

   return old_contents;
}

/* Reset Node_T n's length field to new length of contents */
void Node_updateLength(Node_T n, size_t new_length){
   assert(n != NULL);
   n->length = new_length;
}

/* see node.h for specification */
DynArray_T Node_getFileContents(Node_T n){
   assert(n != NULL);
   assert(n->type == FT_FILE);

   return n->contents;
}

/* see node.h for specification */
int Node_linkChild(Node_T parent, Node_T child) {
   size_t i;
   char* rest;

   assert(parent != NULL);
   assert(child != NULL);
   assert(CheckerFT_Node_isValid(parent));
   assert(CheckerFT_Node_isValid(child));
   /* Make sure file does not have child. */
   if (parent->type == FT_FILE) {
      return PARENT_CHILD_ERROR;
   }

   if(Node_hasChild(parent, child->path, NULL)) {
      assert(CheckerFT_Node_isValid(parent));
      assert(CheckerFT_Node_isValid(child));
      return ALREADY_IN_TREE;
   }
   i = strlen(parent->path);
   if(strncmp(child->path, parent->path, i)) {
      assert(CheckerFT_Node_isValid(parent));
      assert(CheckerFT_Node_isValid(child));
      return PARENT_CHILD_ERROR;
   }
   rest = child->path + i;
   if(strlen(child->path) >= i && rest[0] != '/') {
      assert(CheckerFT_Node_isValid(parent));
      assert(CheckerFT_Node_isValid(child));
      return PARENT_CHILD_ERROR;
   }
   rest++;
   if(strstr(rest, "/") != NULL) {
      assert(CheckerFT_Node_isValid(parent));
      assert(CheckerFT_Node_isValid(child));
      return PARENT_CHILD_ERROR;
   }
   child->parent = parent;

   if(DynArray_bsearch(parent->contents, child, &i,
         (int (*)(const void*, const void*)) Node_compare) == 1) {
      assert(CheckerFT_Node_isValid(parent));
      assert(CheckerFT_Node_isValid(child));
      return ALREADY_IN_TREE;
   }

   if(DynArray_addAt(parent->contents, i, child) == TRUE) {
      assert(CheckerFT_Node_isValid(parent));
      assert(CheckerFT_Node_isValid(child));
      return SUCCESS;
   }
   else {
      assert(CheckerFT_Node_isValid(parent));
      assert(CheckerFT_Node_isValid(child));
      return PARENT_CHILD_ERROR;
   }
}

/* see node.h for specification */
int  Node_unlinkChild(Node_T parent, Node_T child) {
    size_t i;

    assert(parent != NULL);
    assert(child != NULL);
    assert(CheckerFT_Node_isValid(parent));
    assert(CheckerFT_Node_isValid(child));

    /* parent is a file node, return error */
    if (parent->type == FT_FILE){
        return PARENT_CHILD_ERROR;
    }

    if(DynArray_bsearch(parent->contents, child, &i,
            (int (*)(const void*, const void*)) Node_compare) == 0) {
        assert(CheckerFT_Node_isValid(parent));
        assert(CheckerFT_Node_isValid(child));
        return PARENT_CHILD_ERROR;
    }

    (void) DynArray_removeAt(parent->contents, i);

    assert(CheckerFT_Node_isValid(parent));
    assert(CheckerFT_Node_isValid(child));
    return SUCCESS;
    }


/* see node.h for specification */
int Node_addChild(Node_T parent, const char* dir, nodeType type) {
    Node_T new;
    int result;

    assert(parent != NULL);
    assert(dir != NULL);
    assert(CheckerFT_Node_isValid(parent));

    /* if parent is already a file node, return error */
    if (parent->type == FT_FILE){
        return PARENT_CHILD_ERROR;
    }
    new = Node_create(dir, parent, type);
    if(new == NULL) {
        assert(CheckerFT_Node_isValid(parent));
        return PARENT_CHILD_ERROR;
    }
    result = Node_linkChild(parent, new);
    if(result != SUCCESS)
        (void) Node_destroy(new);
    else
        assert(CheckerFT_Node_isValid(new));

    assert(CheckerFT_Node_isValid(parent));
    return result;
}


/* see node.h for specification */
char* Node_toString(Node_T n) {
   char* copyPath;

   assert(n != NULL);

   copyPath = malloc(strlen(n->path)+1);
   if(copyPath == NULL) {
      return NULL;
   }
   else {
      return strcpy(copyPath, n->path);
   }
}