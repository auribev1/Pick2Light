from digi.xbee.devices import XBeeDevice, RemoteXBeeDevice, XBee64BitAddress
from tag_class import Tag
import pandas as pd
import time
from functions import jsongen


def my_data_received_callback(xbee_message):
    address = xbee_message.remote_device.get_64bit_addr()
    data = xbee_message.data.decode("utf8")
    tag_num = data[4:7]
    info = data[:4]
    conf_bit = data[7]      # Bit de confirmación
    print(tag_num, " ", info)
    db.loc[(db.tag_num == tag_num) &   # Doble validación si el tag corresponde al numero y si la mac es igual al xbeeadress
           (base + db.mac == str(address))].objects.apply(lambda x: x.state_update(info, master, conf_bit))


db = pd.read_csv("tags_info.csv", dtype=object, sep=";")

master = XBeeDevice("COM13", 115200)
master.open()
print(master.get_64bit_addr())

master.add_data_received_callback(my_data_received_callback)

base = "0013A200"

db["objects"] = [Tag(master, base, i, j) for i, j in zip(db.mac, db.tag_num)]

for i in range(len(db)):
    db.objects[i].state_init(master)

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
    db.objects[i].send_data(master, "123"+str(i))

for i in range(len(db)):
    print(db.objects[i].value)