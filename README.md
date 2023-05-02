# BD2-proyecto1
Proyecto 1 del curso de Base de datos 2 en UTEC

# Integrantes

* Yared Riveros Rodriguez
* Mariajulia Romani Tafur
* Camila Rodriguez Valverde

---

## Introducción
Nuestr objetivo es crear desde cero 2 técnicas de gestión de bases de datos: Sequential File y Extendible Hashing. Luego, realizar una comparación de ambas técnicas en función de accesos a memoria y tiempo de ejecución de sus algoritmos de búsqueda e inserción. Se espera que la técnica Extendible Hashing tenga más accesos a memoria que el Sequential File, pero que sus operación de búsqueda e inserción tomen tomen menos tiempo en ejecutarse.

Para las pruebas, crearemos nuestro propio dataset, que contiene los siguientes atributos: id, nombre apellidos, ciclo, carrera y codigo de los alumnos de UTEC.

## Técnicas utilizadas

Este proyecto presenta dos implementaciones de almacenamiento y búsqueda de registros: archivos secuenciales ordenados y hashing extensible. El usuario puede seleccionar la implementación que desea utilizar después de cargar el archivo CSV. Las operaciones disponibles para ambas implementaciones incluyen buscar, eliminar y buscar por rango.

### Archivos secuenciales ordenados

La implementación de archivos secuenciales ordenados utiliza dos archivos: un archivo principal y un archivo auxiliar. Los registros se insertan en el archivo principal de manera ordenada, mientras que el archivo auxiliar se utiliza para agregar nuevos registros que no encajan en el orden actual. Cuando el archivo auxiliar alcanza un límite predefinido de registros, los dos archivos se fusionan y se vuelven a ordenar.

#### Funciones implementadas:

`insertRecord`: inserta un registro en el archivo principal o auxiliar según las condiciones especificadas.
search: busca un registro por su ID, primero en el archivo auxiliar y luego en el archivo principal. Retorna un vector con todos los registros que tienen la ID especificada.

`deleteRecord`: elimina todos los registros con la ID especificada en el archivo principal.

`mergeFiles`: fusiona y reordena el archivo principal y auxiliar cuando se alcanza el límite predefinido de registros en el archivo auxiliar.

`searchRange`: busca todos los registros cuyas ID estén dentro de un rango especificado.

#### Complejidad (accesos a memoria secundaria)

`Insert`: 1

`Search`: #records

`Delete`: 1+ 2*#records


### Hashing extensible

Nuestra implementación utiliza 2 archivos: records.bin (almacena los buckets que a su vez almacenan records) e index.bin (almacena los índices que me permiten saber la posición de los buckets en records.bin).

#### Funciones implementadas:

La complejidad de todas estas operaciones es O(k), donde k es la cantidad máxima de encadenamientos.

`Insert`: función para insertar registros. Primero, busca el índice asociado al id del record. Luego, busca la cadena de buckets asociado a ese índice. Por último, existen 3 casos posibles para la inserción:

- Si el 1er bucket de la cadena no está lleno, inserta el record.
- Si el bucket está lleno y la profundidad local del bucket es menor a la profundidad global, entonces se crea un bucket a partir del bucket original y se dividen los records entre éstos 2.
- Si el bucket está lleno y la profundidad local del bucket es mayor o igual a la profundidad global, entonces se crea un nuevo bucket y se encadena al bucket que se llenó

`Search`: función para buscar registros por su ID. Primero, busca el índice asociado al id del record. Luego, busca la cadena de buckets asociado a ese índice. Por último, itera en la cadena de buckets para buscar el record.

`Delete`: función para eliminar registros por su ID. Primero, realiza el mismo proceso descrito en la búsqueda. Luego, una vez encontrado el bucket que contiene el record, lo elimina del bucket.

*Nota: EL range search no es soportado por esta técnica*

#### Complejidad (accesos a memoria secundaria)

`Insert`: 4 + #encadenamientos + 2*#indices

`Search`: 2 + #encadenamientos 

`Delete`: 3 + #encadenamientos


## Resultados experimentales

  loading...

## Uso

Después de cargar el archivo CSV, el programa presentará un menú  en consola donde el usuario puede seleccionar la implementación que desea utilizar (archivos secuenciales ordenados o hashing extensible). Una vez seleccionada la implementación, el usuario puede realizar las siguientes operaciones: buscar, eliminar y buscar por rango de ID.

## Compilación y ejecución

  loading...

## Dependencias

C++17 o posterior

## Licencia

@LaFe
