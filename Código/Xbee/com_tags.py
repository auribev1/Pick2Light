from digi.xbee.devices import XBeeDevice
from tag_class import Tag, TrafficLight
import pandas as pd
import time
from functions import jsongen


def my_data_received_callback1(xbee_message):
    address = str(xbee_message.remote_device.get_64bit_addr())
    if address[8:] not in traffic_lights.mac.values:
        data = xbee_message.data.decode("utf8")
        tag_num = data[4:7]
        info = data[:4]
        conf_bit = data[7]  # Bit de confirmación
        print(tag_num, " ", info)

        rack = str(db.loc[(db.tag_num == tag_num) & (base + db.mac == address)].rack.values[0])

        db.loc[(db.tag_num == tag_num) & (base + db.mac == address)].objects.apply(
            lambda x: x.state_update(info, master1, conf_bit))

        tf_info = 1 if 2 in [i.state for i in db.loc[(db.rack == rack)].objects] else 0
        traffic_lights.loc[(traffic_lights.traffic_light == rack)].objects.apply(
            lambda x: x.state_update(tf_info, master1)) #descomentar luego



def my_data_received_callback2(xbee_message):
    address = str(xbee_message.remote_device.get_64bit_addr())
    if address[8:] not in traffic_lights.mac.values:
        data = xbee_message.data.decode("utf8")
        tag_num = data[4:7]
        info = data[:4]
        conf_bit = data[7]  # Bit de confirmación
        print(tag_num, " ", info)

        rack = str(db.loc[(db.tag_num == tag_num) & (base + db.mac == address)].rack.values[0])

        db.loc[(db.tag_num == tag_num) & (base + db.mac == address)].objects.apply(
            lambda x: x.state_update(info, master2, conf_bit))

        tf_info = 1 if 2 in [i.state for i in db.loc[(db.rack == rack)].objects] else 0
        traffic_lights.loc[(traffic_lights.traffic_light == rack)].objects.apply(
            lambda x: x.state_update(tf_info, master2))


#db = pd.read_csv("tags_info.csv", dtype=object, sep=";")
db = pd.read_csv("tags_lab.csv", dtype=object, sep=";")
traffic_lights = pd.read_csv("traffic_lights.csv", dtype=object, sep=";")

master1 = XBeeDevice("COM13", 115200)
master1.open()
print(master1.get_64bit_addr())
master1.add_data_received_callback(my_data_received_callback1)

master2 = XBeeDevice("COM6", 115200)
master2.open()
print(master2.get_64bit_addr())
master2.add_data_received_callback(my_data_received_callback2)

base = "0013A200"

db["objects"] = [Tag(master1, base, i, j) if int(k) != 4 else Tag(master2, base, i, j) for i, j, k in zip(db.mac, db.tag_num, db.rack)]
traffic_lights["objects"] = [TrafficLight(master1, base, i, j) if int(j) != 4 else TrafficLight(master2, base, i, j)
                             for i, j in zip(traffic_lights.mac, traffic_lights.traffic_light)]

error_log = [db.objects[i].state_init(master1) if int(db.iloc[i].rack) != 4 else db.objects[i].state_init(master2) for i in range(len(db))]
if any(isinstance(i, str) for i in error_log):
    raise Exception("Revisar los siguientes tags: " + "\n".join('%s' % tag for tag in error_log if tag is not None))


while True:
    json = jsongen(1, 137)
    if int(db.loc[db.tag_num == json["num"]].rack) != 4:
        db.loc[db.tag_num == json["num"]].objects.apply(lambda x: x.send_data(master1, json["info"]))
    else:
        db.loc[db.tag_num == json["num"]].objects.apply(lambda x: x.send_data(master2, json["info"]))
    time.sleep(2)


# Pruebas

# for i in range(len(db)):
#     print(db.objects[i].state)

# for i in range(len(traffic_lights)):
#     print(traffic_lights.objects[i].state)

# for i in range(len(db)):
#     print(db.objects[i].value)

# for i in range(len(db)):
#     if int(db.iloc[i].rack) != 4:
#         db.objects[i].send_data(master1, "1234")
#     else:
#         db.objects[i].send_data(master2, "1234")
