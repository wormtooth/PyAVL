#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "avl.h"
#include "pyavlmodule.h"

typedef struct {
    PyObject_HEAD
    avl_iter_t *iter;
    avl_iter_getter getter;
} TreeIterObj;

static PyObject* TreeIterObj_new(PyTypeObject *type, PyObject *args, PyObject *kwargs) {
    TreeIterObj *self;
    self = (TreeIterObj *)type->tp_alloc(type, 0);
    if (!self) {
        return NULL;
    }
    self->iter = NULL;
    self->getter = NULL;

    return (PyObject *)self;
}

static void TreeIterObj_free(TreeIterObj *self) {
    avl_iter_free(self->iter);
    Py_TYPE(self)->tp_free((PyObject *)self);
}

extern PyObject*
TreeIter_NewFromRoot(avl_node_t *root, avl_iter_getter getter) {
    TreeIterObj *self = (TreeIterObj *)TreeIterObj_new(&TreeIter_Type, NULL, NULL);
    if (!self) {
        return NULL;
    }
    self->iter = avl_iter_new(root);
    self->getter = getter;
    return (PyObject *)self;
}

static PyObject* TreeIter_next(TreeIterObj *self) {
    if (!(self->iter) || !(self->getter)) {
        return NULL;
    }
    avl_node_t *node = avl_iter_next(self->iter);
    if (!node) {
        return NULL;
    }
    
    avl_iter_getter getter = self->getter;
    return getter(node);
}

PyTypeObject TreeIter_Type = {
    /* The ob_type field must be initialized in the module init function
     * to be portable to Windows without using C++. */
    PyVarObject_HEAD_INIT(NULL, 0)
    "pyavl._TreeIter",          /*tp_name*/
    sizeof(TreeIterObj),        /*tp_basicsize*/
    0,                          /*tp_itemsize*/
    /* methods */
    (destructor)TreeIterObj_free,/*tp_dealloc*/
    0,                          /*tp_vectorcall_offset*/
    (getattrfunc)0,             /*tp_getattr*/
    (setattrfunc)0,             /*tp_setattr*/
    0,                          /*tp_as_async*/
    0,                          /*tp_repr*/
    0,                          /*tp_as_number*/
    0,                          /*tp_as_sequence*/
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
    PyObject_SelfIter,          /*tp_iter*/
    (iternextfunc)TreeIter_next,/*tp_iternext*/
    0,                          /*tp_methods*/
    0,                          /*tp_members*/
    0,                          /*tp_getset*/
    0,                          /*tp_base*/
    0,                          /*tp_dict*/
    0,                          /*tp_descr_get*/
    0,                          /*tp_descr_set*/
    0,                          /*tp_dictoffset*/
    0,                          /*tp_init*/
    0,                          /*tp_alloc*/
    TreeIterObj_new,            /*tp_new*/
    0,                          /*tp_free*/
    0,                          /*tp_is_gc*/
};