# Pick2Light
Pick2Light communication development with python and arduino

Inicialmente paera conectaar los xbees al computador se debe instalar los drivers necesarios. Para la ultima versión de windows 11.
https://ftdichip.com/drivers/

Tras esto se puede comenzar a utilizar XCTU para la conexión entre arduinos. Aqui se puede descargar e instalar:
https://hub.digi.com/support/products/xctu/

Luego se puede comenzar a utilizar el protocolo serial. El serial tanto de UART (Pc) y del Arduino tienen un Buffer mayor a 64 Bytes y puede ser configurable.
La trama de mensajes a enviar tiene actualmente 24 Bytes de información, se debe configurar igualmente una función asincrona que vaya limpiando el buffer para que no se pierda información.
