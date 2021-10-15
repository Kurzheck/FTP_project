import socket

size = 8192

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind(('', 9876))
sequence = 0

try:
  while True:
    data, address = sock.recvfrom(size)
    sequence += 1
    msg_str = data.upper().decode()
    sock.sendto((str(sequence) + ' ' + msg_str).encode(), address)
finally:
  sock.close()