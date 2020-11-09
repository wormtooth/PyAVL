from distutils.core import setup, Extension
import glob

def get_version():
    vermap = {
        "major": "#define PYAVL_VERSION_MAJOR",
        "minor": "#define PYAVL_VERSION_MINOR",
        "micro": "#define PYAVL_VERSION_MICRO"
    }
    ver = {}
    with open("src/pyavlmodule.h", "r") as f:
        for line in f:
            for k, v in vermap.items():
                if k in ver:
                    continue
                if line.startswith(v):
                    line = line[len(v):].strip()
                    ver[k] = int(line)
    return "{}.{}.{}".format(
        ver["major"], ver["minor"], ver["micro"]
    )

PyAVLExt = Extension(
    "pyavl",
    sources=glob.glob("src/*.c")
)

with open("README.md", "r") as f:
    long_description = f.read()

setup(
    name="pyavl",
    version=get_version(),
    description="Python C extension library for AVL tree.",
    long_description=long_description,
    author="wormtooth",
    author_email="ye@wormtooth.com",
    maintainer="wormtooth",
    ext_modules=[
        PyAVLExt
    ]
)