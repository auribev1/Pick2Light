from digi.xbee.devices import XBeeDevice, RemoteXBeeDevice, XBee64BitAddress


state = 0
xbee = XBeeDevice("COM13", 115200)
xbee.open()
print(xbee.get_64bit_addr())

base = "0013A200"
xb1 = "417FC58B"
xb2 = "4172C75C"
xb3 = "418CBA6C"
xb4 = "4107898C"
xb5 = "41894079"
xb6 = "40ABDDAA"
xb7 = "4172C75A"
xb8 = "4107898E"
xb9 = "40B5EA4C"
xb10 = "40BA975B"

remote1 = RemoteXBeeDevice(xbee, XBee64BitAddress.from_hex_string(base+xb1))
remote2 = RemoteXBeeDevice(xbee, XBee64BitAddress.from_hex_string(base+xb2))
remote3 = RemoteXBeeDevice(xbee, XBee64BitAddress.from_hex_string(base+xb3))
remote4 = RemoteXBeeDevice(xbee, XBee64BitAddress.from_hex_string(base+xb4))
remote5 = RemoteXBeeDevice(xbee, XBee64BitAddress.from_hex_string(base+xb5))
remote6 = RemoteXBeeDevice(xbee, XBee64BitAddress.from_hex_string(base+xb6))
remote7 = RemoteXBeeDevice(xbee, XBee64BitAddress.from_hex_string(base+xb7))
remote8 = RemoteXBeeDevice(xbee, XBee64BitAddress.from_hex_string(base+xb8))
remote9 = RemoteXBeeDevice(xbee, XBee64BitAddress.from_hex_string(base+xb9))
remote10 = RemoteXBeeDevice(xbee, XBee64BitAddress.from_hex_string(base+xb10))

#Inicializar
init = 0
xbee.send_data_async(remote1, "0000"+"001")

#Enviar dato
xbee.send_data_async(remote1, "1234"+"001")

#Confirma recepción de nuevo dato
xbee.send_data_async(remote1, "9999"+"001")


def my_data_received_callback(xbee_message):
    address = xbee_message.remote_device.get_64bit_addr()
    data = xbee_message.data.decode("utf8")
    print("Received data from %s: %s" % (address, data))
    #if str(address) == base+xb1:
        #print("si")
        #if data == "0000001":
            #xbee.send_data_async(remote1, "1234" + "001")



xbee.add_data_received_callback(my_data_received_callback)
