import unittest
from pyavl.treeset import TreeSet
import random

class TreeSetTest(unittest.TestCase):
    
    def test_insert(self):
        s = set()
        ts = TreeSet()
        for _ in range(100000):
            x = random.randint(-1000, 1000)
            s.add(x)
            ts.add(x)
            self.assertEqual(
                len(s), len(ts),
                "size of TreeSet is wrong.")

        self.assertEqual(
            sorted(s), ts.keys(),
            "TreeSet.keys fails")
    
    def test_init(self):
        data = [random.randint(-1000, 1000) for _ in range(100000)]
        s = set(data)
        ts = TreeSet(data)
        self.assertEqual(
            sorted(s), ts.keys(),
            "TreeSet.__init__ fails")
    
    def test_extend(self):
        data = [random.randint(-1000, 1000) for _ in range(100000)]
        s = set(data)
        ts = TreeSet()
        ts.extend(data)
        self.assertEqual(
            sorted(s), ts.keys(),
            "TreeSet.extend fails")
    
    def test_remove(self):
        data = [random.randint(-1000, 1000) for _ in range(10000)]
        s = set(data)
        ts = TreeSet(data)
        for _ in range(100):
            x = random.randint(-1000, 1000)
            if x in s:
                s.remove(x)
            ts.remove(x)
            self.assertEqual(
                sorted(s), ts.keys(),
                "TreeSet.remove fails")
    
    def test_clear(self):
        data = list(range(100))
        random.shuffle(data)
        ts = TreeSet(data)
        ts.clear()
        self.assertEqual(
            ts.keys(), [], "TreeSet.clear fails"
        )
        self.assertEqual(
            ts.size(), 0, "TreeSet.clear fails"
        )
    
    def test_minmax(self):
        for _ in range(100):
            data = [random.randint(-1000, 1000) for _ in range(10000)]
            ts = TreeSet(data)
            self.assertEqual(
                min(data), ts.min(), "TreeSet.min fails"
            )
            self.assertEqual(
                max(data), ts.max(), "TreeSet.max fails"
            )

if __name__ == "__main__":
    unittest.main()