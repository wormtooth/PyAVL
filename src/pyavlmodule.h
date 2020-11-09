#ifndef PY_AVL_MODULE_H
#define PY_AVL_MODULE_H

#define PYAVL_VERSION_MAJOR 1
#define PYAVL_VERSION_MINOR 0
#define PYAVL_VERSION_MICRO 1

PyTypeObject TreeSet_Type;
#define TreeSetObj_Check(obj)    (Py_TYPE(obj) == &TreeSet_Type)

PyTypeObject TreeMap_Type;
#define TreeMapObj_Check(obj)    (Py_TYPE(obj) == &TreeMap_Type)

PyTypeObject AVLIter_Type;
#define AVLIterObj_Check(obj)    (Py_TYPE(obj) == &AVLIter_Type)

#endif