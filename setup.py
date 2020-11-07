from distutils.core import setup, Extension

TreeSetExt = Extension(
    "pyavl.treeset",
    sources=["src/avl.c", "src/treeset.c"]
)

with open("README.md", "r") as f:
    long_description = f.read()

setup(
    name="pyavl",
    packages=["pyavl"],
    version="0.1",
    description="TreeSet using AVL tree.",
    long_description=long_description,
    author="wormtooth",
    author_email="ye@wormtooth.com",
    maintainer="wormtooth",
    ext_modules=[
        TreeSetExt,
    ]
)