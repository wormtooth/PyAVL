import unittest
from pyavl import TreeSet, TreeMap
import random
import time

def timeit(n, func, *args, **kwargs):
    start = time.time()
    for _ in range(n):
        func(*args, **kwargs)
    return (time.time() - start) * 1000

class TreeSetBenchmarkTest(unittest.TestCase):
    
    def test_treeset_init(self):
        def f(data):
            TreeSet(data)
        def g(data):
            set(data)
        cnt = 100
        print()
        for i in range(3):
            data = [random.randint(-1000, 1000) for _ in range(1000 * (10 ** i))]
            t1 = timeit(cnt, f, data)
            t2 = timeit(cnt, g, data)
            s = set(data)
            print(f"Initialization with {len(data)} numbers ({len(s)} unique), run {cnt} times")
            print(f"TreeSet: {t1:.2f}ms, set: {t2:.2f}ms, TreeSet/set: {t1/t2:.2f}\n")
    
    def test_treeset_contain(self):
        def f(container, n):
            return n in container
        cnt = 100
        print()
        for i in range(4):
            N = 1000 * (10 ** i)
            ts = TreeSet(range(N))
            s = set(range(N))
            t1 = timeit(cnt, f, ts, N)
            t2 = timeit(cnt, f, s, N)
            print(f"Lookup missing target with {N} numbers, run {cnt} times")
            print(f"TreeSet: {t1:.2f}ms, set: {t2:.2f}ms, TreeSet/set: {t1/t2:.2f}\n")
    
    def test_treeset_min(self):
        def f(ts):
            ts.min()
        def g(s):
            min(s)
        cnt = 100
        print()
        for i in range(4):
            N = 1000 * (10 ** i)
            ts = TreeSet(range(N))
            s = set(range(N))
            t1 = timeit(cnt, f, ts)
            t2 = timeit(cnt, g, s)
            print(f"Find min in {N} numbers, run {cnt} times")
            print(f"TreeSet: {t1:.2f}ms, set: {t2:.2f}ms, set/TreeSet: {t2/t1:.2f}\n")


class TreeMapBenchmark(unittest.TestCase):

    def test_treemap_init(self):
        def f(data):
            TreeMap(data)
        def g(data):
            dict(data)
        cnt = 100
        print()
        for i in range(3):
            data = [
                (random.randint(-1000, 1000), random.randint(-1000, 1000))
                for _ in range(1000 * (10 ** i))
            ]
            t1 = timeit(cnt, f, data)
            t2 = timeit(cnt, g, data)
            d = dict(data)
            print(f"Initialization with {len(data)} pairs ({len(d)} unique keys), run {cnt} times")
            print(f"TreeMap: {t1:.2f}ms, dict: {t2:.2f}ms, TreeMap/dict: {t1/t2:.2f}\n")
    
    def test_treeset_contain(self):
        def f(container, n):
            return n in container
        cnt = 100
        print()
        for i in range(4):
            N = 1000 * (10 ** i)
            m = TreeMap(zip(range(N), range(N)))
            d = dict(zip(range(N), range(N)))
            t1 = timeit(cnt, f, m, N)
            t2 = timeit(cnt, f, d, N)
            print(f"Lookup missing target with {N} numbers, run {cnt} times")
            print(f"TreeMap: {t1:.2f}ms, dict: {t2:.2f}ms, TreeMap/dict: {t1/t2:.2f}\n")
    
    def test_treeset_min(self):
        def f(m):
            m.min()
        def g(d):
            min(d.items())
        cnt = 100
        print()
        for i in range(4):
            N = 1000 * (10 ** i)
            m = TreeMap(zip(range(N), range(N)))
            d = dict(zip(range(N), range(N)))
            t1 = timeit(cnt, f, m)
            t2 = timeit(cnt, g, d)
            print(f"Find min in {N} keys, run {cnt} times")
            print(f"TreeMap: {t1:.2f}ms, dict: {t2:.2f}ms, dict/TreeMap: {t2/t1:.2f}\n")

if __name__ == "__main__":
    unittest.main()