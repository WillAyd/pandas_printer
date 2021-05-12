import numpy as np

from setuptools import Extension, setup

np_test_ext = Extension("libprinter",
                        sources=["libprinter.c"],
                        include_dirs=[np.get_include()],
                        define_macros=[("NPY_NO_DEPRECATED_API", "0")],
                        extra_compile_args=["-g", "-UNDEBUG", "-O0"])

setup(version="0.0.1", ext_modules=[np_test_ext])
