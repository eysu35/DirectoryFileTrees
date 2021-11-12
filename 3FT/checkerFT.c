/*--------------------------------------------------------------------*/
/* checkerFT.c                                                        */
/* Authors: Michael Garcia and Ellen Su                               */
/*--------------------------------------------------------------------*/

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "dynarray.h"
#include "checkerFT.h"


/* see checkerFT.h for specification */
boolean CheckerFT_Node_isValid(Node_T n) {
   Node_T parent;
   const char* npath;
   const char* ppath;
   const char* rest;
   size_t i;
   size_t num_children;

   /* Sample check: a NULL pointer is not a valid node */
   if(n == NULL) {
      fprintf(stderr, "A node is a NULL pointer\n");
      return FALSE;
   }

   /* Check that type is either file or directory. */
   if (Node_getType(n) != 0 && Node_getType(n) != 1) {
         fprintf(stderr, "Node's type is not valid.");
         return FALSE;
   }

   /********** DIR CHECKS ****************/
   if (Node_getType(n) == 1){
      
   }

   /********** FILE CHECKS ****************/
   if (Node_getType(n) == 1){
      /* check that actual length of file contents matches node's 
      length field */
      if (DynArray_getLength(Node_getFileContents(n) != Node_getLength(n))){
         fprintf(stderr, "Length of node's contents do not match node->length");
         return FALSE;
      }
   }





   parent = Node_getParent(n);
   if(parent != NULL) {
      npath = Node_getPath(n);

      /********** PATH CHECKS ****************/

      /* Sample check that parent's path must be prefix of n's path */
      ppath = Node_getPath(parent);
      /* Checks that parent's path is not null. */
      if(ppath == NULL) {
         fprintf(stderr, "P has NULL path\n");
         return FALSE;
      }
      i = strlen(ppath);
      if(strncmp(npath, ppath, i)) {
         fprintf(stderr, "P's path is not a prefix of C's path\n");
         return FALSE;
      }
      /* Sample check that n's path after parent's path + '/'
         must have no further '/' characters */
      rest = npath + i;
      rest++;
      if(strstr(rest, "/") != NULL) {
         fprintf(stderr, "C's path has grandchild of P's path\n");
         return FALSE;
      }

      /********** CHILD CHECKS ****************/

      /* Check that children nodes of dir node are in sorted order */
      /* if file node, num_children = 1 so will skip this loop */
      num_children = Node_getNumChildren(parent);
      if (num_children > 1){
         for (i = 0; i < num_children - 1; i++){
            /* check that the children are not null */
            if (Node_getChild(parent, i) == NULL){
               fprintf(stderr, "P has a NULL child node\n");
               return FALSE;
            }
            if (Node_getChild(parent, i + 1) == NULL){
               fprintf(stderr, "P has a NULL child node!\n");
               return FALSE;
            }

            if (Node_compare(Node_getChild(parent, i), Node_getChild(parent, i+1)) >= 0){
               fprintf(stderr, "P's children are not in sorted order\n");
               return FALSE;
            }
         }
      }

   }
   return TRUE;
}

/*
   Performs a pre-order traversal of the tree rooted at n.
   Returns FALSE if a broken invariant is found and
   returns TRUE otherwise.

   You may want to change this function's return type or
   parameter list to facilitate constructing your checks.
   If you do, you should update this function comment.
*/
static boolean CheckerFT_treeCheck(Node_T n) {
   size_t c;

   if(n != NULL) {

      /* Sample check on each non-root node: node must be valid */
      /* If not, pass that failure back up immediately */
      if(!CheckerFT_Node_isValid(n))
         return FALSE;


      for(c = 0; c < Node_getNumChildren(n); c++)
      {
         Node_T child = Node_getChild(n, c);

         /* if recurring down one subtree results in a failed check
            farther down, passes the failure back up immediately */
         if(!CheckerFT_treeCheck(child))
            return FALSE;
      }
   }
   return TRUE;
}

/* see CheckerFT.h for specification */
boolean CheckerFT_isValid(boolean isInit, Node_T root, size_t count) {

   /* Sample check on a top-level data structure invariant:
      if the DT is not initialized, its count should be 0. */
   if(!isInit){
      if(count != 0) {
         fprintf(stderr, "Not initialized, but count is not 0\n");
         return FALSE;
      }

      /* if DT is not initialized, root should be NULL */
      if(root != NULL){
         fprintf(stderr, "Not initialized, but root is not NULL\n");
         return FALSE;
      }
   }
   /* check if DT is initialized, either root is NULL and count = 0
   or root is not null and count != 0. */
   if (isInit){
      if (root == NULL){
         if (count != 0){
            fprintf(stderr, "Initialized DT with NULL root, but count != 0\n");
            return FALSE;
         }
      }
      if (root != NULL){
         if (count == 0){
            fprintf(stderr, "Initialized DT with non NULL root, but count == 0\n");
            return FALSE;
         }
         /* check to make sure root has no parent */

         if (Node_getParent(root) != NULL){
            fprintf(stderr, "Root has parent node\n");
            return FALSE;
         }  
      }
   }

 

   /* Now checks invariants recursively at each node from the root. */
   return CheckerFT_treeCheck(root);


}
