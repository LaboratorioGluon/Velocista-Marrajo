

import socket

UDP_IP = "192.168.1.133"
UDP_PORT = 5005

MESSAGE = b"Hello, World!"

print("UDP target IP: %s" % UDP_IP)
print("UDP target port: %s" % UDP_PORT)
print("message: %s" % MESSAGE)

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

sock.bind((UDP_IP, UDP_PORT))
sock.settimeout(1)
with open("log.txt", "w") as fp:
    while True:
        try:
            data, addr = sock.recvfrom(1024) # buffer size is 1024 bytes
            datos = data.decode()
            fp.write(datos)
            fp.flush()
            print(datos,end="")
        except socket.timeout as e:
            print(e)
            
sock.close()