from digi.xbee.devices import XBeeDevice, RemoteXBeeDevice, XBee64BitAddress

xbee = XBeeDevice("COM9", 115200)
xbee.open()
print(xbee.get_64bit_addr())
remote = RemoteXBeeDevice(xbee, XBee64BitAddress.from_hex_string("0013A20040BA9759"))
xbee.send_data_async(remote, "Hello XBee!")


def my_data_received_callback(xbee_message):
    address = xbee_message.remote_device.get_64bit_addr()
    data = xbee_message.data.decode("utf8")
    print("Received data from %s: %s" % (address, data))


xbee.add_data_received_callback(my_data_received_callback)

while True:
    a = 0
