<h1 align="center">DataFusionDB</h1>

## Development
Para poder ejecutar el proyecto en modo desarrollo, tendrás que modificar el archivo `compile_flags.txt` y colocar tus rutas relativas a `pybind11` y a tu versión de `Python`. Esto te permitirá que el compilador `gcc` pueda encontrar las librerías necesarias para enlazar correctamente el código de `C++` con `Python`.

Por otro lado, el archivo `setup.py` sirve como un script de configuración que permite aprovechar la configuración en `cpp` para utilizarse en python. A la hora de ejecutarlo se genera un archivo `.so` que es importado por `frontend/interface.py`, para recibir las funciones de `C++` en `Python`.

1. Instalar dependencias necesarias con `pip install -r requirements.txt`. y si estás en linux `chmod +x ./scripts/arch_dependencies.sh && ./scripts/arch_dependencies.sh`.
2. Ejecutar el comando para generar el archivo `.so` con `chmod +x ./scripts/handle_link_connection.sh && ./scripts/handle_link_connection.sh`.
3. Ejecutar el comando para ejecutar el servidor con `python frontend/interface.py`.

