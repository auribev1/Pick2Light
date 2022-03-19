from digi.xbee.devices import XBeeDevice, RemoteXBeeDevice, XBee64BitAddress
from tag_class import Tag
import pandas as pd


def my_data_received_callback(xbee_message):
    address = xbee_message.remote_device.get_64bit_addr()
    data = xbee_message.data.decode("utf8")
    tag_num = data[4:]
    info = data[:4]
    print(tag_num, " ", info)
    db.loc[(db.tag_num == tag_num) & (base + db.mac == str(address))].objects.apply(lambda x: x.state_update(info, master))
    # Doble validación si el tag corresponde al numero y si la mac es igual al xbee adress


db = pd.read_csv("tags_info.csv", dtype=object, sep=";")

master = XBeeDevice("COM13", 115200)
master.open()
print(master.get_64bit_addr())

master.add_data_received_callback(my_data_received_callback)

base = "0013A200"

db["objects"] = [Tag(master, base, i, j) for i, j in zip(db.mac, db.tag_num)]



# Pruebas
db.objects[0].state
db.objects[0].send_data(master, "1234")
db.objects[0].value

for i in range(len(db)):
    print(db.objects[i].state)

