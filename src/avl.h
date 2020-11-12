/**
 * @file avl.h
 * @author wormtooth (ye@wormtooth.com)
 * @brief Interfaces for AVL tree, support insert, delete, find and foreach.
 * 
 * The basic type is `avl_node_t`, but it can be easily extended using `AVL_NODE_HEAD`.
 * For example,
 * ```
 * struct your_ext_t {
 *     AVL_NODE_HEAD
 *     int custom_field;
 * }
 * ```
 * Then `your_ext_t *` can be casted to `avl_node_t *` when used in insert, delete,
 * find and foreach.
 * 
 * @version 0.3
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#ifndef PY_AVL_H
#define PY_AVL_H

#include <stdint.h>

typedef struct _object PyObject;

typedef struct _avl_node {
    struct _avl_node *left;
    struct _avl_node *right;
    PyObject *key;
    uint64_t height:8;
    uint64_t size:56;
} avl_node_t;

typedef void (*avl_func)(avl_node_t *, void *);

/**
 * @brief Define the initial segment of every extension of avl_node_t.
 * 
 */
#define AVL_NODE_HEAD       avl_node_t _;

/**
 * @brief Left child of root.
 * 
 */
#define AVL_LEFT(root)      ((avl_node_t*)(root))->left

/**
 * @brief Right child of root.
 * 
 */
#define AVL_RIGHT(root)     ((avl_node_t*)(root))->right

/**
 * @brief Height of root.
 * 
 */
#define AVL_HEIGHT(root)    ((avl_node_t*)(root))->height

/**
 * @brief Height of root, but return 0 if root is NULL. Can only be rvalue.
 * 
 */
#define AVL_HEIGHT0(root)   ((root)? AVL_HEIGHT(root): 0)

/**
 * @brief Size of root.
 * 
 */
#define AVL_SIZE(root)    ((avl_node_t*)(root))->size

/**
 * @brief Size of root, but return 0 if root is NULL. Can only be rvalue.
 * 
 */
#define AVL_SIZE0(root)   ((root)? AVL_SIZE(root): 0)

/**
 * @brief Key of root.
 * 
 */
#define AVL_KEY(root)       ((avl_node_t*)(root))->key

/**
 * @brief Initialize a tree node with a given key.
 * 
 * @param node The node to initialize.
 * @param key The associated key to the node.
 */
extern void avl_node_init(avl_node_t *node, PyObject *key);

/**
 * @brief Undo intialization process.
 * 
 * @param node The node to clear.
 */
extern void avl_node_clear(avl_node_t *node);

/**
 * @brief Create a tree node with a given key.
 * This is only intended for avl_node_t. Do not use it for extensions of avl_node_t.
 * 
 * @param key The associated key to the node.
 * @return Return the created node on success, NULL on failure.
 */
extern avl_node_t* avl_node_new(PyObject *key);

/**
 * @brief Free an AVL tree properly.
 * This is only intended for avl_node_t. Do not use it for extensions of avl_node_t.
 * 
 * @param root The root of an AVL tree.
 */
extern void avl_node_free(avl_node_t* root);

/**
 * @brief Insert a key into an AVL tree.
 * 
 * @param root The root of an AVL tree.
 * @param node The node to insert.
 * @param ret The return code of insertion.
 * @param found The found node with the key.
 * @return Return the original tree on errors and return code is set to -1.
 * 
 * Return the original tree if node->key is presented already in the
 * tree and return code is set to 0. If found is not NULL, set it to
 * the node with the same key in the tree.
 * 
 * Return the modified tree if the node is successfully inserted and set
 * return code to 1.
 */
extern avl_node_t*
avl_node_insert(avl_node_t *root, avl_node_t *node, int *ret, avl_node_t **found);

/**
 * @brief Delete a key from an AVL tree.
 * 
 * @param root The root of an AVL tree.
 * @param key The key to delete.
 * @param ret The return code of deletion.
 * @param deleted The deleted node.
 * @return Return the original tree on errors and return code is set to -1.
 * 
 * Return the original tree if node->key is not presented in the
 * tree and return code is set to 0.
 * 
 * Return the modified tree if the node is successfully deleted and set
 * return code to 1. `deleted` is set to the deleted node with its left and
 * right children set to NULL.
 */
extern avl_node_t*
avl_node_delete(avl_node_t *root, PyObject *key, int *ret, avl_node_t **deleted);

/**
 * @brief Find a tree node by a key.
 * 
 * @param root The root of an tree.
 * @param key The key to find.
 * @param ret The return code of search.
 * @return Return NULL on errors and return code is set to -1.
 * 
 * Return NULL if the key is not presented in the tree and return code is set to 0.
 * 
 * Return the node with the given key if the key is presented in the tree and set
 * return code to 1.
 */
extern avl_node_t*
avl_node_find(avl_node_t *root, PyObject *key, int *ret);

/**
 * @brief Execute a function for all nodes in an AVL tree in order.
 * 
 * @param root The root of an AVL tree.
 * @param func The function to execute, should have signature
 * func(avl_node_t *node, void *extra).
 * @param extra Extra data to pass into the function.
 */
extern void
avl_node_foreach(avl_node_t *root, avl_func func, void *extra);

/**
 * @brief Get the node at a given position in an AVL tree.
 * 
 * @param root The root of an AVL tree.
 * @param loc The location index of the node to get, indexed from 0.
 * @return Return NULL if the index is out of range, and the node at given index
 * if the index is in the range.
 */
extern avl_node_t*
avl_node_at(avl_node_t *root, int loc);

/**
 *  `avl_iter_t` provides iterator protocol for `avl_node_t *`
 */

typedef struct _avl_iter {
    avl_node_t *next;
    avl_node_t *stack[128];
    int idx;
} avl_iter_t;

/**
 * @brief Create an iterator associated to an AVL tree.
 * 
 * @param root The root of an AVL tree.
 * @return Return the created iterator on success, NULL on failure.
 */
extern avl_iter_t* avl_iter_new(avl_node_t *root);

/**
 * @brief Free an iterator.
 * 
 * @param root The root of an AVL tree.
 */
extern void avl_iter_free(avl_iter_t *iter);

/**
 * @brief Return the next node in an AVL tree.
 * 
 * @param iter The iterator assoicated to an AVL tree.
 * @return Return the node on success, NULL if stopped. 
 */
extern avl_node_t* avl_iter_next(avl_iter_t *iter);

#endif