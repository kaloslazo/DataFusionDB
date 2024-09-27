import sysconfig
from setuptools import setup;
from pybind11 import get_include;
from pybind11.setup_helpers import Pybind11Extension, build_ext;

# configuracion para la compilacion de rutas
pyb11_include = get_include();
python_include_dir = sysconfig.get_path("include");

# configuracion para los modulos, indicamos donde esta insertado la conexion
ext_modules = [
    Pybind11Extension(
        "link_connection",
        [
            "backend/link_connection.cpp",
            "backend/SQLParser.cpp",
            "backend/Record.cpp",
            "backend/AvlFile/AvlRA.cpp",
            "backend/AvlFile/AvlRB.cpp",
            "backend/SequentialFile/SequentialFileRA.cpp",
            "backend/SequentialFile/SequentialFileRB.cpp",
        ],
        include_dirs=[
            pyb11_include,
            python_include_dir,
            "backend",
            "backend/AvlFile",
            "backend/SequentialFile",
        ],
        language = "c++",
        extra_compile_args = ["-std=c++17", "-g"],
    ),
]

# se ejecuta con python setup.py build_ext --inplace
setup(
    name = "link_connection",
    ext_modules = ext_modules,
    cmdclass = {
        'build_ext': build_ext
    },
);
