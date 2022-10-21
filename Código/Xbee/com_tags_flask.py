from digi.xbee.devices import XBeeDevice
from tag_class import Tag, TrafficLight
import pandas as pd
from flask import Flask, request, make_response


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

        post = db.loc[(db.rack == rack)]
        tf_info = 1 if 2 in [i.state for i in db.loc[(db.rack == rack)].objects] else 0
        traffic_lights.loc[(traffic_lights.traffic_light == rack)].objects.apply(
            lambda x: x.state_update(tf_info, master1, post))


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

        post = db.loc[(db.rack == rack)]
        tf_info = 1 if 2 in [i.state for i in db.loc[(db.rack == rack)].objects] else 0
        traffic_lights.loc[(traffic_lights.traffic_light == rack)].objects.apply(
            lambda x: x.state_update(tf_info, master2, post))


db = pd.read_csv("tags_lab.csv", dtype=object, sep=";")
traffic_lights = pd.read_csv("traffic_lights.csv", dtype=object, sep=";")

master1 = XBeeDevice("COM7", 115200)
master1.open()
print(master1.get_64bit_addr())
master1.add_data_received_callback(my_data_received_callback1)

master2 = XBeeDevice("COM5", 115200)
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


#Servidor local
app = Flask(__name__)

@app.route("/module-information/", methods=['POST'])
def post_form():
    if request.method == "POST":
        json = request.get_json()
        for item in json.items():
            if int(db.loc[db.tag_num == item[0]].rack) != 4:
                db.loc[db.tag_num == item[0]].objects.apply(lambda x: x.send_data(master1, item[1]))
            else:
                db.loc[db.tag_num == item[0]].objects.apply(lambda x: x.send_data(master2, item[1]))
    return make_response("sucess", 200)


# while True:
#     json = jsongen(1, 137, 10)
#     print(json)
#     for item in json.items():
#         if int(db.loc[db.tag_num == item[0]].rack) != 4:
#             db.loc[db.tag_num == item[0]].objects.apply(lambda x: x.send_data(master1, item[1]))
#         else:
#             db.loc[db.tag_num == item[0]].objects.apply(lambda x: x.send_data(master2, item[1]))
#     time.sleep(60)


if __name__=="__main__":
    app.run()