/****************************************************************************
 *file ctc_avl_tree.c

 *author  Copyright (C) 2011 Centec Networks Inc.  All rights reserved.

 *date 2009-11-26

 *version v2.0

 The file define  avl-tree arithmetic lib
 ****************************************************************************/

#include "kal.h"
#include "ctc_avl_tree.h"

/* Get a free AVL tree node. */
static struct ctc_avl_node *
_ctc_avl_node_new (struct ctc_avl_tree *ctc_avl_tree)
{
  struct ctc_avl_node *node;
  int32 node_sz;

  node = ctc_avl_tree->free_list;
  if (node)
    {
      ctc_avl_tree->free_list = node->right;
      ctc_avl_tree->count++;
    }
  else if (ctc_avl_tree->max_nodes == 0)
    {
      node_sz = sizeof (struct ctc_avl_node);

      node = (struct ctc_avl_node *) mem_malloc(MEM_AVL_MODULE, node_sz);
      if (! node)
	     return NULL;
      kal_memset(node, 0, node_sz);

      ctc_avl_tree->count++;
    }

  return node;
}

/* Free a AVL tree node. */
static void
_ctc_avl_node_free (struct ctc_avl_tree *ctc_avl_tree, struct ctc_avl_node *node)
{
  node->right = ctc_avl_tree->free_list;
  node->left = NULL;
  ctc_avl_tree->free_list = node;
  ctc_avl_tree->count--;

  return;
}

/* Free AVL free node list. */
static void
_ctc_avl_freelist_free (struct ctc_avl_tree *ctc_avl_tree)
{
  struct ctc_avl_node *node;
  struct ctc_avl_node *next;

  for (node = ctc_avl_tree->free_list; node; node = next)
    {
      next = node->right;
      if (node)
	mem_free(node);
      ctc_avl_tree->count--;
    }
}

/* Create AVL tree.
   If the max_nodes is 0, no preallocation is done. Every node is allocated
   first from the free_list and then from the system memory. */
int32
ctc_avl_create (struct ctc_avl_tree **ctc_avl_tree,
                int32 max_nodes,
                int32 (*compare_function) (void *data1, void *data2))
{
    int32 i;
    struct ctc_avl_tree *avl;
    struct ctc_avl_node *node;
    int32 node_sz;

    avl = (struct ctc_avl_tree *) mem_malloc (MEM_AVL_MODULE, sizeof (struct ctc_avl_tree));
    if (! avl)
        return -1;

    kal_memset(avl, 0, sizeof (struct ctc_avl_tree));

    avl->max_nodes = max_nodes;
    avl->root = NULL;
    avl->compare_function = compare_function;

    /* Initialize free list. */
    avl->free_list = NULL;

    /* Initialize max count. */
    avl->count = max_nodes;

    /* Check if nodes are to be preallocated. */
    if (max_nodes != 0)
    {
        /* Preallocate maximum number of entries and put on free list. */
        node_sz = sizeof (struct ctc_avl_node);

        avl->data = (char *) mem_malloc (MEM_AVL_MODULE, (node_sz * max_nodes));
        if (! avl->data)
        {
            mem_free(avl);
            return -1;
        }

        kal_memset(avl->data, 0, node_sz * max_nodes);

        /* Add all entries in the free list. */
        for (i = 0; i < max_nodes; i++)
        {
            node = (struct ctc_avl_node *) (avl->data + i * node_sz);
            _ctc_avl_node_free (avl, node);
        }
    }

    *ctc_avl_tree = avl;

    return 0;
}

/* Delete AVL tree. */
static int32
ctc_avl_finish (struct ctc_avl_tree *ctc_avl_tree)
{
  /* Check tree nodes allocation type. */
  if(ctc_avl_tree->max_nodes == 0)
     {
       /* Free free list. */
        _ctc_avl_freelist_free (ctc_avl_tree);
     }
   else if(ctc_avl_tree->data)
     {
        /* Free all preallocated nodes. */
        mem_free(ctc_avl_tree->data);
     }

  /* Free tree. */
  mem_free(ctc_avl_tree);

  return 0;
}

/* Internal function for traversal. */
static int32
_ctc_avl_traverse (struct ctc_avl_tree *ctc_avl_tree,
                   struct ctc_avl_node *node,
                   avl_traversal_fn fn,
                   void *data)
{
    int32  ret;

    if (node != NULL)
    {
        if ((ret = _ctc_avl_traverse(ctc_avl_tree, node->left, fn, data)) < 0)
            return ret;

        if ((ret = (*fn)(node->info, data)) < 0)
            return ret;

        if ((ret = _ctc_avl_traverse(ctc_avl_tree, node->right, fn, data)) < 0)
            return ret;
    }

    return 0;
}

static int32
_ctc_avl_traverse2 (struct ctc_avl_tree *ctc_avl_tree,
                    struct ctc_avl_node *node,
                    avl_traversal2_fn fn,
                    void *data1, void *data2)
{
    int32  ret;

    if (node != NULL)
    {
        if ((ret = _ctc_avl_traverse2(ctc_avl_tree, node->left, fn, data1, data2)) < 0)
            return ret;

        if ((ret = (*fn)(node->info, data1, data2)) < 0)
            return ret;

        if ((ret = _ctc_avl_traverse2(ctc_avl_tree, node->right, fn, data1, data2)) < 0)
            return ret;
    }

    return 0;
}

static int32
_ctc_avl_traverse3 (struct ctc_avl_tree *ctc_avl_tree,
                struct ctc_avl_node *node,
                avl_traversal3_fn fn,
                void *data1, void *data2, void *data3)
{
  int32  ret;

  if (node != NULL)
    {
      if ((ret = _ctc_avl_traverse3(ctc_avl_tree, node->left, fn, data1, data2,
                                data3)) < 0)
        return ret;

      if ((ret = (*fn)(node->info, data1, data2, data3)) < 0)
        return ret;

      if ((ret = _ctc_avl_traverse3(ctc_avl_tree, node->right, fn, data1, data2,
                                data3)) < 0)
        return ret;
    }

  return 0;
}

/* Traverse tree. */
int32
ctc_avl_traverse (struct ctc_avl_tree *ctc_avl_tree, avl_traversal_fn fn, void *data)
{
  return _ctc_avl_traverse (ctc_avl_tree, ctc_avl_tree->root, fn, data);
}

int32
ctc_avl_traverse2 (struct ctc_avl_tree *ctc_avl_tree, avl_traversal2_fn fn, void *data1,
               void *data2)
{
  return _ctc_avl_traverse2 (ctc_avl_tree, ctc_avl_tree->root, fn, data1, data2);
}

int32
ctc_avl_traverse3 (struct ctc_avl_tree *ctc_avl_tree, avl_traversal3_fn fn, void *data1,
               void *data2, void *data3)
{
  return _ctc_avl_traverse3 (ctc_avl_tree, ctc_avl_tree->root, fn, data1, data2, data3);
}

/* Tree top. */
struct ctc_avl_node *
ctc_avl_top (struct ctc_avl_tree *ctc_avl_tree)
{
  return (ctc_avl_tree != NULL) ? ctc_avl_tree->root : NULL;
}

/* Tree left bottom node */
struct ctc_avl_node *ctc_avl_left_bottom_node (struct ctc_avl_tree *ctc_avl_tree, struct ctc_avl_node *left_node)
{

    if (left_node == NULL)
    {
        return NULL;
    }

    if (left_node->left != NULL)
    {
        return ctc_avl_left_bottom_node(ctc_avl_tree, left_node->left);
    }
    else
    {
        return left_node;
    }

}
/* Tree right bottom node . */
struct ctc_avl_node *
ctc_avl_right_bottom_node (struct ctc_avl_tree *ctc_avl_tree, struct ctc_avl_node *right_node)
{
    if (right_node == NULL)
    {
        return NULL;
    }

    if (right_node->right != NULL)
    {
        return ctc_avl_left_bottom_node(ctc_avl_tree, right_node->right);
    }
    else
    {
        return right_node;
    }
}

/* Internal function to lookup node. */
static struct ctc_avl_node*
_ctc_avl_lookup (struct ctc_avl_tree *tree, struct ctc_avl_node *node, void *data)
{
  int32 cmp;

  if (node == NULL)
    return NULL;

  cmp = (*(tree->compare_function)) (data, node->info);
  if (cmp < 0)
    return _ctc_avl_lookup (tree, node->left, data);
  if (cmp > 0)
    return _ctc_avl_lookup (tree, node->right, data);

  return node;
}

/* Lookup AVL tree. */
struct ctc_avl_node *
ctc_avl_search (struct ctc_avl_tree *tree, void *data)
{
  if (tree->root == NULL)
    return NULL;

  return _ctc_avl_lookup (tree, tree->root, data);
}

void*
ctc_avl_lookup_min (struct ctc_avl_tree *tree)
{
  ctc_avl_node_t* t;

  if (tree == NULL)
    return NULL;

  t = tree->root;

  while(NULL != t->left)
  {
    t = t->left;
  }

  return t->info;
}

void*
ctc_avl_lookup_max (struct ctc_avl_tree *tree)
{
  ctc_avl_node_t* t;

  if (tree == NULL)
    return NULL;

  t = tree->root;

  while(NULL != t->right)
  {
    t = t->right;
  }

  return t->info;
}

/* Next node. */
struct ctc_avl_node*
ctc_avl_next (struct ctc_avl_node *node)
{
    struct ctc_avl_node *start;
    int32 count = 0;

    if (node->left)
        return node->left;

    if (node->right)
        return node->right;

    start = node;
    while (node->parent)
    {
        if (node->parent->left == node && node->parent->right)
            return node->parent->right;

        count++;

        node = node->parent;
    }

    return NULL;
}

/* Get next node. */
struct ctc_avl_node*
ctc_avl_getnext (struct ctc_avl_tree *tree, struct ctc_avl_node *node)
{
    struct ctc_avl_node *nodeA, *nodeB;

    if (tree->root == NULL) return NULL;
        nodeA = node;
    nodeB = (nodeA == NULL ? tree->root : nodeA->right);
    if (nodeB == NULL)
    {
        /* Go up parent from the left subtree */
        for (; ; )
        {
            nodeB = nodeA->parent;
            if (nodeB == NULL)
                break;
            if (nodeA->flag == 0)
                break;
            nodeA = nodeB;
        }
    }
    else
    {  /* go downstairs into the right subtree */
        for (; ; )
        {
            nodeA = nodeB->left;
            if (nodeA == NULL)
                break;
            nodeB = nodeA;
        }
    }

    return nodeB;
}

/* Rotate tree for balancing. */
static struct ctc_avl_node *
_ctc_avl_rotate (struct ctc_avl_tree *tree, struct ctc_avl_node *node)
{
    struct ctc_avl_node *nodeA, *nodeB, *nodeC, *nodeD, *nodeE, *nodeF;

    nodeB = node;

    if (nodeB->balance < 0)
    {
        /* Negative (left) rotation */
        nodeA = nodeB->parent;
        nodeC = nodeB->left;
        nodeD = nodeC->right;
        if (nodeC->balance <= 0)
        {
            /* Single negative rotation */
            if (nodeA == NULL)
                tree->root = nodeC;
            else if (nodeB->flag == 0)
                nodeA->left = nodeC;
            else
                nodeA->right = nodeC;
            nodeB->rank -= nodeC->rank;
            nodeC->parent = nodeA;
            nodeC->flag = nodeB->flag;
            nodeC->balance++;
            nodeC->right = nodeB;
            nodeB->parent = nodeC;
            nodeB->flag = 1;
            nodeB->balance = (-nodeC->balance);
            nodeB->left = nodeD;
            if (nodeD != NULL)
            {
                nodeD->parent = nodeB;
                nodeD->flag = 0;
            }
            node = nodeC;
        }
        else
        {
            /* Double negative rotation */
            nodeE = nodeD->left;
            nodeF = nodeD->right;
            if (nodeA == NULL)
                tree->root = nodeD;
            else  if (nodeB->flag == 0)
                nodeA->left = nodeD;
            else nodeA->right = nodeD;
                nodeB->rank -= (nodeC->rank + nodeD->rank);
            nodeD->rank += nodeC->rank;
            nodeB->balance = (nodeD->balance >= 0 ? 0 : + 1);
            nodeC->balance = (nodeD->balance <= 0 ? 0 : -1);
            nodeD->parent = nodeA;
            nodeD->flag = nodeB->flag;
            nodeD->balance = 0;
            nodeD->left = nodeC;
            nodeD->right = nodeB;
            nodeB->parent = nodeD;
            nodeB->flag = 1;
            nodeB->left = nodeF;
            nodeC->parent = nodeD;
            nodeC->flag = 0;
            nodeC->right = nodeE;
            if (nodeE != NULL)
            {
                nodeE->parent = nodeC;
                nodeE->flag = 1;
            }
            if (nodeF != NULL)
            {
                nodeF->parent = nodeB;
                nodeF->flag = 0;
            }
            node = nodeD;
        }
    }
    else
    {
        /* Positive (right) rotation */
        nodeA = nodeB->parent;
        nodeC = nodeB->right;
        nodeD = nodeC->left;
        if (nodeC->balance >= 0)
        {
            /* Single positive rotation */
            if (nodeA == NULL)
                tree->root = nodeC;
            else if (nodeB->flag == 0)
                nodeA->left = nodeC;
            else
                nodeA->right = nodeC;
            nodeC->rank += nodeB->rank;
            nodeC->parent = nodeA;
            nodeC->flag = nodeB->flag;
            nodeC->balance--;
            nodeC->left = nodeB;
            nodeB->parent = nodeC;
            nodeB->flag = 0;
            nodeB->balance = (-nodeC->balance);
            nodeB->right = nodeD;
            if (nodeD != NULL)
            {
                nodeD->parent = nodeB;
                nodeD->flag = 1;
            }
            node = nodeC;
        }
        else
        {
            /* Double positive rotation */
            nodeE = nodeD->left;
            nodeF = nodeD->right;
            if (nodeA == NULL)
                tree->root = nodeD;
            else if (nodeB->flag == 0)
                nodeA->left = nodeD;
            else
                nodeA->right = nodeD;
            nodeC->rank -= nodeD->rank;
            nodeD->rank += nodeB->rank;
            nodeB->balance = (nodeD->balance <= 0 ? 0 : -1);
            nodeC->balance = (nodeD->balance >= 0 ? 0 : + 1);
            nodeD->parent = nodeA;
            nodeD->flag = nodeB->flag;
            nodeD->balance = 0;
            nodeD->left = nodeB;
            nodeD->right = nodeC;
            nodeB->parent = nodeD;
            nodeB->flag = 0;
            nodeB->right = nodeE;
            nodeC->parent = nodeD;
            nodeC->flag = 1;
            nodeC->left = nodeF;
            if (nodeE != NULL)
            {
                nodeE->parent = nodeB;
                nodeE->flag = 1;
            }
            if (nodeF != NULL)
            {
                nodeF->parent = nodeC;
                nodeF->flag = 0;
            }
            node = nodeD;
        }
    }
    return node;
}

/* Insert a node in the AVL tree. */
int32
ctc_avl_insert (struct ctc_avl_tree *tree, void *data)
{
    struct ctc_avl_node *nodeA, *nodeB, *nodeC;
    int32 flag = 0;

    /* Create a new node. */
    nodeC = _ctc_avl_node_new (tree);

    if (nodeC == NULL)
        return -1;

    /* Find location for insertion. */
    nodeA = NULL;
    nodeB = tree->root;

    while (nodeB != NULL)
    {
        nodeA = nodeB;
        if (tree->compare_function (data, nodeA->info) <= 0)
        {
            flag = 0;
            nodeB = nodeA->left;
            nodeA->rank++;
        }
        else
        {
            flag = 1;
            nodeB = nodeA->right;
        }
    }

    nodeC->info = data;
    nodeC->rank = 1;
    nodeC->parent = nodeA;
    nodeC->flag = (nodeA == NULL ? 0 : flag);
    nodeC->balance = 0;
    nodeC->left = nodeC->right = NULL;
    if (nodeA == NULL)
        tree->root = nodeC;
    else if (flag == 0)
        nodeA->left = nodeC;
    else
        nodeA->right = nodeC;

    /* Go parent root and correct all subtrees affected by insertion. */
    while (nodeA != NULL)
    {
        if (flag == 0)
        {
            /* Height of p's left subtree increased. */
            if (nodeA->balance > 0)
            {
                nodeA->balance = 0;
                break;
            }
            if (nodeA->balance < 0)
            {
                _ctc_avl_rotate (tree, nodeA);
                break;
            }
            nodeA->balance = -1;
            flag = nodeA->flag;
            nodeA = nodeA->parent;
        }
        else
        {
            /* Height of p's right subtree increased. */
            if (nodeA->balance < 0)
            {
                nodeA->balance = 0;
                break;
            }
            if (nodeA->balance > 0)
            {
                _ctc_avl_rotate (tree, nodeA);
                break;
            }
            nodeA->balance = + 1;
            flag = nodeA->flag;
            nodeA = nodeA->parent;
        }
    }

    return 0;
}

/* Remove a node entry. */
static int32
_ctc_avl_delete_node (struct ctc_avl_tree *tree, struct ctc_avl_node *node)
{
    struct ctc_avl_node *nodeA, *nodeB, *nodeC, *nodeD, *nodeE, *nodeF, *nodeG;
    int32 flag;

    if (node == NULL)
        return -1;

    nodeB = node;

    /* If both subtrees of the specified node are non-empty, the node
    should be interchanged with the next one, at least one subtree
    of which is always empty. */
    if (nodeB->left == NULL || nodeB->right == NULL)
        goto SKIP;

    nodeA = nodeB->parent;
    nodeC = nodeB->left;

    nodeD = ctc_avl_getnext (tree, nodeB);
    nodeE = nodeD->right;
    if (nodeB->right == nodeD)
    {
        if (nodeA == NULL)
            tree->root = nodeD;
        else if (nodeB->flag == 0)
            nodeA->left = nodeD;
        else nodeA->right = nodeD;
            nodeD->rank = nodeB->rank;
        nodeD->parent = nodeA;
        nodeD->flag = nodeB->flag;
        nodeD->balance = nodeB->balance;
        nodeD->left = nodeC;
        nodeD->right = nodeB;
        nodeC->parent = nodeD;
        nodeB->rank = 1;
        nodeB->parent = nodeD;
        nodeB->flag = 1;
        nodeB->balance = (nodeE == NULL ? 0 : + 1);
        nodeB->left = NULL;
        nodeB->right = nodeE;

        if (nodeE != NULL)
            nodeE->parent = nodeB;
    }
    else
    {
        nodeF = nodeB->right;
        nodeG = nodeD->parent;

        if (nodeA == NULL)
            tree->root = nodeD;
        else if (nodeB->flag == 0)
            nodeA->left = nodeD;
        else nodeA->right = nodeD;

            nodeD->rank = nodeB->rank;
        nodeD->parent = nodeA;
        nodeD->flag = nodeB->flag;
        nodeD->balance = nodeB->balance;
        nodeD->left = nodeC;
        nodeD->right = nodeF;
        nodeC->parent = nodeD;
        nodeF->parent = nodeD;
        nodeG->left = nodeB;
        nodeB->rank = 1;
        nodeB->parent = nodeG;
        nodeB->flag = 0;
        nodeB->balance = (nodeE == NULL ? 0 : + 1);
        nodeB->left = NULL;
        nodeB->right = nodeE;

        if (nodeE != NULL)
            nodeE->parent = nodeB;
    }

    SKIP:
    /* Now the specified node [p] has at least one empty subtree;
    go up to the root and adjust the rank field of all nodes
    affected by deletion */
    nodeC = nodeB;
    nodeA = nodeC->parent;
    while (nodeA != NULL)
    {
        if (nodeC->flag == 0)
            nodeA->rank--;
        nodeC = nodeA;
        nodeA = nodeC->parent;
    }

    /* delete the specified node from the tree */
    nodeA = nodeB->parent;
    flag = nodeB->flag;
    nodeC = nodeB->left != NULL ? nodeB->left : nodeB->right;

    if (nodeA == NULL)
        tree->root = nodeC;
    else if (flag == 0)
        nodeA->left = nodeC;
    else
        nodeA->right = nodeC;
    if (nodeC != NULL)
    {
        nodeC->parent = nodeA;
        nodeC->flag = flag;
    }

    /* Go up to the root and correct all subtrees affected by
    deletion */
    while (nodeA != NULL)
    {
        if (flag == 0)
        {  /* the height of the left subtree of [f] is decreased */
            if (nodeA->balance == 0)
            {
                nodeA->balance = + 1;
                break;
            }
            if (nodeA->balance < 0)
                nodeA->balance = 0;
            else
            {
                nodeA = _ctc_avl_rotate (tree, nodeA);
                if (nodeA->balance < 0)
                    break;
            }
            flag = nodeA->flag;
            nodeA = nodeA->parent;
        }
        else
        {  /* the height of the right subtree of [f] is decreased */
            if (nodeA->balance == 0)
            {
                nodeA->balance = -1;
                break;
            }
            if (nodeA->balance > 0)
                nodeA->balance = 0;
            else
            {
                nodeA = _ctc_avl_rotate (tree, nodeA);
                if (nodeA->balance > 0)
                    break;
            }
            flag = nodeA->flag;
            nodeA = nodeA->parent;
        }
    }

    /* Free node. */
    _ctc_avl_node_free (tree, nodeB);

    return 0;
}

/* Delete node containing data. */
int32
ctc_avl_remove (struct ctc_avl_tree *tree, void *data)
{
  struct ctc_avl_node *node;
  int32 ret = 0;

  node =  ctc_avl_search (tree, data);
  if (node)
    {
      /* Delete node. */
      ret = _ctc_avl_delete_node (tree, node);
    }
  else
    ret = -1;

  return ret;
}

/* Delete node from AVL tree. */
int32
ctc_avl_delete_node (struct ctc_avl_tree *ctc_avl_tree, struct ctc_avl_node *node)
{
  return _ctc_avl_delete_node (ctc_avl_tree, node);
}

/* Get  AVL tree node count. */
int32
ctc_avl_get_tree_size(struct ctc_avl_tree *p_avl_tree)
{
  return p_avl_tree->count;
}


/*
tree free function.
*/
int32
ctc_avl_tree_free (struct ctc_avl_tree **pp_avl_tree, void (*avl_data_free)(void *ptr))
{
    struct ctc_avl_node *tmp_node;
    struct ctc_avl_node *node;

    if (*pp_avl_tree == NULL)
        return (0);

    node = ctc_avl_top (*pp_avl_tree);

    while (node)
    {
        if (node->left)
        {
            node = node->left;
            continue;
        }

        if (node->right)
        {
            node = node->right;
            continue;
        }

        tmp_node = node;
        node = node->parent;

        if (node != NULL)
        {
            if (node->left == tmp_node)
                node->left = NULL;
            else
                node->right = NULL;

            /* Free node data */
            if (avl_data_free)
                (*avl_data_free)(tmp_node->info);

            /* Move node to a free list.*/
            _ctc_avl_node_free (*pp_avl_tree, tmp_node);
        }
        else
        {
            /* Free node data */
            if (avl_data_free)
                (*avl_data_free)(tmp_node->info);

            /* Move node to a free list.*/
            _ctc_avl_node_free (*pp_avl_tree, tmp_node);
            break;
        }
    }

    /* Delete tree. */
    ctc_avl_finish (*pp_avl_tree);
    *pp_avl_tree = NULL;
    return 0;
}

/*
  Tree cleanup. Remove all nodes from tree but do not free the tree.
*/
int32
ctc_avl_tree_cleanup (struct ctc_avl_tree *ctc_avl_tree, void (*avl_data_free)(void *ptr))
{
    struct ctc_avl_node *tmp_node;
    struct ctc_avl_node *node;

    if (ctc_avl_tree == NULL)
        return -1;

    node = ctc_avl_top (ctc_avl_tree);

    while (node)
    {
        if (node->left)
        {
            node = node->left;
            continue;
        }

        if (node->right)
        {
            node = node->right;
            continue;
        }

        tmp_node = node;
        node = node->parent;

        if (node != NULL)
        {
            if (node->left == tmp_node)
                node->left = NULL;
            else
                node->right = NULL;

            /* Free node data */
            if (avl_data_free)
                (*avl_data_free)(tmp_node->info);

            /* Move node to a free list.*/
            _ctc_avl_node_free (ctc_avl_tree, tmp_node);
        }
        else
        {
            /* Free node data */
            if (avl_data_free)
                (*avl_data_free)(tmp_node->info);

            /* Move node to a free list.*/
            _ctc_avl_node_free (ctc_avl_tree, tmp_node);
            break;
        }
    }

    ctc_avl_tree->root = NULL;
    return 0;
}

