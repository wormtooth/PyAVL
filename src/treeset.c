#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "avl.h"

typedef struct {
    PyObject_HEAD
    avl_node_t *root;
    Py_ssize_t size;
} TreeSetObj;

static PyTypeObject TreeSet_Type;

#define TreeSetObj_Check(obj)    (Py_TYPE(obj) == &TreeSet_Type)

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

static void add_key_to_list(avl_node_t *node, PyObject *list) {
    PyList_Append(list, AVL_KEY(node));
}

static PyObject* TreeSetObj_keys(TreeSetObj *self) {
    PyObject *list = PyList_New(0);
    avl_node_foreach(self->root, (avl_func)add_key_to_list, (void *)list);
    return list;
}

static PyObject* TreeSetObj_size(TreeSetObj *self) {
    return Py_BuildValue("n", self->size);
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

static PyMethodDef TreeSetObj_Methods[] = {
    {
        "add",
        (PyCFunction)TreeSetObj_add,
        METH_VARARGS,
        "Add an object into the TreeSet."
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
        "keys",
        (PyCFunction)TreeSetObj_keys,
        METH_NOARGS,
        "Get all keys of the TreeSet in an sorted list."
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
    {
        "size",
        (PyCFunction)TreeSetObj_size,
        METH_NOARGS,
        "Get the size of the TreeSet."
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

static PyTypeObject TreeSet_Type = {
    /* The ob_type field must be initialized in the module init function
     * to be portable to Windows without using C++. */
    PyVarObject_HEAD_INIT(NULL, 0)
    "pyavl.treeset.TreeSet",    /*tp_name*/
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
    0,                          /*tp_iter*/
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

static struct PyModuleDef treeset_module = {
    PyModuleDef_HEAD_INIT,
    "treeset",
    "TreeSet using AVL tree.",
    -1,
};

PyMODINIT_FUNC PyInit_treeset() {
    PyObject *m;

    if (PyType_Ready(&TreeSet_Type) < 0) {
        return NULL;
    }
    m = PyModule_Create(&treeset_module);
    if (m == NULL) {
        return NULL;
    }

    Py_INCREF(&TreeSet_Type);
    if (PyModule_AddObject(m, "TreeSet", (PyObject *)(&TreeSet_Type)) < 0) {
        Py_DECREF(&TreeSet_Type);
        Py_DECREF(m);
        return NULL;
    }

    return m;
}