# BD2-proyecto1
Proyecto 1 del curso de Base de datos 2 en UTEC

# Integrantes

* Yared Riveros Rodriguez
* Mariajulia Romani Tafur
* Camila Rodriguez Valverde

---

## Implementación de archivos secuenciales ordenados y hashing extensible

Este proyecto presenta dos implementaciones de almacenamiento y búsqueda de registros: archivos secuenciales ordenados y hashing extensible. El usuario puede seleccionar la implementación que desea utilizar después de cargar el archivo CSV. Las operaciones disponibles para ambas implementaciones incluyen buscar, eliminar y buscar por rango.

### Archivos secuenciales ordenados

La implementación de archivos secuenciales ordenados utiliza dos archivos: un archivo principal y un archivo auxiliar. Los registros se insertan en el archivo principal de manera ordenada, mientras que el archivo auxiliar se utiliza para agregar nuevos registros que no encajan en el orden actual. Cuando el archivo auxiliar alcanza un límite predefinido de registros, los dos archivos se fusionan y se vuelven a ordenar.

En esta implementación, hemos trabajado en las siguientes funciones:

`insertRecord`: inserta un registro en el archivo principal o auxiliar según las condiciones especificadas.
search: busca un registro por su ID, primero en el archivo auxiliar y luego en el archivo principal. Retorna un vector con todos los registros que tienen la ID especificada.
`deleteRecord`: elimina todos los registros con la ID especificada en el archivo principal.
`mergeFiles`: fusiona y reordena el archivo principal y auxiliar cuando se alcanza el límite predefinido de registros en el archivo auxiliar.
searchRange: busca todos los registros cuyas ID estén dentro de un rango especificado.


### Hashing extensible

loading...

## Funciones implementadas:

`search`: función para buscar registros por su ID.
`delete`: función para eliminar registros por su ID.
`searchRange`: función para buscar registros dentro de un rango de ID.

## Uso


Después de cargar el archivo CSV, el programa presentará una interfaz de menú donde el usuario puede seleccionar la implementación que desea utilizar (archivos secuenciales ordenados o hashing extensible). Una vez seleccionada la implementación, el usuario puede realizar las siguientes operaciones: buscar, eliminar y buscar por rango de ID.

## Compilación y ejecución

loading..

Dependencias
C++17 o posterior

Licencia

@LaFe
