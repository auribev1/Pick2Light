from digi.xbee.devices import XBeeDevice, RemoteXBeeDevice, XBee64BitAddress
from tag_class import Tag
import pandas as pd
import threading
import time
from functions import jsongen


def my_data_received_callback(xbee_message):
    address = xbee_message.remote_device.get_64bit_addr()
    data = xbee_message.data.decode("utf8")
    tag_num = data[4:]
    info = data[:4]
    print(tag_num, " ", info)
    db.loc[(db.tag_num == tag_num) & (base + db.mac == str(address))].objects.apply(lambda x: x.state_update(info, master))
    # Doble validación si el tag corresponde al numero y si la mac es igual al xbee adress


class ThreadedCount(threading.Thread):
    def run(self):
        thread = threading.current_thread()
        n = 3  # Contador de intentos
        for j in range(n):
            for tag in db.objects:
                if tag.state == 0:
                    tag.state_init(master)
                time.sleep(5)
        for tag in db.objects:
            if tag.state == 0:
                tag.state = 3
        return


db = pd.read_csv("tags_info.csv", dtype=object, sep=";")

master = XBeeDevice("COM13", 115200)
master.open()
print(master.get_64bit_addr())

master.add_data_received_callback(my_data_received_callback)

base = "0013A200"

db["objects"] = [Tag(master, base, i, j) for i, j in zip(db.mac, db.tag_num)]

init_try = ThreadedCount()
init_try.start()

while True:
    json = jsongen(1, 10)
    db.loc[db.tag_num == json["num"]].objects.apply(lambda x: x.send_data(master, json["info"]))
    time.sleep(5)

# Pruebas
db.objects[0].state
db.objects[0].send_data(master, "1234")
db.objects[0].value

for i in range(len(db)):
    print(db.objects[i].state)

for i in range(len(db)):
    db.objects[i].send_data(master, "1434")

for i in range(len(db)):
    print(db.objects[i].value)