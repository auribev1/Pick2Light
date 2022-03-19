# Pick2Light
Pick2Light communication development with python and arduino

Inicialmente paera conectaar los xbees al computador se debe instalar los drivers necesarios. Para la ultima versión de windows 11.
https://ftdichip.com/drivers/

Tras esto se puede comenzar a utilizar XCTU para la conexión entre arduinos. Aqui se puede descargar e instalar:
https://hub.digi.com/support/products/xctu/

Luego se puede comenzar a utilizar el protocolo serial. El serial tanto de UART (Pc) y del Arduino tienen un Buffer mayor a 64 Bytes y puede ser configurable.
La trama de mensajes a enviar tiene actualmente 24 Bytes de información, se debe configurar igualmente una función asincrona que vaya limpiando el buffer para que no se pierda información.

# Liberias
Se utiliza la libreria digi-xbee que permite configurar dispositivos xbee y hacer procedimientos de envío y recepción tanto sincronos como asincronos.

![image](https://user-images.githubusercontent.com/26825857/159137987-960b7cd8-227f-4801-98d5-3c7496b419e4.png)

# Pruebas
![image](https://user-images.githubusercontent.com/26825857/159138019-b44932cb-afd8-4037-a275-cf3338cf9c77.png)

En el código de prueba se puede ver como se define la MAC a la que se va a envír con comabdo de xbee remoto. Con la función send_data se envía la información a la instancia de xbee receptor configurada.

# Código actual
Se creo una clase tag_class que hereda las caracteristicas de la clase xbee pero tiene unas funciones especificas y atributos. En este se tiene en cuenta el valor almacenado (para enviar al servidor), numero de tag, estado en el que se encuentra.

Para el caso de las funciones se tiene:
  State_init: que cuando se crea el xbee se actualiza el estado por medio de su ejecución que envpiia 0000 a el receptor para establecer conexión
  state_confirm: cuando el estado del xbee es 2 y llega un numero confirmatorio entonces esta se ejectuta para enviar 9999 al xbee, y retornar al estado 1
  send_data: mientras el xbee no haya sido inicializado correctamente lo hace, si es estado 1 o 2 vuelve a estado 1 y envía la información para actualizar en el display con el nuevo valor
  dtate_update: dependiendo de la información actualiza el estado. Si el estado es cero y la confirmación del xbee corresponde a la iniciación entonces cambia a estado 1 (que corresponde a inicializado). Si el estado es 1 y el mensaje esta entre 0000 y 9999 entonces significa que el xbee recibio el mensaje que se le envío con la funcion de enviar datos y cambia a estado 2(estado de información alumbrandose), Cuando el estado es 2 y el mensaje esta entre estos mismos intervalos entonces se almacena el valor en la clase y se procede a ejecutar state-confirm para enviar 9999 y resetear el estado a 1.
