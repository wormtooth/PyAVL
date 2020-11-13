import unittest
from pyavl import TreeSet
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
            sorted(s), list(ts),
            "TreeSet.keys fails")
    
    def test_init(self):
        data = [random.randint(-1000, 1000) for _ in range(100000)]
        s = set(data)
        ts = TreeSet(data)
        self.assertEqual(
            sorted(s), list(ts),
            "TreeSet.__init__ fails")
    
    def test_extend(self):
        data = [random.randint(-1000, 1000) for _ in range(100000)]
        s = set(data)
        ts = TreeSet()
        ts.extend(data)
        self.assertEqual(
            sorted(s), list(ts),
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
                sorted(s), list(ts),
                "TreeSet.remove fails")
    
    def test_clear(self):
        data = list(range(100))
        random.shuffle(data)
        ts = TreeSet(data)
        ts.clear()
        self.assertEqual(
            list(ts), [], "TreeSet.clear fails"
        )
        self.assertEqual(
            len(ts), 0, "TreeSet.clear fails"
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
    
    def test_at(self):
        s = TreeSet(range(100))
        keys = list(s)
        # test loc
        for i, n in enumerate(keys):
            self.assertEqual(n, s.loc(i))
        with self.assertRaises(IndexError):
            s.loc(101)
        # test at_most
        self.assertEqual(s.at_most(23.3), 23)
        # test at_least
        self.assertEqual(s.at_least(23.3), 24)

if __name__ == "__main__":
    unittest.main()