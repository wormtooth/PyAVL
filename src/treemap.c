#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "avl.h"
#include "pyavlmodule.h"

typedef struct {
    AVL_NODE_HEAD
    PyObject *val;
} avl_map_t;

static avl_map_t* avl_map_new(PyObject *key, PyObject *val) {
    avl_map_t *obj = (avl_map_t *)malloc(sizeof(avl_map_t));
    if (obj) {
        avl_node_init((avl_node_t *)obj, key);
        Py_INCREF(val);
        obj->val = val;
    }
    return obj;
}

static void avl_map_free(avl_map_t *root) {
    if (!root) return;
    avl_node_clear((avl_node_t *)root);
    Py_DECREF(root->val);
    avl_map_free((avl_map_t *)AVL_LEFT(root));
    avl_map_free((avl_map_t *)AVL_RIGHT(root));
    free(root);
}

typedef struct {
    PyObject_HEAD
    avl_map_t *root;
    Py_ssize_t size;
} TreeMapObj;

static PyObject* TreeMapObj_new(PyTypeObject *type, PyObject *args, PyObject *kwargs) {
    TreeMapObj *self;
    self = (TreeMapObj *)type->tp_alloc(type, 0);
    if (self == NULL) {
        return NULL;
    }
    self->root = NULL;
    self->size = 0;
    return (PyObject *)self;
}

static void TreeMapObj_free(TreeMapObj *self) {
    avl_map_free(self->root);
    Py_TYPE(self)->tp_free((PyObject *)self);
}

/* Methods Declaration */

static PyObject* TreeMapObj_clear(TreeMapObj *self) {
    avl_map_free(self->root);
    self->root = NULL;
    self->size = 0;
    Py_RETURN_NONE;
}

static PyObject* TreeMapObj_get(TreeMapObj *self, PyObject *args) {
    int argc = args? PyTuple_Size(args): 0;
    if (argc == 0 || argc > 2) {
        PyErr_SetString(
            PyExc_ValueError,
            "TreeMap.get takes 1 or 2 positional arguments."
        );
        return NULL;
    }
    PyObject *key;
    PyObject *ret = Py_None;
    int parse;
    if (argc == 1) {
        parse = PyArg_ParseTuple(args, "O:get", &key);
    } else {
        parse = PyArg_ParseTuple(args, "OO:get", &key, &ret);
    }
    if (!parse) {
        return NULL;
    }

    int code;
    avl_map_t *found = (avl_map_t *)avl_node_find(
        (avl_node_t *)self->root, key, &code);
    if (code == -1) {
        PyErr_Clear();
    } else if (code == 1) {
        ret = found->val;
    }

    Py_INCREF(ret);
    return ret;
}

static void add_key_to_list(avl_map_t *node, PyObject *list) {
    PyList_Append(list, AVL_KEY(node));
}

static PyObject* TreeMapObj_keys(TreeMapObj *self) {
    PyObject *list = PyList_New(0);
    avl_node_foreach(
        (avl_node_t *)self->root, (avl_func)add_key_to_list, (void *)list
    );
    return list;
}

static void add_val_to_list(avl_map_t *node, PyObject *list) {
    PyList_Append(list, node->val);
}

static PyObject* TreeMapObj_values(TreeMapObj *self) {
    PyObject *list = PyList_New(0);
    avl_node_foreach(
        (avl_node_t *)self->root, (avl_func)add_val_to_list, (void *)list
    );
    return list;
}

static void add_item_to_list(avl_map_t *node, PyObject *list) {
    PyObject *item = Py_BuildValue("(OO)", AVL_KEY(node), node->val);
    PyList_Append(list, item);
    Py_DECREF(item);
}

static PyObject* TreeMapObj_items(TreeMapObj *self) {
    PyObject *list = PyList_New(0);
    avl_node_foreach(
        (avl_node_t *)self->root, (avl_func)add_item_to_list, (void *)list
    );
    return list;
}

static int treemap_insert(TreeMapObj *self, PyObject *key, PyObject *val) {
    avl_map_t *node = avl_map_new(key, val);
    if (!node) {
        PyErr_NoMemory();
        return -1;
    }
    int ret;
    avl_map_t *found;
    self->root = (avl_map_t *)avl_node_insert(
        (avl_node_t *)self->root, (avl_node_t *)node,
        &ret, (avl_node_t **)&found
    );
    if (ret == -1) {
        return -1;
    } else if (ret == 0) {
        avl_map_free(node);
        Py_DECREF(found->val);
        Py_INCREF(val);
        found->val = val;
    } else {
        self->size ++;
    }
    return 0;
}

static int treemap_update(TreeMapObj *self, PyObject *mapping) {
    if (!mapping) return 0;
    int ret;
    if (!PyDict_Check(mapping)) {
        PyObject *mp = PyDict_New();
        ret = -1;
        if (PyDict_MergeFromSeq2(mp, mapping, 1) == 0) {
            ret = treemap_update(self, mp);
        }
        Py_DECREF(mp);
        return ret;
    }
    PyObject *key, *val;
    Py_ssize_t pos = 0;
    while (PyDict_Next(mapping, &pos, &key, &val)) {
        ret = treemap_insert(self, key, val);
        if (ret < 0) {
            return ret;
        }
    }
    return 0;
}

static PyObject* TreeMapObj_update(TreeMapObj *self, PyObject *args) {
    PyObject *obj;
    if (!PyArg_ParseTuple(args, "O:update", &obj)) {
        return NULL;
    }
    if (treemap_update(self, obj) < 0) {
        return NULL;
    }
    Py_RETURN_NONE;
}

static PyObject* TreeMapObj_min(TreeMapObj *self) {
    avl_map_t *root = self->root;
    if (!root) {
        PyErr_SetString(
            PyExc_ValueError,
            "TreeSet is empty"
        );
        return NULL;
    }
    while (AVL_LEFT(root)) {
        root = (avl_map_t *)AVL_LEFT(root);
    }
    PyObject *key = AVL_KEY(root);
    PyObject *val = root->val;
    return Py_BuildValue("(OO)", key, val);
}

static PyObject* TreeMapObj_max(TreeMapObj *self) {
    avl_map_t *root = self->root;
    if (!root) {
        PyErr_SetString(
            PyExc_ValueError,
            "TreeSet is empty"
        );
        return NULL;
    }
    while (AVL_RIGHT(root)) {
        root = (avl_map_t *)AVL_RIGHT(root);
    }
    PyObject *key = AVL_KEY(root);
    PyObject *val = root->val;
    return Py_BuildValue("(OO)", key, val);
}

/* init */
static PyObject*
TreeMapObj_init(TreeMapObj *self, PyObject *args, PyObject *kwargs) {
    if (args && PyTuple_Size(args) > 1) {
        PyErr_SetString(
            PyExc_ValueError,
            "TreeMap.__init__ takes at most 1 positional argument."
        );
        return NULL;
    }

    if (PyTuple_Size(args) == 1) {
        PyObject *obj;
        if (!PyArg_ParseTuple(args, "O:__init__", &obj)) {
            return NULL;
        }
        if (treemap_update(self, obj) < 0) {
            return NULL;
        }
    }

    if (treemap_update(self, kwargs) < 0) {
        return NULL;
    }

    Py_RETURN_NONE;
}

/* Mapping Protocol */

static Py_ssize_t TreeMapObj_length(TreeMapObj *self) {
    return self->size;
}

static PyObject* TreeMapObj_subscript(TreeMapObj *self, PyObject *key) {
    int ret;
    avl_map_t *found = (avl_map_t *)avl_node_find(
        (avl_node_t *)self->root, key, &ret
    );

    if (ret <= 0) {
        _PyErr_SetKeyError(key);
        return NULL;
    }
    PyObject *val = found->val;
    Py_INCREF(val);
    return val;
}

static int treemap_delete(TreeMapObj *self, PyObject *key, avl_map_t **deleted) {
    int ret;
    avl_map_t *tmp = NULL;
    self->root = (avl_map_t *)avl_node_delete(
        (avl_node_t *)self->root, key,
        &ret, (avl_node_t **)&tmp
    );
    if (ret == -1) {
        return -1;
    } else if (ret == 0) {
        _PyErr_SetKeyError(key);
        return -1;
    }
    if (deleted) {
        *deleted = tmp;
    } else {
        avl_map_free(tmp);
    }
    self->size --;
    return 0;
}

static int TreeMapObj_ass_sub(TreeMapObj *self, PyObject *key, PyObject *val) {
    if (!val) {
        return treemap_delete(self, key, NULL);
    }
    return treemap_insert(self, key, val);
}

static PyMappingMethods TreeMapObj_Mapping = {
    (lenfunc)TreeMapObj_length,         // mp_length
    (binaryfunc)TreeMapObj_subscript,   // mp_subscript
    (objobjargproc)TreeMapObj_ass_sub,  // mp_ass_subscript
};

/* Sequence Protocol */
static int TreeMapObj_contains(TreeMapObj *self, PyObject *key) {
    int ret;
    avl_node_find((avl_node_t *)self->root, key, &ret);
    return ret;
}

static PySequenceMethods TreeMapObj_Sequence = {
    .sq_contains = (objobjproc)TreeMapObj_contains
};

static PyMethodDef TreeMapObj_Methods[] = {
    {
        "clear",
        (PyCFunction)TreeMapObj_clear,
        METH_NOARGS,
        "Remove all items from the TreeMap."
    },
    {
        "get",
        (PyCFunction)TreeMapObj_get,
        METH_VARARGS,
        "Return the value for key if key is in the TreeMap, else default."
    },
    {
        "keys",
        (PyCFunction)TreeMapObj_keys,
        METH_NOARGS,
        "Get all keys of the TreeMap in an sorted list."
    },
    {
        "max",
        (PyCFunction)TreeMapObj_max,
        METH_NOARGS,
        "Get the (key, val) pair with maximal key in the TreeMap."
    },
    {
        "min",
        (PyCFunction)TreeMapObj_min,
        METH_NOARGS,
        "Get the (key, val) pair with minimal key in the TreeMap."
    },
    {
        "values",
        (PyCFunction)TreeMapObj_values,
        METH_NOARGS,
        "Get all values of the TreeMap, ordered by their keys."
    },
    {
        "items",
        (PyCFunction)TreeMapObj_items,
        METH_NOARGS,
        "Get all (key, value) pairs of the TreeMap, ordered by key."
    },
    {
        "update",
        (PyCFunction)TreeMapObj_update,
        METH_VARARGS,
        "Update the TreeMap by a dict, the argument will be converted to a dict if needed."
    },
    {NULL}
};

PyTypeObject TreeMap_Type = {
    /* The ob_type field must be initialized in the module init function
     * to be portable to Windows without using C++. */
    PyVarObject_HEAD_INIT(NULL, 0)
    "pyavl.TreeMap",            /*tp_name*/
    sizeof(TreeMapObj),         /*tp_basicsize*/
    0,                          /*tp_itemsize*/
    /* methods */
    (destructor)TreeMapObj_free,/*tp_dealloc*/
    0,                          /*tp_vectorcall_offset*/
    (getattrfunc)0,             /*tp_getattr*/
    (setattrfunc)0,             /*tp_setattr*/
    0,                          /*tp_as_async*/
    0,                          /*tp_repr*/
    0,                          /*tp_as_number*/
    &TreeMapObj_Sequence,       /*tp_as_sequence*/
    &TreeMapObj_Mapping,        /*tp_as_mapping*/
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
    TreeMapObj_Methods,         /*tp_methods*/
    0,                          /*tp_members*/
    0,                          /*tp_getset*/
    0,                          /*tp_base*/
    0,                          /*tp_dict*/
    0,                          /*tp_descr_get*/
    0,                          /*tp_descr_set*/
    0,                          /*tp_dictoffset*/
    (initproc)TreeMapObj_init,  /*tp_init*/
    0,                          /*tp_alloc*/
    TreeMapObj_new,             /*tp_new*/
    0,                          /*tp_free*/
    0,                          /*tp_is_gc*/
};
