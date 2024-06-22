# Blendersito (nombre provisional)

Blendersito es un clon de Blender, desarrollado desde cero para Symbian s60v3.
aclaraciones:
1) No es un port: Este programa no comparte código con el original.
2) Funcionalidad básica: El programa es básico, y algunos medios han exagerado sus capacidades actuales.
3) Futuras actualizaciones: Está en desarrollo la capacidad para modelar, renderizar, animar, entre otras funciones. Las novedades se compartirán en redes sociales.
4) Obsolescencia programada: El objetivo del proyecto es evidenciar la obsolescencia programada de los teléfonos, tanto de hace 18 años como de los actuales. Busco generar una demanda por parte de los consumidores de teléfonos más sostenibles y duraderos, reduciendo la enorme cantidad de residuos electrónicos. tambien ayudaria a que mas gente accesda a la tecnologia gracias al reciclaje de estos antiguos dispositivos.
5) Conocimiento en informática: Queremos resaltar la falta de conocimiento general en informática. No debería sorprender que Blendersito funcione en un Nokia N95, un teléfono que en su tiempo era de gama alta, comparable con un iPhone 15 Pro Max de hoy. Quiero dejar claro que con 300 MHz y 128 MB de RAM se pueden realizar tareas que hoy en día requieren teléfonos con 8 GB de RAM y procesadores multicore.
6) Uso del logo de Blender: El logo de Blender pertenece a la Fundación Blender. Me gustaría dialogar con la fundación para dividir el proyecto en dos: uno completamente libre como motor de juegos 3D con su propia identidad, y otro que pueda ser parte de la Fundación Blender, usando su logo y assets para evitar múltiples clones del proyecto (como ocurre con distribuciones de linux). tambien seria interesante ahora si que Blendersito empiece a ser compatible con el blender de toda la vida.
7) Renderización y trazado de rayos: Blendersito ya puede hacer renderizaciones. La renderización, en términos informáticos, es el proceso de generar una imagen a partir de un modelo 2D o 3D. Si bien aún no he implementado el trazado de rayos, el Nokia N95 es capaz de hacerlo mediante su CPU y FPU (Unidad de Punto Flotante). En películas como Terminator 2 y Jurassic Park, se utilizaron computadoras SGI Onyx de 1993. El Nokia N95, lanzado en 2007, tiene 14 años de diferencia y es comparable en cuanto a potencia. Es preocupante que tanta gente se sorprenda de que un teléfono antiguo pueda realizar un render.
8) Desempeño gráfico: Blendersito se ve fluido gracias a la aceleración gráfica por hardware del N95 (PowerVR MBX Lite) y su FPU, lo que permite mover modelos 3D complejos como Arthur Morgan de Red Dead Redemption 2.

![](https://pbs.twimg.com/media/GPawZAKWsAA7Rw9?format=png) ![](https://pbs.twimg.com/media/GPawalTWUAAe-J1?format=png)

9) Este proyecto funciona en todos los Nokia s60v3. Sin embargo, muchos teléfonos, incluso modelos lanzados posteriormente, no tienen aceleración gráfica debido a la mala decisión de Nokia de ahorrar dinero eliminando esta característica. Aunque estos teléfonos puedan parecer más rápidos en papel que el N95, en la práctica son más lentos y económicos debido a la falta de aceleración gráfica.

Gracias a todos los que compartieron el proyecto. Espero que este esfuerzo sirva para aumentar la conciencia y exigir a las empresas que fabriquen teléfonos más duraderos y sostenibles.

(Actualmente no hay soporte para s60v5 o Symbian belle. Pero en teoria no seria dificil de lograr)

En video se ve mas bonito: [Primera Demo en Video](https://youtu.be/dMe-Vit5OT0)

Fue optimizado para funcionar en un Nokia N95-3 con 128mb de ram.
Puede funcionar en otros Nokia s60v3 con 64mb o menos. pero como la mayoria no tiene aceleracion grafica, no se va a ver tan fluido como en las demos que estoy mostrando.

Novedades 20/06/2023:
1) se quito el logo de Blender ya que tiene derechos de autor y le pertenece a la Blender Foundation.
2) se arreglaron multiples errores al importar OBJ.
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
