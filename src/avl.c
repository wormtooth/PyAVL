#include "avl.h"

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#define MAX_AVL_HEIGHT 128

/* Memory Management */

extern void avl_node_init(avl_node_t *node, PyObject *key) {
    AVL_HEIGHT(node) = 1;
    AVL_LEFT(node) = NULL;
    AVL_RIGHT(node) = NULL;
    Py_INCREF(key);
    AVL_KEY(node) = key;
}

extern void avl_node_clear(avl_node_t *node) {
    Py_DECREF(AVL_KEY(node));
    AVL_KEY(node) = NULL;
}

extern avl_node_t* avl_node_new(PyObject *key) {
    avl_node_t *node = (avl_node_t*)malloc(sizeof(avl_node_t));
    if (node) {
        avl_node_init(node, key);
    }
    return node;
}

extern void avl_node_free(avl_node_t *root) {
    if (!root) return;
    avl_node_clear(root);
    avl_node_free(AVL_LEFT(root));
    avl_node_free(AVL_RIGHT(root));
    free(root);
}

/* Tree Modification */

/**
 * @brief Compare two Python objects.
 * 
 * @param a Left operand.
 * @param b Right operand.
 * @return Return -2 on errors, -1 if a < b, 0 if a == b or 1 if a > b.
 */
static int _avl_py_cmp(PyObject *a, PyObject *b);

/**
 * @brief Implementation of avl_node_insert.
 */
static avl_node_t*
_avl_insert_helper(avl_node_t *tree, avl_node_t *node, int *ret, avl_node_t **found);

extern avl_node_t*
avl_node_insert(avl_node_t *root, avl_node_t *node, int *ret, avl_node_t **found) {
    return _avl_insert_helper(root, node, ret, found);
}

/**
 * @brief Implementation of avl_node_delete.
 */
static avl_node_t*
_avl_delete_helper(avl_node_t *root, PyObject *key, int *ret, avl_node_t **deleted);

extern avl_node_t*
avl_node_delete(avl_node_t *root, PyObject *key, int *ret, avl_node_t **deleted) {
    return _avl_delete_helper(root, key, ret, deleted);
}

/* Tree Utilities */

extern avl_node_t*
avl_node_find(avl_node_t *root, PyObject *key, int *ret) {
    while (root) {
        int cmp = _avl_py_cmp(key, AVL_KEY(root));
        if (cmp == -2) {
            *ret = -1;
            return NULL;
        } else if (cmp == 0) {
            *ret = 1;
            return root;
        } else if (cmp == -1) {
            root = AVL_LEFT(root);
        } else {
            root = AVL_RIGHT(root);
        }
    }

    *ret = 0;
    return NULL;
}

extern void
avl_node_foreach(avl_node_t *root, avl_func func, void *extra) {
    avl_node_t *stack[MAX_AVL_HEIGHT];
    int n = 0;

    while (n || root) {
        while (AVL_LEFT(root)) {
            stack[n++] = root;
            root = AVL_LEFT(root);
        }
        func(root, extra);
        while (!(AVL_RIGHT(root)) && n) {
            root = stack[--n];
            func(root, extra);
        }
        root = AVL_RIGHT(root);
    }
}

/* Implementation of Static Functions */

static int _avl_py_cmp(PyObject *a, PyObject *b) {
    int lt = PyObject_RichCompareBool(a, b, Py_LT);
    if (lt == -1) {
        return -2;
    } else if (lt == 1) {
        return -1;
    }
    
    int eq = PyObject_RichCompareBool(a, b, Py_EQ);
    if (eq == -1) {
        return -2;
    }

    return eq? 0: 1;
}

/*
      y                               x
    / \     Right Rotation          /  \
   x   T3   - - - - - - - >        T1   y 
  / \       < - - - - - - -            / \
 T1  T2     Left Rotation            T2  T3

*/

static avl_node_t* _avl_right_rotate(avl_node_t *y) {
    avl_node_t *x = AVL_LEFT(y);
    avl_node_t *T2 = AVL_RIGHT(x);

    AVL_RIGHT(x) = y;
    AVL_LEFT(y) = T2;

    AVL_HEIGHT(y) = Py_MAX(AVL_HEIGHT0(AVL_LEFT(y)), AVL_HEIGHT0(AVL_RIGHT(y))) + 1;
    AVL_HEIGHT(x) = Py_MAX(AVL_HEIGHT0(AVL_LEFT(x)), AVL_HEIGHT0(AVL_RIGHT(x))) + 1;

    return x;
}

static avl_node_t* _avl_left_rotate(avl_node_t *x) {
    avl_node_t *y = AVL_RIGHT(x);
    avl_node_t *T2 = AVL_LEFT(y);

    AVL_LEFT(y) = x;
    AVL_RIGHT(x) = T2;

    AVL_HEIGHT(x) = Py_MAX(AVL_HEIGHT0(AVL_LEFT(x)), AVL_HEIGHT0(AVL_RIGHT(x))) + 1;
    AVL_HEIGHT(y) = Py_MAX(AVL_HEIGHT0(AVL_LEFT(y)), AVL_HEIGHT0(AVL_RIGHT(y))) + 1;

    return y;
}

static avl_node_t*
_avl_insert_helper(avl_node_t *root, avl_node_t *node, int *ret, avl_node_t **found) {
    if (!root) {
        *ret = 1;
        return node;
    }
    int cmp = _avl_py_cmp(AVL_KEY(node), AVL_KEY(root));
    if (cmp == -2) {
        *ret = -1;
        return root;
    } else if (cmp == 0) {
        *ret = 0;
        if (found) {
            *found = root;
        }
        return root;
    } else if (cmp == -1) {
        AVL_LEFT(root) = _avl_insert_helper(AVL_LEFT(root), node, ret, found);
    } else {
        AVL_RIGHT(root) = _avl_insert_helper(AVL_RIGHT(root), node, ret, found);
    }

    int lh = AVL_HEIGHT0(AVL_LEFT(root));
    int rh = AVL_HEIGHT0(AVL_RIGHT(root));
    AVL_HEIGHT(root) = Py_MAX(lh, rh) + 1;
    int balance = lh - rh;

    if (balance > 1) {
        cmp = _avl_py_cmp(AVL_KEY(node), AVL_KEY(AVL_LEFT(root)));
        if (cmp == -2) {
            *ret = -1;
            return root;
        } else if (cmp == -1) {
            return _avl_right_rotate(root);
        } else if (cmp == 1) {
            AVL_LEFT(root) = _avl_left_rotate(AVL_LEFT(root));
            return _avl_right_rotate(root);
        }
    } else if (balance < -1) {
        cmp = _avl_py_cmp(AVL_KEY(node), AVL_KEY(AVL_RIGHT(root)));
        if (cmp == -2) {
            *ret = -1;
            return root;
        } else if (cmp == -1) {
            AVL_RIGHT(root) = _avl_right_rotate(AVL_RIGHT(root));
            return _avl_left_rotate(root);
        } else if (cmp == 1) {
            return _avl_left_rotate(root);
        }
    }

    return root;
}

static avl_node_t*
_avl_delete_helper(avl_node_t *root, PyObject *key, int *ret, avl_node_t **deleted) {
    if (!root) {
        *deleted = NULL;
        *ret = 0;
        return root;
    }

    int cmp = _avl_py_cmp(key, AVL_KEY(root));
    if (cmp == -2) {
        *ret = -1;
        return root;
    } else if (cmp == -1) {
        AVL_LEFT(root) = _avl_delete_helper(AVL_LEFT(root), key, ret, deleted);
    } else if (cmp == 1) {
        AVL_RIGHT(root) = _avl_delete_helper(AVL_RIGHT(root), key, ret, deleted);
    } else {
        *deleted = root;
        if (!(AVL_LEFT(root)) || !(AVL_RIGHT(root))) {
            root = AVL_LEFT(root)? AVL_LEFT(root): AVL_RIGHT(root);
        } else {
            avl_node_t *tmp = AVL_RIGHT(root);
            while (AVL_LEFT(tmp))
                tmp = AVL_LEFT(tmp);
            AVL_RIGHT(root) = _avl_delete_helper(
                AVL_RIGHT(root), AVL_KEY(tmp), ret, &tmp);
            AVL_LEFT(tmp) = AVL_LEFT(root);
            AVL_RIGHT(tmp) = AVL_RIGHT(root);
            root = tmp;
        }
        *ret = 1;
        AVL_LEFT(*deleted) = NULL;
        AVL_RIGHT(*deleted) = NULL;
    }

    if (!root)
        return NULL;

    int lh = AVL_HEIGHT0(AVL_LEFT(root));
    int rh = AVL_HEIGHT0(AVL_RIGHT(root));
    AVL_HEIGHT(root) = Py_MAX(lh, rh) + 1;
    int balance = lh - rh;

    if (balance > 1) {
        avl_node_t *node = AVL_LEFT(root);
        balance = AVL_HEIGHT0(AVL_LEFT(node)) - AVL_HEIGHT0(AVL_RIGHT(node));
        if (balance >= 0) {
            return _avl_right_rotate(root);
        } else {
            AVL_LEFT(root) = _avl_left_rotate(node);
            return _avl_right_rotate(root);
        }
    } else if (balance < -1) {
        avl_node_t *node = AVL_RIGHT(root);
        balance = AVL_HEIGHT0(AVL_LEFT(node)) - AVL_HEIGHT0(AVL_RIGHT(node));
        if (balance <= 0) {
            return _avl_left_rotate(root);
        } else {
            AVL_RIGHT(root) = _avl_right_rotate(node);
            return _avl_left_rotate(root);
        }
    }

    return root;
}