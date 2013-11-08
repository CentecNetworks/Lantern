/****************************************************************************
 *file ctc_linklist.c

 *author  Copyright (C) 2011 Centec Networks Inc.  All rights reserved.

 *date 2009-11-26

 *version v2.0

  This file contains  single/double linklist arithmetic lib
 ****************************************************************************/
#include "kal.h"
#include "kal_memmngr.h"
#include "ctc_linklist.h"

/* Allocate new list. */
struct ctc_linklist *
ctc_list_new ()
{
  struct ctc_linklist *new;

  new = (struct ctc_linklist *) mem_malloc(MEM_LINKLIST_MODULE, sizeof(struct ctc_linklist));

    if(new != NULL)
    {
        kal_memset(new,0,sizeof(struct ctc_linklist ));
    }
  return new;
}

struct ctc_linklist *ctc_list_create(ctc_list_cmp_cb_t cmp, ctc_list_del_cb_t del)
{
  struct ctc_linklist *new;


  new = (struct ctc_linklist *) mem_malloc(MEM_LINKLIST_MODULE, sizeof(struct ctc_linklist));

  if(NULL == new)
  {
  	return NULL;
  }
   kal_memset(new,0,sizeof(struct ctc_linklist ));

  new->cmp = cmp;
  new->del = del;
  return new;
}

/* Free list. */
void
ctc_list_free (struct ctc_linklist *l)
{
    mem_free ( l);
    l = NULL;

}

/* Allocate new listnode.  Internal use only. */
static struct ctc_listnode *
ctc_listnode_new ()
{
      struct ctc_listnode *node;
      node = (struct ctc_listnode *)mem_malloc (MEM_LINKLIST_MODULE, sizeof(struct ctc_listnode));
      node?  kal_memset(node,0,sizeof(struct ctc_listnode )):NULL;

  return node;
}

/* Free listnode. */
static void
ctc_listnode_free (struct ctc_listnode *node)
{
  mem_free ( node);
  node = NULL;
}

/* Add new data to the list. */
struct ctc_listnode *
ctc_listnode_add (struct ctc_linklist *list, void *val)
{
  struct ctc_listnode *node;

  if ( (!list) || (!val) )
    return NULL;

  node = ctc_listnode_new ();
  if ( !node )
    return NULL;

  node->prev = list->tail;
  node->data = val;

  if (list->head == NULL)
    list->head = node;
  else
    list->tail->next = node;
  list->tail = node;

  list->count++;

  return node;
}

/* Add new node with sort function. */
struct ctc_listnode *
ctc_listnode_add_sort (struct ctc_linklist *list, void *val)
{
  struct ctc_listnode *node;
  struct ctc_listnode *new;

  new = ctc_listnode_new ();
  if (! new)
    return NULL;

  new->data = val;

  if (list->cmp)
    {
      for (node = list->head; node; node = node->next)
	{
	  if ((list->cmp(val, node->data)) < 0)
	    {
	      new->next = node;
	      new->prev = node->prev;

	      if (node->prev)
		node->prev->next = new;
	      else
		list->head = new;
	      node->prev = new;
	      list->count++;
	      return new;
	    }
	}
    }

  new->prev = list->tail;

  if (list->tail)
    list->tail->next = new;
  else
    list->head = new;

  list->tail = new;
  list->count++;
  return new;
}

struct ctc_listnode *ctc_listnode_add_head (struct ctc_linklist *list, void *val)
{
  struct ctc_listnode *node;

  node = ctc_listnode_new ();
  if( !node )
  {
    return NULL;
  }

  node->data = val;


  if (list->head)
      list->head->prev = node;
  else
      list->tail = node;

  node->next = list->head;
  node->prev = NULL;

  list->head = node;

  list->count++;

  return node;
}
struct ctc_listnode *ctc_listnode_add_tail (struct ctc_linklist *list, void *val)
{

    struct ctc_listnode *node = NULL;

    if ( (!list) || (!val) )
      return NULL;

    node = ctc_listnode_new ();
    if ( !node )
      return NULL;

    node->prev = list->tail;
    node->data = val;

    if (list->head == NULL)
        list->head = node;
    else
        list->tail->next = node;
    list->tail = node;

    list->count++;

    return node;
}

/* Delete specific date pointer from the list. */
void
ctc_listnode_delete (struct ctc_linklist *list, void *val)
{
  struct ctc_listnode *node;

  if ( (!list) || (!val) )
    return;

  for (node = list->head; node; node = node->next)
  {
      if (node->data == val)
      {
          if (node->prev)
              node->prev->next = node->next;
          else
              list->head = node->next;

          if (node->next)
              node->next->prev = node->prev;
          else
              list->tail = node->prev;

          list->count--;

          ctc_listnode_free (node);

          return;
      }
  }
}

void ctc_listnode_delete_node (struct ctc_linklist * list, ctc_listnode_t * node)
{
     if ( (!list) || (!node) )
         return;

     if (node->prev)
         node->prev->next = node->next;
     else
         list->head = node->next;

     if (node->next)
         node->next->prev = node->prev;
     else
         list->tail = node->prev;

     list->count--;

     ctc_listnode_free (node);
}

/* Delete all listnode from the list. */
void
ctc_list_delete_all_node (struct ctc_linklist *list)
{
  struct ctc_listnode *node;
  struct ctc_listnode *next;

  for (node = list->head; node; node = next)
    {
      next = node->next;
      if (list->del)
	list->del(node->data);
      ctc_listnode_free (node);
    }
  list->head = list->tail = NULL;
  list->count = 0;
}

/* Delete all listnode then free list itself. */
void
ctc_list_delete (struct ctc_linklist *list)
{
  ctc_list_delete_all_node (list);
  ctc_list_free (list);
}

void
ctc_list_delete_list (struct ctc_linklist *list)
{
  struct ctc_listnode *node;
  struct ctc_listnode *next;

  if (!list)
    return;

  for (node = list->head; node; node = next)
    {
      next = node->next;
      if (list->del)
	list->del(node->data);
      ctc_listnode_free (node);
    }
  ctc_list_free (list);
}

/* Lookup the node which has given data. */
struct ctc_listnode *
ctc_listnode_lookup (struct ctc_linklist *list, void *data)
{
  struct ctc_listnode *node;

  for (node = list->head; node; CTC_NEXTNODE (node))
    if (data == CTC_GETDATA (node))
      return node;
  return NULL;
}



 void
ctc_list_pointer_init(ctc_list_pointer_t* p_list)
{
    _CTC_LHEAD(p_list) = NULL;
    _CTC_LTAIL(p_list) = &p_list->head;
    p_list->count = 0;
}

  void
ctc_list_pointer_insert_head(ctc_list_pointer_t* p_list, ctc_list_pointer_node_t* p_node)
{
    if ((p_node->p_next = _CTC_LHEAD(p_list)) != NULL)
        p_node->p_next->p_prev = p_node;
    else
        _CTC_LTAIL(p_list) = p_node;
    _CTC_LHEAD(p_list) = p_node;
    p_node->p_prev = &p_list->head;

       p_list->count ++;
}

  void
ctc_list_pointer_insert_tail(ctc_list_pointer_t* p_list, ctc_list_pointer_node_t* p_node)
{
    p_node->p_next = NULL;
    p_node->p_prev = _CTC_LTAIL(p_list);
    _CTC_LTAIL(p_list)->p_next = p_node;
    _CTC_LTAIL(p_list) = p_node;
     p_list->count ++;
}

void
ctc_list_pointer_insert_after(ctc_list_pointer_t* p_list, ctc_list_pointer_node_t* p_node1, ctc_list_pointer_node_t* p_node2)
{
      if ((p_node2->p_next = p_node1->p_next) != NULL)
          p_node2->p_next->p_prev = p_node2;
      else
          _CTC_LTAIL(p_list) = p_node2;
      p_node1->p_next = p_node2;
      p_node2->p_prev = p_node1;
      p_list->count ++;
}

 void
ctc_list_pointer_insert_before(ctc_list_pointer_t* p_list, ctc_list_pointer_node_t* p_node1, ctc_list_pointer_node_t* p_node2)
{
    p_node2->p_next = p_node1;
    p_node2->p_prev = p_node1->p_prev;
    p_node1->p_prev->p_next = p_node2;
    p_node1->p_prev = p_node2;
    p_list->count ++;
}

  ctc_list_pointer_node_t*
ctc_list_pointer_delete_head(ctc_list_pointer_t* p_list)
{
    ctc_list_pointer_node_t* p_node = _CTC_LHEAD(p_list);

    if ((_CTC_LHEAD(p_list) = p_node->p_next) != NULL)
        _CTC_LHEAD(p_list)->p_prev = &p_list->head;
    else
        _CTC_LTAIL(p_list) = &p_list->head;

    if ( p_list->count != 0)
    {
        p_list->count --;
    }
    return p_node;
}

  ctc_list_pointer_node_t*
ctc_list_pointer_delete_tail(ctc_list_pointer_t* p_list)
{
    ctc_list_pointer_node_t* p_node = _CTC_LTAIL(p_list);

    _CTC_LTAIL(p_list) = p_node->p_prev;
    _CTC_LTAIL(p_list)->p_next = NULL;
    if ( p_list->count != 0)
    {
        p_list->count --;
    }

    return p_node;
}

 void
ctc_list_pointer_delete(ctc_list_pointer_t* p_list, ctc_list_pointer_node_t* p_node)
{
    if (p_node->p_next != NULL)
        p_node->p_next->p_prev = p_node->p_prev;
    else
        _CTC_LTAIL(p_list) = p_node->p_prev;
    p_node->p_prev->p_next = p_node->p_next;

     if ( p_list->count != 0)
    {
        p_list->count --;
    }
}

 int
ctc_list_pointer_empty(ctc_list_pointer_t* p_list)
{
    return _CTC_LHEAD(p_list) == NULL;
}

 ctc_list_pointer_node_t*
ctc_list_pointer_head(ctc_list_pointer_t* p_list)
{
    return _CTC_LHEAD(p_list);
}

ctc_list_pointer_node_t*
ctc_list_pointer_node_tail(ctc_list_pointer_t* p_list)
{
    return _CTC_LTAIL(p_list)->p_prev->p_next;
}

ctc_list_pointer_node_t*
ctc_list_pointer_next(ctc_list_pointer_node_t* p_node)
{
    return p_node->p_next;
};

ctc_list_pointer_node_t*
ctc_list_pointer_prev(ctc_list_pointer_node_t* p_node)
{
    return p_node->p_prev->p_prev->p_next;
}
/*
 * ************************  single-linked List   ***************************
*/
struct ctc_slist_s *ctc_slist_new()
{
    struct ctc_slist_s *new;

    new = (struct ctc_slist_s *) mem_malloc(MEM_LINKLIST_MODULE, sizeof(struct ctc_slist_s));
    if (new != NULL)
    {
        kal_memset(new, 0, sizeof(struct ctc_slist_s ));
    }
    return new;

}
void ctc_slist_free (struct ctc_slist_s *slist)
{
    mem_free (slist);
    return ;
}

ctc_slistnode_t *ctc_slist_add_head (struct ctc_slist_s *slist, ctc_slistnode_t *node)
{

    if ( (!slist) || (!node) )
        return NULL;

    if (slist->head == NULL)
    {
        slist->head = node;
        slist->tail = node;
    }
    else
    {
       node->next = slist->head;
    }
    slist->head = node;

    slist->count++;
    return node;


}
ctc_slistnode_t *ctc_slist_add_tail (struct ctc_slist_s *slist,  ctc_slistnode_t *node)
{
    if ( (!slist) || (!node) )
        return NULL;

    if (slist->head == NULL)
    {
        slist->head = node;
        slist->tail = node;
    }
    else
        slist->tail->next = node;

    slist->tail = node;

    slist->count++;

    return node;

}
void ctc_slist_delete_node(struct ctc_slist_s *slist, ctc_slistnode_t *del_node)
{
     ctc_slistnode_t *node;
    ctc_slistnode_t *next_node;

    if ( (!slist) || (!del_node) )
        return;

    for (node = slist->head,next_node =node ?node->next:NULL;
           node; node = next_node,next_node =node ?node->next:NULL)
    {
        if ( node == del_node )
        { /*head node*/
           slist->head =  next_node;
           if(!next_node)
                slist->tail = NULL;
             slist->count--;
            break;
        }

        if (NULL != next_node && next_node == del_node)
        {
            node->next = next_node->next;

            if (!next_node->next)
               slist->tail = node;
             slist->count--;
            break;
        }
    }
    return ;

}

void ctc_slist_delete_node2(struct ctc_slist_s *slist,ctc_slistnode_t *pre_or_next_node, ctc_slistnode_t *del_node)
{

    if ( (!slist) || (!del_node) )
        return;


    if (slist->head == del_node)
    {
        /*pre_or_next_node is next node*/
        slist->head =  pre_or_next_node;
        if (!pre_or_next_node)
            slist->tail = pre_or_next_node;
        slist->count--;
    }
    else
    {
        if (!pre_or_next_node)
        {
            return ;
        }
        pre_or_next_node->next = del_node->next;

        if (!del_node->next)
            slist->tail = pre_or_next_node;
      slist->count--;
    }


}

void ctc_slist_delete (struct ctc_slist_s *slist)
{
  ctc_slist_delete_all_node (slist);
  ctc_slist_free (slist);

}
void ctc_slist_delete_all_node (struct ctc_slist_s * slist)
{
    if ( (!slist) )
        return;

    slist->head = NULL;
    slist->tail = NULL;
    return ;
}

