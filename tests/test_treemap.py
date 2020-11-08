import unittest
from pyavl.treemap import TreeMap
import random

class TreeMapTest(unittest.TestCase):
    
    def test_init(self):
        m = TreeMap({"c":2, "a":3, "b": 1})
        self.assertEqual(m.items(), [("a", 3), ("b", 1), ("c", 2)])
        self.assertEqual(len(m), 3)
        
        m = TreeMap([("b", 1), ("a", 3), ("c", 2)])
        self.assertEqual(m.items(), [("a", 3), ("b", 1), ("c", 2)])
        self.assertEqual(len(m), 3)
        
        m = TreeMap(c=2, a=3, b=1)
        self.assertEqual(m.items(), [("a", 3), ("b", 1), ("c", 2)])
        self.assertEqual(len(m), 3)

        data = [
            (random.randint(0, 1000), random.randint(-1000, 1000))
            for _ in range(10000)
        ]
        d = dict(data)
        m = TreeMap(data)
        self.assertEqual(m.items(), sorted(d.items()))
        self.assertEqual(len(m), len(d))
    
    def test_get(self):
        data = [(n, n) for n in range(1000)]
        d = dict(data)
        m = TreeMap(data)
        for _ in range(100):
            x = random.randint(0, 999)
            self.assertEqual(m[x], d[x])
            self.assertEqual(m.get(x), d.get(x))
        
        with self.assertRaises(KeyError):
            m[1000]
        
        # get default
        for _ in range(100):
            x = random.randint(1001, 2000)
            self.assertEqual(m.get(x), d.get(x)) # default is None
            self.assertEqual(m.get(x, 1), d.get(x, 1)) # defaul is 1
    
    def test_set(self):
        d = dict()
        m = TreeMap()
        for _ in range(10000):
            k = random.randint(1, 1000)
            v = random.randint(1, 1000)
            d[k] = v
            m[k] = v
            self.assertEqual(len(m), len(d))
            self.assertEqual(m[k], d[k])
        self.assertEqual(m.items(), sorted(d.items()))
    
    def test_del(self):
        data = [(n, n) for n in range(1000)]
        d = dict(data)
        m = TreeMap(data)
        for n in range(0, 1000, 2):
            del d[n]
            del m[n]
            self.assertEqual(len(m), len(d))
        self.assertEqual(m.items(), sorted(d.items()))

        with self.assertRaises(KeyError):
            del m[10000]
    
    def test_in(self):
        data = [
            (random.randint(0, 1000), random.randint(-1000, 1000))
            for _ in range(10000)
        ]
        d = dict(data)
        m = TreeMap(data)
        for _ in range(10000):
            k = random.randint(-1000, 1000)
            self.assertEqual(k in m, k in d)
    
    def test_update(self):
        data = [
            (random.randint(0, 1000), random.randint(-1000, 1000))
            for _ in range(10000)
        ]
        d = dict()
        m = TreeMap()

        d.update(data)
        m.update(data)
        self.assertEqual(len(m), len(d))
        self.assertEqual(m.items(), sorted(d.items()))
    
    def test_clear(self):
        m = TreeMap(
            (random.randint(0, 1000), random.randint(-1000, 1000))
            for _ in range(10000)
        )
        m.clear()
        self.assertEqual(m.items(), [])
        self.assertEqual(len(m), 0)
    
    def test_minmax(self):
        data = [
            (random.randint(-1000, 1000), random.randint(-1000, 1000))
            for _ in range(300)
        ]
        d = dict(data)
        m = TreeMap(data)

        self.assertEqual(m.min(), min(d.items()))
        self.assertEqual(m.max(), max(d.items()))



if __name__ == "__main__":
    unittest.main()