import socket
import sys

print("Number of arguments:", len(sys.argv), "arguments.")
print("Argument List:", str(sys.argv))
print(str(sys.argv[1]))

# Create a TCP/IP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# Connect the socket to the port where the server is listening
server_address = ("127.0.0.1", 8082)

sock.connect(server_address)

try:
    # Send data
    message = "This is the message.  It will be repeated." + (str(sys.argv[1])) + "!\n"
    sock.sendall(bytes(message, "utf-8"))

    sock.sendall(b"Hello, world 2!\n")

    data = sock.recv(10000)
    print(sys.stdout, 'received message from Server "%s\n"' % data.decode("ASCII"))

finally:
    sock.close()
