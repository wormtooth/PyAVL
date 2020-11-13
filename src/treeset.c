#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "avl.h"
#include "pyavlmodule.h"

typedef struct {
    PyObject_HEAD
    avl_node_t *root;
    Py_ssize_t size;
} TreeSetObj;

static PyObject* TreeSetObj_new(PyTypeObject *type, PyObject *args, PyObject *kwargs) {
    TreeSetObj *self;
    self = (TreeSetObj *)type->tp_alloc(type, 0);
    if (self == NULL) {
        return NULL;
    }
    self->root = NULL;
    self->size = 0;

    return (PyObject *)self;
}

static void TreeSetObj_free(TreeSetObj *self) {
    avl_node_free(self->root);
    Py_TYPE(self)->tp_free((PyObject *)self);
}

static PyObject* TreeSetObj_add(TreeSetObj *self, PyObject *args) {
    PyObject *key;
    if (!PyArg_ParseTuple(args, "O:add", &key))
        return NULL;
    int ret;
    avl_node_t *node = avl_node_new(key);
    self->root = avl_node_insert(self->root, node, &ret, NULL);
    if (ret == -1) {
        return NULL;
    } else if (ret == 0) {
        avl_node_free(node);
    }
    self->size += ret;
    Py_RETURN_NONE;
}

static PyObject* TreeSetObj_remove(TreeSetObj *self, PyObject *args) {
    PyObject *key;
    if (!PyArg_ParseTuple(args, "O:remove", &key))
        return NULL;
    avl_node_t *deleted;
    int ret;
    self->root = avl_node_delete(self->root, key, &ret, &deleted);
    if (ret == -1) {
        return NULL;
    } else if (ret == 1) {
        avl_node_free(deleted);
    }
    self->size -= ret;
    Py_RETURN_NONE;
}

static PyObject* TreeSetObj_extend_iter(TreeSetObj *self, PyObject *iter) {
    PyObject *key;
    while ((key = PyIter_Next(iter))) {
        int ret;
        avl_node_t *node = avl_node_new(key);
        self->root = avl_node_insert(self->root, node, &ret, NULL);
        Py_DECREF(key);
        if (ret == -1) {
            return NULL;
        } else if (ret == 0) {
            avl_node_free(node);
        }
        self->size += ret;
    }
    Py_RETURN_NONE;
}

static PyObject* TreeSetObj_extend(TreeSetObj *self, PyObject *args) {
    PyObject *obj;
    if (!PyArg_ParseTuple(args, "O:extend", &obj))
        return NULL;
    PyObject *iter = PyObject_GetIter(obj);
    if (iter == NULL) {
        PyErr_SetString(
            PyExc_TypeError, "AVLTree.extend must take an iterable as input.");
        return NULL;
    }
    PyObject *ret = TreeSetObj_extend_iter(self, iter);
    Py_DECREF(iter);

    return ret;
}

static PyObject* TreeSetObj_clear(TreeSetObj *self) {
    avl_node_free(self->root);
    self->root = NULL;
    self->size = 0;
    Py_RETURN_NONE;
}

static int TreeSetObj_init(TreeSetObj *self, PyObject *args) {
    
    if (args == NULL || PyTuple_Size(args) == 0)
        return 0;
    if (PyTuple_Size(args) != 1) {
        PyErr_SetString(
            PyExc_TypeError, "AVLTree.__init__ takes at most 1 positional argument."
        );
        return -1;
    }

    PyObject *obj;
    if (!PyArg_ParseTuple(args, "O", &obj))
        return -1;
    
    PyObject *iter = PyObject_GetIter(obj);
    if (!iter) {
        PyErr_SetString(
            PyExc_ValueError, "AVLTree.__init__ accepts only iterable."
        );
        return -1;
    }
    
    PyObject *ret = TreeSetObj_extend_iter(self, iter);
    Py_DECREF(iter);
    if (!ret) return -1;
    Py_DECREF(ret);
    
    return 0;
}

static PyObject* treeset_getkey(avl_node_t *node) {
    if (!node) {
        return NULL;
    }
    PyObject *key = AVL_KEY(node);
    Py_INCREF(key);
    return key;
}

static PyObject* TreeSetObj_iter(TreeSetObj *self) {
    return TreeIter_NewFromRoot(
        self->root, (avl_iter_getter)treeset_getkey
    );
}

static PyObject* TreeSetObj_min(TreeSetObj *self) {
    avl_node_t *root = self->root;
    if (!root) {
        PyErr_SetString(
            PyExc_ValueError,
            "TreeSet is empty"
        );
        return NULL;
    }
    while (AVL_LEFT(root)) {
        root = AVL_LEFT(root);
    }
    PyObject *ret = AVL_KEY(root);
    Py_INCREF(ret);
    return ret;
}

static PyObject* TreeSetObj_max(TreeSetObj *self) {
    avl_node_t *root = self->root;
    if (!root) {
        PyErr_SetString(
            PyExc_ValueError,
            "TreeSet is empty"
        );
        return NULL;
    }
    while (AVL_RIGHT(root)) {
        root = AVL_RIGHT(root);
    }
    PyObject *ret = AVL_KEY(root);
    Py_INCREF(ret);
    return ret;
}

static PyObject* TreeSetObj_loc(TreeSetObj *self, PyObject *args) {
    int idx;
    if (!PyArg_ParseTuple(args, "i:loc", &idx)) {
        return NULL;
    }
    int loc = idx;
    if (loc < 0) {
        loc += (int)self->size;
    }
    avl_node_t *node = avl_node_loc(self->root, loc);
    if (!node) {
        PyErr_SetString(
            PyExc_IndexError, "TreeSet index out of range"
        );
        return NULL;
    }
    PyObject *ret = AVL_KEY(node);
    Py_INCREF(ret);
    return ret;
}

static PyObject* TreeSetObj_at_most(TreeSetObj *self, PyObject *args) {
    PyObject *key;
    if (!PyArg_ParseTuple(args, "O:at_most", &key)) {
        return NULL;
    }
    int ret;
    avl_node_t *node = avl_node_at_most(self->root, key, &ret);
    if (ret < 0) {
        return NULL;
    } else if (ret == 0) {
        Py_RETURN_NONE;
    }
    key = AVL_KEY(node);
    Py_INCREF(key);
    return key;
}

static PyObject* TreeSetObj_at_least(TreeSetObj *self, PyObject *args) {
    PyObject *key;
    if (!PyArg_ParseTuple(args, "O:at_least", &key)) {
        return NULL;
    }
    int ret;
    avl_node_t *node = avl_node_at_least(self->root, key, &ret);
    if (ret < 0) {
        return NULL;
    } else if (ret == 0) {
        Py_RETURN_NONE;
    }
    key = AVL_KEY(node);
    Py_INCREF(key);
    return key;
}

static PyMethodDef TreeSetObj_Methods[] = {
    {
        "add",
        (PyCFunction)TreeSetObj_add,
        METH_VARARGS,
        "Add an object into the TreeSet."
    },
    {
        "at_most",
        (PyCFunction)TreeSetObj_at_most,
        METH_VARARGS,
        "Get the largest key in the TreeSet that is not bigger than the given key."
    },
    {
        "at_least",
        (PyCFunction)TreeSetObj_at_least,
        METH_VARARGS,
        "Get the smallest key in the TreeSet that is not smaller than the given key."
    },
    {
        "clear",
        (PyCFunction)TreeSetObj_clear,
        METH_NOARGS,
        "Clear the TreeSet."
    },
    {
        "extend",
        (PyCFunction)TreeSetObj_extend,
        METH_VARARGS,
        "Extends the TreeSet by an iterable."
    },
    {
        "loc",
        (PyCFunction)TreeSetObj_loc,
        METH_VARARGS,
        "Return the key at the given location."
    },
    {
        "max",
        (PyCFunction)TreeSetObj_max,
        METH_NOARGS,
        "Get the max of the TreeSet."
    },
    {
        "min",
        (PyCFunction)TreeSetObj_min,
        METH_NOARGS,
        "Get the min of the TreeSet."
    },
    {
        "remove",
        (PyCFunction)TreeSetObj_remove,
        METH_VARARGS,
        "Remove an object from the TreeSet."
    },
    {NULL}
};

/* sequence method */
static Py_ssize_t TreeSetObj_len(TreeSetObj *self) {
    return self->size;
}

static int TreeSetObj_contains(TreeSetObj *self, PyObject *key) {
    int ret;
    avl_node_find(self->root, key, &ret);
    return ret;
}

static PySequenceMethods TreeSetObj_Seq = {
    .sq_length = (lenfunc)TreeSetObj_len,
    .sq_contains = (objobjproc)TreeSetObj_contains
};

PyTypeObject TreeSet_Type = {
    /* The ob_type field must be initialized in the module init function
     * to be portable to Windows without using C++. */
    PyVarObject_HEAD_INIT(NULL, 0)
    "pyavl.TreeSet",            /*tp_name*/
    sizeof(TreeSetObj),         /*tp_basicsize*/
    0,                          /*tp_itemsize*/
    /* methods */
    (destructor)TreeSetObj_free,/*tp_dealloc*/
    0,                          /*tp_vectorcall_offset*/
    (getattrfunc)0,             /*tp_getattr*/
    (setattrfunc)0,             /*tp_setattr*/
    0,                          /*tp_as_async*/
    0,                          /*tp_repr*/
    0,                          /*tp_as_number*/
    &TreeSetObj_Seq,            /*tp_as_sequence*/
    0,                          /*tp_as_mapping*/
    0,                          /*tp_hash*/
    0,                          /*tp_call*/
    0,                          /*tp_str*/
    (getattrofunc)0,            /*tp_getattro*/
    0,                          /*tp_setattro*/
    0,                          /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT,         /*tp_flags*/
    0,                          /*tp_doc*/
    0,                          /*tp_traverse*/
    0,                          /*tp_clear*/
    0,                          /*tp_richcompare*/
    0,                          /*tp_weaklistoffset*/
    (getiterfunc)TreeSetObj_iter,/*tp_iter*/
    0,                          /*tp_iternext*/
    TreeSetObj_Methods,         /*tp_methods*/
    0,                          /*tp_members*/
    0,                          /*tp_getset*/
    0,                          /*tp_base*/
    0,                          /*tp_dict*/
    0,                          /*tp_descr_get*/
    0,                          /*tp_descr_set*/
    0,                          /*tp_dictoffset*/
    (initproc)TreeSetObj_init,  /*tp_init*/
    0,                          /*tp_alloc*/
    TreeSetObj_new,             /*tp_new*/
    0,                          /*tp_free*/
    0,                          /*tp_is_gc*/
};