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
<img width="658" alt="image" src="https://user-images.githubusercontent.com/26825857/178826462-38e19442-4348-4ff8-920e-31ec7499260e.png">
Esta función se ejecuta cada vez que le llega un mensaje de un tag al coordinador. A continuación se explica que acciones ejecuta:

* Si el tag se encuentra en estado 0 (inicializado pero sin confirmar) y el mensaje corresponde al de "init" cambia a estado 1 (inicializado confirmado a la espera de información)
* Si el tag se encuentra en estado 1, el bit del mensaje no es de confirmación y el mensaje son numeros (de cuatro digitos) diferentes al de init y confirm (osea entre 0001 y 9998) se actualiza el estado a 2 (tag visualizando información)
* Si el tag se encuentra en estado 2 (visualizando información) y le llega información con un numero y el bit de confirmación en 1 significa que le estan confirmando. El valor que llega, si es diferente a "init" y a "confirm" se almacena como el valor confirmado por el tag y se ejecuta la función "confirm" mencionada anteriormente.
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

### jsongen
### post_info


## com_tags

### librerias
### función callback
### Leer datos e inicializar coordinadores
### crear clases e inicializarlas
### ciclo
