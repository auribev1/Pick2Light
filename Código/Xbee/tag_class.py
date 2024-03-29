
from digi.xbee.devices import RemoteXBeeDevice, XBee64BitAddress
from functions import post_info
import time
from datetime import datetime

class Tag:

    init = "0000"
    confirm = "9999"

    def __init__(self, master, base, mac, tag_num):
        self.xbee = RemoteXBeeDevice(master, XBee64BitAddress.from_hex_string(base+str(mac))) # inicializa el xbee
        self.tag_num = tag_num      # crea el numero tag como atributo
        self.value = None
        self.state = 0

    def state_init(self, master):
        count = 0
        while count < 3:
            try:
                if self.state == 0:
                    master.send_data(self.xbee, self.init + self.tag_num)  # manda 000 para establecer conexión
                    count += 1
                else:
                    return None
            except:
                count += 1

            time.sleep(0.1)
        return self.tag_num #Crear lista con todos los tags malos

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
                    count += 1
                else:
                    return True
            except:
                count += 1

            time.sleep(0.1)

        print("\n" + "Tag: " + str(self.tag_num) + " no ha respondido, verifique su estado antes de continuar" + "\n")
        time.sleep(60 * 5)
        raise Exception("Shut down")
        #raise Exception("Tag: " + str(self.tag_num) + " no ha respondido, verifique su estado antes de continuar")

    def state_update(self, info, master, conf_bit):
        if self.state == 0:                                 # Si esta en estado de inicialización
            if info == self.init:
                self.state = 1                              # Estado de inicializado

        elif (self.state == 1) and (conf_bit == str(0)):    # Si el tag esta en 1 y el bit de confirmación no tiene valor
            if info != self.init and info != self.confirm:  # Y los valores que recibe son entre 0000 y 9999
                self.state = 2                              # Ubiquese en el estado dos de que ya tiene información alumbrandose

        elif (self.state == 2) and (conf_bit == str(1)):    # Si el tag ya tiene estado 2 y confirmación con bit de confirmación 1
            if info != self.init and info != self.confirm:  # Y los valores que recibe son entre 0000 y 9999
                now = datetime.now()
                date_time = now.strftime("%m/%d/%Y, %H:%M:%S")
                self.value = info + ", " + date_time        # almacenar el valor enviado
                self.state_confirm(master)

        elif (self.state == 1) and (conf_bit == str(1)):    # Confirmación con bit de confirmación 1
            print("Tag: " + self.tag_num + " probablemente tenga el botón hundido o el mensaje se repitio")


class TrafficLight:
    green = 0
    red = 1

    def __init__(self, master, base, mac, rack):
        self.xbee = RemoteXBeeDevice(master, XBee64BitAddress.from_hex_string(base+str(mac))) # inicializa el xbee
        self.rack = rack
        self.state = self.green
        self.state_init(master)

    def state_init(self, master):
        master.send_data(self.xbee, "000" + str(self.green) + "000")

    def state_update(self, info, master, post):
        if self.state == self.red and info == self.green:
            master.send_data(self.xbee, "000" + str(self.green) + "000")
            self.state = self.green
            post_info(post)     # Cuando cambia a verde hace un post con la información confirmada

        elif self.state == self.green and info == self.red:
            master.send_data(self.xbee, "000" + str(self.red) + "000")
            self.state = self.red

