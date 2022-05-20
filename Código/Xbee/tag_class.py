
from digi.xbee.devices import RemoteXBeeDevice, XBee64BitAddress
import time
import sys


class Tag:

    init = "0000"
    confirm = "9999"

    def __init__(self, master, base, mac, tag_num):
        self.xbee = RemoteXBeeDevice(master, XBee64BitAddress.from_hex_string(base+str(mac))) # inicializa el xbee
        self.tag_num = tag_num      # crea el numero tag como atributo
        self.value = ""             # define el valor almacenado
        self.state = 0

    def state_init(self, master):
        count = 0
        while count < 3:
            try:
                if self.state == 0:
                    master.send_data(self.xbee, self.init + self.tag_num)  # manda 000 para establecer conexión
                else:
                    return None
            except:
                count += 1
                print("intento " + str(count))

            time.sleep(0.1)
        return self.tag_num #Crear lista con todos los tags malos
        #raise Exception("Tag: " + str(self.tag_num) + " no ha respondido, verifique su estado antes de continuar")

    def state_confirm(self, master):
        master.send_data(self.xbee, self.confirm + self.tag_num)  # manda 9999 para finalizar comunicación
        self.state = 1

    def send_data(self, master, data):
        self.state = 1  # En caso de que vuelvan a actualizar la info se regresa a estado 1 esperando confirmacion
        count = 0
        while count < 3:
            try:
                if self.state == 1:
                    master.send_data(self.xbee, str(data) + self.tag_num)
                    #master.send_data_async(self.xbee, str(data) + self.tag_num)
                else:
                    return True
            except:
                count += 1
                print("intento: ", count)

            time.sleep(0.1)
        raise Exception("Tag: " + str(self.tag_num) + " no ha respondido, verifique su estado antes de continuar")

    def state_update(self, info, master, conf_bit):
        if self.state == 0:                                 # Si esta en estado de inicialización
            if info == self.init:
                self.state = 1                              # Estado de inicializado

        elif (self.state == 1) and (conf_bit == str(0)):    # Si el tag esta en 1 y el bit de confirmación no tiene valor
            if info != self.init and info != self.confirm:  # Y los valores que recibe son entre 0000 y 9999
                self.state = 2                              # Ubiquese en el estado dos de que ya tiene información alumbrandose

        elif (self.state == 2) and (conf_bit == str(1)):    # Si el tag ya tiene estado 2 y confirmación con bit de confirmación 1
            if info != self.init and info != self.confirm:  # Y los valores que recibe son entre 0000 y 9999
                self.value = info                           # almacenar el valor enviado
                self.state_confirm(master)
                self.post_info()

        elif (self.state == 1) and (conf_bit == str(1)):    # Confirmación con bit de confirmación 1
            print("Tag: " + self.tag_num + " probablemente tenga el botón hundido o el mensaje se repitio")

    def post_info(self):    # Función para devolver al servidor
        print("Tag: ", self.tag_num, " envía el numero: ", self.value)


class TrafficLight:
    green = 0
    red = 1

    def __init__(self, master, base, mac, rack):
        self.xbee = RemoteXBeeDevice(master, XBee64BitAddress.from_hex_string(base+str(mac))) # inicializa el xbee
        self.rack = rack
        self.state = self.red

    def state_update(self, info, master):
        if self.state == self.red and info == self.green:
            master.send_data(self.xbee, "000" + str(self.green) + "000")
            self.state = self.green
        elif self.state == self.green and info == self.red:
            master.send_data(self.xbee, "000" + str(self.red) + "000")
            self.state = self.red

