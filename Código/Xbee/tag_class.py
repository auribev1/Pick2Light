
from digi.xbee.devices import XBeeDevice, RemoteXBeeDevice, XBee64BitAddress


class Tag:

    init = "0000"
    confirm = "9999"

    def __init__(self, master, base, mac, tag_num):
        self.xbee = RemoteXBeeDevice(master, XBee64BitAddress.from_hex_string(base+str(mac))) # inicializa el xbee
        self.tag_num = tag_num      # crea el numero tag como atributo
        self.value = ""             # define el valor almacenado
        self.state = self.state_init(master) # Inicializa el xbee para configurarlo por primera vez

    def state_init(self, master):
        master.send_data_async(self.xbee, self.init + self.tag_num) # manda 000 para establecer conexión
        return 0  # corresponde a elemento enviado

    def state_confirm(self, master):
        master.send_data_async(self.xbee, self.confirm + self.tag_num)  #manda 9999 para finalizar comunicación
        self.state = 1

    def send_data(self, master, data):
        if self.state == 1 or self.state == 2:
            self.state = 1    # En caso de que vuelvan a actualizar la info se regresa a estado 1 esperando confirmacion
            master.send_data_async(self.xbee, str(data) + self.tag_num)

        elif self.state == 3:   # Si TAG no responde despues de un tiempo pasa a estar inactivo
            print("tag innactivo - Revisar")

    def state_update(self, info, master):
        if self.state == 0:                                 # Si esta en estado de inicialización
            if info == self.init:
                self.state = 1                              # Estado de inicializado

        elif self.state == 1:                               # Si ya se encuentra inicializado
            if info != self.init and info != self.confirm:  # Y los valores que recibe son entre 0000 y 9999
                self.state = 2                              # Ubiquese en el estado dos de que ya tiene información alumbrandose

        elif self.state == 2:                               # Si ya se tiene información alumbrandose
            if info != self.init and info != self.confirm:  # Y los valores que recibe son entre 0000 y 9999
                self.value = info                           # almacenar el valor enviado
                self.state_confirm(master)
