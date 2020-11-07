# PyAVL

**PyAVL** is a Python library for AVL tree, a self-balancing binary search tree. It supports TreeSet and support for TreeMap is under development.

```python
>>> from pyavl.treeset import TreeSet
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

## Installing PyAVL

**PyAVL** is tested on Python3.7+. To install:

```console
$ git clone https://github.com/wormtooth/PyAVL.git PyAVL
$ cd PyAVL
$ python setup.py install
```

*You might need to change `python` above to `python3`.*

## Tests and Benchmarks

Tests and benchmarks can be found in the **tests** folders. To run:

```
$ cd tests
$ python -m unittest discover -v
```

*You might need to change `python` above to `python3`.*

## License

MIT