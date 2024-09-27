import sysconfig
from setuptools import setup
from pybind11 import get_include
from pybind11.setup_helpers import Pybind11Extension, build_ext

pyb11_include = get_include()
python_include_dir = sysconfig.get_path("include")

ext_modules = [
    Pybind11Extension(
        "link_connection",
        [
            "backend/link_connection.cpp",
            "backend/SQLParser.cpp",
            "backend/Record.cpp",
            "backend/ExtendibleHashingFile/ExtendibleHashing.cpp",
            "backend/AvlFile/AvlRA.cpp",
            "backend/AvlFile/AvlRB.cpp",
            "backend/SequentialFile/SequentialFileRA.cpp",
            "backend/SequentialFile/SequentialFileRB.cpp",
        ],
        include_dirs=[
            pyb11_include,
            python_include_dir,
            "backend",
            "backend/ExtendibleHashingFile",
            "backend/AvlFile",
            "backend/SequentialFile",
        ],
        language="c++",
        extra_compile_args=["-std=c++17", "-g"],
    ),
]

setup(
    name="link_connection",
    ext_modules=ext_modules,
    cmdclass={'build_ext': build_ext},
)
