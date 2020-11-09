# PyAVL

**PyAVL** is a Python library for AVL tree, a self-balancing binary search tree. It supports TreeSet and TreeMap.

**TreeSet**

```python
>>> from pyavl import TreeSet
>>> ts = TreeSet([3, 4, 1, 8, -2])
>>> ts.keys()
[-2, 1, 3, 4, 8]
>>> 1 in ts
True
>>> ts.remove(1)
>>> 1 in ts
False
>>> 9 in ts
False
>>> ts.add(9)
>>> 9 in ts
True
>>> ts.min(), ts.max()
(-2, 9)
>>> ts.clear()
>>> ts.keys()
[]
```

**TreeMap**

```python
>>> from pyavl import TreeMap
>>> m = TreeMap([("d", 3), ("a", 5), ("c", -1), ("b", 2), ("a", 0)])
>>> m["a"]
0
>>> "a" in m, "e" in m
(True, False)
>>> m["a"] = 10
>>> m.items()
[('a', 10), ('b', 2), ('c', -1), ('d', 3)]
>>> m.update({"e": 4, "b": -100})
>>> m.items()
[('a', 10), ('b', -100), ('c', -1), ('d', 3), ('e', 4)]
>>> m.min()
('a', 10)
>>> del m["c"]
>>> m.values()
[10, -100, 3, 4]
>>> m.clear()
>>> m.keys()
[]
```

## Installing PyAVL

**PyAVL** is tested on Python3.7+. To install:

```console
$ git clone https://github.com/wormtooth/PyAVL.git PyAVL
$ cd PyAVL
$ python setup.py install
```

*You might need to change `python` above to `python3`.*

## Tests and Benchmarks

Tests and benchmarks can be found in the **tests** folders. To run tests:

```console
$ cd tests
$ python -m unittest discover -v
```

To run benchmarks in the folder of **tests**:

```console
$ python benchmark.py
```

*You might need to change `python` above to `python3`.*

## License

MIT