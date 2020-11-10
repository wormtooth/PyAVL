#ifndef PY_AVL_MODULE_H
#define PY_AVL_MODULE_H

#define PYAVL_VERSION_MAJOR 1
#define PYAVL_VERSION_MINOR 1
#define PYAVL_VERSION_MICRO 0

PyTypeObject TreeSet_Type;
#define TreeSetObj_Check(obj)    (Py_TYPE(obj) == &TreeSet_Type)

PyTypeObject TreeMap_Type;
#define TreeMapObj_Check(obj)    (Py_TYPE(obj) == &TreeMap_Type)

/* TreeIter_Type */

/**
 * @brief Getter function, should return a new reference
 * 
 */
typedef PyObject* (*avl_iter_getter)(avl_node_t *);

PyTypeObject TreeIter_Type;
#define TreeIterObj_Check(obj)    (Py_TYPE(obj) == &TreeIter_Type)

extern PyObject*
TreeIter_NewFromRoot(avl_node_t *root, avl_iter_getter getter);

#endif