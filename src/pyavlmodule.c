#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "pyavlmodule.h"

static PyObject* pyavl_version(PyObject *self) {
    char buff[16];
    snprintf(
        buff, sizeof(buff), "%d.%d.%d",
        PYAVL_VERSION_MAJOR,
        PYAVL_VERSION_MINOR,
        PYAVL_VERSION_MICRO
    );
    return Py_BuildValue("s", buff);
}

static PyMethodDef pyavl_methods[] = {
    {
        "version",
        (PyCFunction)pyavl_version,
        METH_NOARGS,
        "Return the version of PyAVL"
    },
    {NULL}
};

static struct PyModuleDef pyavl_module = {
    PyModuleDef_HEAD_INIT,
    "pyavl",
    "Data Structures using AVL tree.",
    -1,
    pyavl_methods
};

PyMODINIT_FUNC PyInit_pyavl() {
    PyObject *m;

    if (PyType_Ready(&TreeSet_Type) < 0) {
        return NULL;
    }
    if (PyType_Ready(&TreeMap_Type) < 0) {
        return NULL;
    }

    m = PyModule_Create(&pyavl_module);
    if (m == NULL) {
        return NULL;
    }

    Py_INCREF(&TreeSet_Type);
    Py_INCREF(&TreeMap_Type);
    if (PyModule_AddObject(m, "TreeSet", (PyObject *)(&TreeSet_Type)) < 0) {
        goto error;
    }

    if (PyModule_AddObject(m, "TreeMap", (PyObject *)(&TreeMap_Type)) < 0) {
        goto error;
    }
    return m;
error:
    Py_DECREF(&TreeSet_Type);
    Py_DECREF(&TreeMap_Type);
    Py_DECREF(m);
    return NULL;
}