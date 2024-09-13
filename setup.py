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
        ["backend/link_connection.cpp"],
        include_dirs=[
            pyb11_include,
            python_include_dir
        ],
    ),
];

# configuracion para la compilacion
# se ejecuta con python setup.py build_ext --inplace
setup(
    name = "link_connection",
    ext_modules = ext_modules,
    cmdclass = {
        'build_ext': build_ext
    },
);
