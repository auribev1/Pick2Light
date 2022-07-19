# Pick2Light
Pick2Light communication development with python and arduino

Inicialmente paera conectaar los xbees al computador se debe instalar los drivers necesarios. Para la ultima versión de windows 11.
https://ftdichip.com/drivers/

Tras esto se puede comenzar a utilizar XCTU para la conexión entre arduinos. Aqui se puede descargar e instalar:
https://hub.digi.com/support/products/xctu/

Luego se puede comenzar a utilizar el protocolo serial. El serial tanto de UART (Pc) y del Arduino tienen un Buffer mayor a 64 Bytes y puede ser configurable.
La trama de mensajes a enviar tiene actualmente 24 Bytes de información, se debe configurar igualmente una función asincrona que vaya limpiando el buffer para que no se pierda información.

Como premisa, es necesario utilizar la librería de digi-xbee de python, cualquier duda remitirse a https://xbplib.readthedocs.io/en/latest/getting_started_with_xbee_python_library.html
A continuación, se mencionará las particularidades del código

Si se quiere ver videos del funcionamiento, remitirse al siguiente link: https://eafit-my.sharepoint.com/:f:/g/personal/auribev1_eafit_edu_co/Eu-vMzbqAvRNkzlmpepgZJsBzMxKYYDdITqIgpvSy-AqTA?e=4jDgrs

# Código
La comunicación entre tags y maestros se basa en tres archivos que estan en el repositorio. Estos archivos son: com_tags, tag_class y functions. A continuación se van a detallar

## tag_class
Este archivo contiene las clases que se utilizan para representar los elementos involucrados en la comunicación. Basicamente, el proceso consiste en la interacción de dos clases, los tags de los modulos (Tag) y los semaforos (TrafficLight).

### Tag

Esta clase que hereda las caracteristicas de la clase xbee pero tiene unas funciones especificas y atributos. Este se puede observar en la siguiente figura:

<img width="580" alt="image" src="https://user-images.githubusercontent.com/26825857/178814007-73c36c4c-d2af-4869-a2dd-0d8824276694.png">

Cada Tag tiene como atributos los códigos de init (0000) y confirm (9999). Estos basicamente son los comandos que se transmiten a un tag para inicializarlos y confirmar un pedido especificamente. Estos sserviran durante el funcionamiento para simplificar el código.

En la imagen se puede ver el constructor representado por "__init__" que, cuando se crea una clase de este tipo, se crean unas caracteristicas definidas, estas se listan a continuación.

* Como se menciono anteriormente, una de las caracteristicas del Tag es que es un xbee, este xbee se inicializa con RemoteXBeeDevice de la librería mencionada anteriormente. A este se le debe especificar el coordinador,  al que se le debe especificar cual es el coordinador y se le debe pasar que tipo de Xbee ser, para este caso se utiliza un XBee64BitAddress. A este xbee se le debe pasar la MAC con el que se va a crear, en el código, cuando se crea la clase se especifica la MAC dividida en la parte especifica (con el nombre mac) y la parte general para todos los xbee (llamada base).
* tag_num es el parametro de tres caracteres que identifica al tag del laboratorio y va desde 000 hasta 999, este se especifica al crear la clase
* value es el valor que va a almacenar. Cuando el desde el servidor llega un valor y el tag se alumbra, luego de confirmar el valor este se almacena en este atributo
* state es el estado operativo en el que se encuentra. Este se inicializa en 0 que significa que esta inicializado sin confirmar

A continuación se explican los metodos

#### state_init
<img width="428" alt="image" src="https://user-images.githubusercontent.com/26825857/178817088-6ca2282d-95be-4c30-98a7-ab39840851a6.png">

Esta función sirve para inicializar el tag. El hace un conteo de 3 iteraciones para intentar inicializarlo. Para realizarlo, si el estado sigue estando en 0 (inicializado sin confirmar) utiliza al coordinador para enviarle un mensaje al tag, especificamente el mensaje "init" definido anteriormente. Cuando el estado cambia a 1 (estado de inicializado confirmado) la función acaba. En caso de que no responda el tag, luego de 3 iteraciónes, la función retorna el numero de tag con problema para almacenarlo en una lista de reporte. Entre cada una de las 3 iteraciónes hay una espera de 0.1 segundos.

#### state_confirm
<img width="371" alt="image" src="https://user-images.githubusercontent.com/26825857/178818971-3633160a-9f88-4920-b597-1374d36497d3.png">

Esta función se utiliza cuando el tag tiene un valor almacenado y confirma su valor presionando el boton. Esta función utiliza el coordinador para enviar al tag el mensaje "confirm" mencionado anteriormente. Luego el tag vuelve a estar en estado operativo 1 (inicializado confirmado a la espera de mensajes)

#### send_data
<img width="683" alt="image" src="https://user-images.githubusercontent.com/26825857/178823499-8904dc95-622b-4002-b59e-7add778c798f.png">

Esta función sirve para enviar información a los tags (información diferente de la confirmación e inicialización, es decir, numeros de pedidos). Inicialmente el tag cambia a estado operativo 1 (inicializado confirmado a la espera de información). Luego se ejecuta el mismo proceso de state_init, itera tres veces y en cada iteración intenta mandar el valor especificado en la función con ayuda del coordinador, en caso de fallar itera otra vez hasta que se supere el numero de intentos, en caso de que no se logre establecer comunicación, se levanta una exepción con el error del tag que se debe revisar. Si se establece comunicación, se espera que el tag cambie de estado y salga de la función.

#### state_update
Esta función es la más importante ya que maneja la transición entre estados operativos de un tag.
<img width="643" alt="image" src="https://user-images.githubusercontent.com/26825857/179061048-23d5572a-6fa7-4861-995d-f7e1214a7494.png">
Esta función se ejecuta cada vez que le llega un mensaje de un tag al coordinador. A continuación se explica que acciones ejecuta:

* Si el tag se encuentra en estado 0 (inicializado pero sin confirmar) y el mensaje corresponde al de "init" cambia a estado 1 (inicializado confirmado a la espera de información)
* Si el tag se encuentra en estado 1, el bit del mensaje no es de confirmación y el mensaje son numeros (de cuatro digitos) diferentes al de init y confirm (osea entre 0001 y 9998) se actualiza el estado a 2 (tag visualizando información)
* Si el tag se encuentra en estado 2 (visualizando información) y le llega información con un numero y el bit de confirmación en 1 significa que le estan confirmando. El valor que llega, si es diferente a "init" y a "confirm" se almacena como el valor confirmado por el tag incluyendo la fecha y hora de confirmación y se ejecuta la función "confirm" mencionada anteriormente.
* Por ultimo, si el estado es 1 (inicializado confirmado a la espera de información) y el bit de confirmacion es 1 es un estado fallido, probablemente el botón este hundido.

### TrafficLight
<img width="575" alt="image" src="https://user-images.githubusercontent.com/26825857/178858250-c83f4664-c056-4142-b209-567f99628645.png">

Basicamente traffic light es otro tipo de xbee que tiene información similar a la clase Tag. Tiene dos atributos que corresponden a Green o Red que representan el mensaje que se debe envíar para cambiar el semaforo a un respectivo color. la clase constructora define el Xbee asociado, el coordinador respectivo con su respectiva MAC como se explico en la clase Tag y se asocia a un modulo que en este caso se llama "rack". Hay un parametro para almacenar en que estado operativo está (puede estar en rojo o verde). Por último, cuando se construye la clase, se ejecuta la función de inicializar.

#### state_init
<img width="407" alt="image" src="https://user-images.githubusercontent.com/26825857/178858523-e836fcc5-f265-43b1-93aa-0f0b46556152.png">

Esta función envía al semaforo el mensaje para que inicie la operación en color verde (que significa que no hay Tags alumbrando en su respectivo modulo (rack)

#### state_update
<img width="582" alt="image" src="https://user-images.githubusercontent.com/26825857/178858614-9f0c1ae0-b854-4184-9aad-a84c18c6234a.png">

Esta función es similar a la de la clase Tag y se actualiza cada vez que le llega información a diversos tags en la función "callback" que se ejecuta cada vez que llega un mensaje, esta sigue la siguiente secuencia lógica:
* Si cuando se llama la función, el estado del semaforo es rojo (hay tagas alumbrando) y la información corresponde al color verde (no hay tags alumbrando), se envía el color verde al semaforo, se actualiza su estado y se hace un POST al servidor de la información (esto sucede cuando todos los tags de un modulo confirman su valor) con la función "post_info". Esta función se explica en la siguiente sección.
* Si el estado es verde y la información corresponde al color rojo, se envía un mensaje al semaforo para que cambie a color rojo y se actualiza su estado.

## functions
El archivo functions tiene unas funciones auxiliares que permiten complementar la funcionalidad de la aplicación. Esencialmente consiste en dos funciones que sirven para la prueba y que serán remplazadas una vez se haya comunicación directa con la aplicación del servidor

### jsongen
<img width="664" alt="image" src="https://user-images.githubusercontent.com/26825857/179238903-48f8b899-0d20-42be-bcef-841df12e37e2.png">

jsongen es una función que se utiliza para simular la consulta de información en el end point del servidor. Dados unos numeros (minimo y máximo) de tags disponibles y un numero de tags por consulta, se genera un diccionario con N referencias, donde aparece un numero aleatorio de tag y un valor asociado. Esta función se cambiara por la consulta al servidor por información nueva de los modulos para alumbrar

### post_info
<img width="446" alt="image" src="https://user-images.githubusercontent.com/26825857/179239783-4ca4088c-81d0-4178-8948-6002b165d597.png">
post_info es una función que sirve para simular la respuesta de la aplicación al servidor. Esto ocurre cuando todo un modulo confirma su valor y la aplicación de los xbees va a contestar con todos los valores confirmados para cada uno de los tags. Esta funcion consiste en cargar toda la información de los tags de un modulo y solo seleccionar las que tienen un valor asociado. Crea un diccionario con el numero de tag y su valor asociado, actualiza el modulo con valor vacío indicando que ya se leyo la información y que el modulo vuelve a estar disponible. Este valor se printea en la consola, sin embargo, se va actualizará cuando se deba hacer el post con la información al servidor con un protocolo diferente.

## com_tags
com_tags es el main del archivo, el script que va a estar ejecutando todo el funcionamiento y que va a ser el principal. Vamos a comenzar describiendo cada una de las secciones:

### Librerias
<img width="284" alt="image" src="https://user-images.githubusercontent.com/26825857/179241960-aa384c94-9ea2-45b6-be8f-418a545a5fdc.png">
Se importan las librerías del xbee de digi-xbee que dejan crear dospositivos. Se importa la librería pandas para poder cargar los dataframes y las asociadas a los otros archivos descritas anteriormente

### Función callback
<img width="595" alt="image" src="https://user-images.githubusercontent.com/26825857/179242297-ee69d7df-cc4e-4470-b0b5-57d959e841e2.png">
La función callback es de las más importantes ya que es la función que se ejecuta cada vez que a un coordinador le llega un mensaje. Recibe la información de manera asincrona sin necesidad de parar el código. Una vez un coordinador conectado al computador reciba este mensaje, se ejecuta la función siguiendo los siguientes pasos:
*El mensaje se almacena como un string y de este string se extrae la dirección MAC. Si la dirección MAC no corresponde a un semaforo, se evalúa la función (esto se hace porque los semaforos no contestan nada). Si la dirección corresponde a un tag, lo primero que se hace es decodificar el mensaje y extraer cierta información: numero del tag que envía, información que el tag envió y el bit de confirmación que representa si el tag esta hundiendo el boton de confirmación o no.
*De la dirección MAC del tag que envió la información se extrae el rack o modulo al que pertenece el tag esto se hace para poder filtrar los tags que se encuentran tambien encendidos (util para poder enviar la confirmación)
*Se utilizan los datos identificados almacenados para ejecutar la función de state_update mencionada anteriormente. Esto permite que dependiendo del mensaje, cada tag cambie su estado
*Las ultimas lineas identifican si todos los tags de un modulo se encuentran confirmados o no, esto se hace para ejecutar la función state_update del semaforo para que una vez todos los modulos esten listos, este alumbre verde, de caso contrario, si aun hay información visualizandose en algun tag, alumbre verde.

Esta función es la de call back para el primer coordinador. Son dos coordinadores para que puedan manejar todas las peticiones simultaneas sin colapsar el buffer. Se debe entonces definir una función identica que considere el coordinador dos definido como "master2". En el caso anterior, cuando a la función se pasaba información sobre el coordinador encargado de enviar informacion, se utilizaba master1. La imagen de la segunda función call back se muestra a continuación.

<img width="605" alt="image" src="https://user-images.githubusercontent.com/26825857/179312563-0428efbf-f141-47f5-8301-535b321056bb.png">

### Leer datos e inicializar coordinadores
<img width="464" alt="image" src="https://user-images.githubusercontent.com/26825857/179312628-60b332e1-ae65-4fe7-9c4d-70fc18e31b88.png">

Lo primero que se hace es leer el archivo de información de tags del laboratorio, esto tiene información relacionada a cada tag, su referencia, la MAC especifica del xbee asociada a este y el rack o modulo al que pertenece. Esta se puede ver a continuación.

<img width="218" alt="image" src="https://user-images.githubusercontent.com/26825857/179313857-0f2b01df-d708-4b06-a1d2-cc9c16807e63.png">

La segunda información que se carga corresponde a los semaforos. Se cuenta para cada semaforo la MAC del xbee asociado y el modulo que representa. Esta se puede ver a continuación.

<img width="166" alt="image" src="https://user-images.githubusercontent.com/26825857/179314154-b50dcb13-085e-42fe-9d23-a3c7811f0600.png">

Luego se inicializan los dos coordinadores, el puerto que se les especifica depende del computador y puede ser consultado en el XCTU cuando se contecta cada uno. Luego de definir cual coordinador va a ser el 1 (controla los modulos A,B y C) y el 2 (controla el modulo D), se abre la conexión y se asocia la función asincrona de call back a cada una (son dos funciones, una diferente para cada coordinador). Por ultimo se define la base que corresponde al tramo de MAC que es comun para todos los xbees, esto se hace para simplificar el mandejo de información.

### Crear clases e inicializarlas
<img width="845" alt="image" src="https://user-images.githubusercontent.com/26825857/179315007-a4229d0a-e320-4e99-988b-a8282e9325f3.png">

Luego de inicializar los coordinadores lo que se hace es que en las dos tablas o dataframes creados, se añade una columna a cada esrtructura que inicialice los tags o semaforos con las clases creadas mencionadas anteriormente (esto ayuda a comunicarse con los xbee teniendo información adicional sobre los estados operativos de cada objeto). A cada inicialización le corresponde un coordinador que gobierna la comunicación. Luego se inicializan los tags del laboratorio con la función state_init y los tags que no respondan se van almacenando en una lista que se llama error_log. Esta lista se muestra si hay alguna referencia en ella para que no se proceda con el código si hay tags malos.

### Ciclo
<img width="602" alt="image" src="https://user-images.githubusercontent.com/26825857/179316885-957e5a4d-0a07-4457-9812-b94f0599b174.png">

Por último esta la parte del ciclo que se ejecuta constantemente. En esta parte debe llamarse la función que lea el endpoint del servidor para consultar la información a alumbrar y ejecutar el enviar información con la función send_data para enviar la información respectiva a cada tag.

Provisionalmente mientras se define la forma de enviar los datos, se llama la función jsongen que genera unos diccionarios aleatorios de pedidos y para estos pedidos, ejecuta la función send_data para enviar a cada tag su respectivo valor asignado y luego esperar 60 segundos para una próxima iteración
