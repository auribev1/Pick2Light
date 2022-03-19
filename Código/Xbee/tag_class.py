
from digi.xbee.devices import XBeeDevice, RemoteXBeeDevice, XBee64BitAddress


class Tag:

    init = "0000"
    confirm = "9999"

    def __init__(self, master, base, mac, tag_num):
        self.xbee = RemoteXBeeDevice(master, XBee64BitAddress.from_hex_string(base+str(mac)))
        self.tag_num = tag_num
        self.value = ""
        self.state = self.state_init(master)

    def state_init(self, master):
        master.send_data_async(self.xbee, self.init + self.tag_num)
        return 0  # corresponde a elemento enviado

    def state_confirm(self, master):
        master.send_data_async(self.xbee, self.confirm + self.tag_num)
        self.state = 1

    def send_data(self, master, data):
        while self.state == 0:                              # Si no se alcanzo a inicializar entonces se lopea hasta que se inicialice
            self.state_init(master)                         # No se que hacer aqui, si lo cambio o lo dejo
            print("sending")

        if self.state == 1 or self.state == 2:
            self.state = 1                                  # En caso de que vuelvan a actualizar la info se regresa a estado 1 esperando confirmacion
            master.send_data_async(self.xbee, str(data) + self.tag_num)

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
