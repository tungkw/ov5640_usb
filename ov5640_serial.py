import time
import serial
import cv2
import numpy as np
import threading

class OV5640:
    def __init__(self, com='com7'):
        self.port = serial.Serial(com, baudrate=1000000, timeout=100)
        self.start_mark = int(0xffd8).to_bytes(2,'big')
        self.end_mark = int(0xffd9).to_bytes(2,'big')
        self.img_lock = threading.Lock()
        self.stop_flag = False
        self.read_thread = None 
        self.img = None

    def start(self):
        self.read_thread = threading.Thread(target=self.stream)
        self.stop_flag = False
        self.read_thread.start()

    def stream(self):
        jpg_buff = b''
        while self.stop_flag is not True:    
            data = self.port.read(10000)
            jpg_buff += data
            start_idx = jpg_buff.find(self.start_mark)
            end_idx = jpg_buff.find(self.end_mark)
            if start_idx != -1 and end_idx != -1:
                img_encode = jpg_buff[start_idx:end_idx+2]
                jpg_buff = jpg_buff[end_idx+2:]
                try:
                    self.img = cv2.imdecode(np.frombuffer(img_encode, np.uint8), cv2.IMREAD_COLOR)
                except Exception as e:
                    self.img = None
    
    def read(self):
        return self.img

    def stop(self):
        self.stop_flag = True
        self.read_thread.join()
        self.read_thread = None

    def close(self):
        if self.read_thread is not None:
            self.stop()
        self.port.close()

if __name__ == "__main__":
    cam = OV5640('com4')
    cam.start()
    while True:
        img = cam.read()
        if img is None:
            continue
        cv2.imshow("img", img)
        key = cv2.waitKey(1)
        if key != -1:
            break

    cam.close()
