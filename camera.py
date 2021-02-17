import serial

head_c = b'\xff\xd8'
tail_c = b'\xff\xd9'

if __name__ == '__main__':
    com = serial.Serial('com4', 3000000, timeout=5)
    buffer = []
    img = None
    head = -1
    tail = -1
    while True:
        try:
            data = com.read(2048)
            print(len(data))
            # buffer += data
            # # print(data)
            # if head == -1:
            #     head = buffer.find(head_c)
            #     if head != -1:
            #         print("find head")
            #         buffer = buffer[head:]
            #         head = 0
            # else:
            #     tail = buffer.find(tail_c)
            #     if tail != -1:
            #         print("find tail")
            #         img = buffer[:tail+2]
            #         buffer = buffer[tail+2:]
            #         head = -1
        except Exception as e:
            print("bug")