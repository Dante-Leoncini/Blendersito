# Blendersito (el nombre se va a cambiar)
Una version INSPIRADA en Blender. pero creada desde cero para Symbian s60v3

(Actualmente no hay soporte para s60v5 o Symbian belle. Pero en teoria no seria dificil de lograr)

En video se ve mas bonito: [Primera Demo en Video](https://youtu.be/dMe-Vit5OT0)

Fue optimizado para funcionar en un Nokia N95-3 con 128mb de ram.
Puede funcionar en otros Nokia s60v3 con 64mb o menos. pero como la mayoria no tiene aceleracion grafica, no se va a ver tan fluido como en las demos que estoy mostrando.

Novedades 20/06/2023:
1) se quito el logo de Blender ya que tiene derechos de autor y le pertenece a la Blender Foundation.
2) se arreglaron mujltiples errores al importar OBJ.
3) Se agrego soporte de Quads y Ngones en el importador de obj
4) se puede hacer renders del viewport
5) se puede hacer animaciones basicas lineales
6) se puede renderizar secuencias de imagenes
7) es posible hacer multiples pases de render, como el zbuffer para poder usar en composicion
8) se avanza en el arreglo del modo edicion, pero aun falta.
9) se experimento con un modo de postprocesado por cpu. falta terminar pero parece dar buenos resultados
10) se agrego un modo de render. pero las luces aun no funcionan correctamente
11) se agrego "fog" para que la apariencia de la UI sea mas parecida

Novedades 8/5/2023:
1) Ya se puede importar archivos obj!
![photo Example](https://github.com/Dante-Leoncini/Blendersito/blob/main/capturas/importObj.png?raw=true)

2) tambien se agrego el soporte de luces, camaras y emptys.
3) Ahora se puede duplicar mallas 3d linkeadas.
4) ahora los materiales no son parte del objeto. los objetos se pueden linkear a un material como se haria en el blender original,
5) se esta trabajando en una UI.
6) se agrego el cursor 3d. actualmente se puede hacer un snap al objeto seleccionado y al crear un objeto se crea donde esta el cursor 3d (planeo agregar mas funcionalidades a futuro)
7) soporte de vertex color (hace falta desactivar la iluminacion y activarlo en el material)
   
![photo Example](https://github.com/Dante-Leoncini/Blendersito/blob/main/capturas/vertexcolor.png?raw=true)

lo malo... se rompio el modo edicion, el agrupado de vertices y bordes en esta version. cuando este completo el importador, voy a retomar el editor de mallas 3d

![photo Example](https://github.com/Dante-Leoncini/Blendersito/blob/main/capturas/nuevaUI.png?raw=true)

Actualmente permite:
1) Crear y Eliminar multiples objetos: Suzanne, Cube, Claude y Vertice

![photo Example](https://github.com/Dante-Leoncini/Blendersito/blob/main/capturas/Screenshot0007.jpg?raw=true)
![photo Example](https://github.com/Dante-Leoncini/Blendersito/blob/main/capturas/Screenshot0034.jpg?raw=true)

2) Edicion basica de mallas 3d: Mover vertices, Extruir Vertices, Eliminar Vertices, Seleccionar vertices, Bordes y Caras (se esta trabajando en mas opciones)

![photo Example](https://github.com/Dante-Leoncini/Blendersito/blob/main/capturas/Screenshot0027.jpg?raw=true)

3) Editar la posicion, rotacion y escala de los objetos

![photo Example](https://github.com/Dante-Leoncini/Blendersito/blob/main/capturas/Screenshot0026.jpg?raw=true)

4) poder editar el material, Su color (diffuse), Emission, Specular, Shader Smooth, Cambiar Textura, Cambiar la interpolacion de textura entre Closest y Lineal, Activar/desactivar transparents

![photo Example](https://github.com/Dante-Leoncini/Blendersito/blob/main/capturas/Screenshot0014.jpg)
![photo Example](https://github.com/Dante-Leoncini/Blendersito/blob/main/capturas/Screenshot0016.jpg?raw=true)
![photo Example](https://github.com/Dante-Leoncini/Blendersito/blob/main/capturas/Screenshot0024.jpg)
![photo Example](https://github.com/Dante-Leoncini/Blendersito/blob/main/capturas/Screenshot0023.jpg)
![photo Example](https://github.com/Dante-Leoncini/Blendersito/blob/main/capturas/Screenshot0025.jpg)

5) Poder entrar en Modo Object y Modo Edicion
6) proyeccion en perspectiva/orthographic, poder mover la camara a una vista superior, de perfil o frente (trabajando en mas funciones)

![photo Example](https://github.com/Dante-Leoncini/Blendersito/blob/main/capturas/mejores%20(1).jpg?raw=true)

7) Poder cambiar la vista 3d entre Material preview, Solid y Wireframe

![photo Example](https://github.com/Dante-Leoncini/Blendersito/blob/main/capturas/Screenshot0034.jpg)
![photo Example](https://github.com/Dante-Leoncini/Blendersito/blob/main/capturas/Screenshot0036.jpg)
![photo Example](https://github.com/Dante-Leoncini/Blendersito/blob/main/capturas/Screenshot0038.jpg)

8) trabajando en una opcion para importar/exportar modelos .OBJ

Tengo pensado agregar mas funciones. Cualquier aporte que quiera realizar al codigo es Bienvenida
