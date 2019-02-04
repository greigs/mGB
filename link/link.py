#!/usr/bin/python
# coding=utf-8

import socket
import time
import random
import threading
import sys
import os
from enum import Enum
# 

# BGBLinkCable class by TheZZAZZGlitch
class BGBLinkCable():
	def __init__(self,ip,port):
		self.ip = ip
		self.port = port
		self.ticks = 0
		self.frames = 0
		self.received = 0
		self.sent = 0
		self.transfer = -1
		self.lock = threading.Lock()
		self.exchangeHandler = None
		
	def start(self):
		self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		self.sock.connect((self.ip, self.port))
		threading.Thread(target=self.networkLoop).start()
		
	def queryStatus(self):
		status = [0x6a,0,0,0,0,0,0,0]
		self.ticks += 1
		self.frames += 8
		status[2] = self.ticks % 256
		status[3] = (self.ticks // 256) % 256
		status[5] = self.frames % 256
		status[6] = (self.frames // 256) % 256
		status[7] = (self.frames // 256 // 256) % 256
		return bytes(status)
		
	def getStatus(self):
		return (self.frames, self.ticks, self.received, self.sent)
	
	def networkLoop(self):
		while True:
			try:
				data = bytearray(self.sock.recv(8))
				print data
			except KeyboardInterrupt:
				raise
			if len(data) == 0:
				break
			if data[0] == 0x01:
				self.sock.send(data)
				self.sock.send(b'\x6c\x03\x00\x00\x00\x00\x00\x00')
				continue
			if data[0] == 0x6C:
				self.sock.send(b'\x6c\x01\x00\x00\x00\x00\x00\x00')
				self.sock.send(self.queryStatus())
				continue
			if data[0] == 0x65:
				continue
			if data[0] == 0x6A:
				self.sock.send(self.queryStatus())
				continue
			if (data[0] == 0x69 or data[0] == 0x68):
				self.received+=1
				self.sent+=1
				print "asd"
				if self.received == 1:
					data[1] = 0x91
				if self.received == 2:
					data[1] = 0x40
				if self.received == 3:
					data[1] = 0x60

				# self.exchangeHandler(data[1], self)


				data[0] = 0x68
				
				data[1] = 0x91
				self.sock.send(data)
				time.sleep(5)
				data[1] = 0x40
				self.sock.send(data)
				time.sleep(5)
				data[1] = 0x60
				self.sock.send(data)
				time.sleep(5)


				
				#self.sock.send(self.queryStatus())
				#self.sock.send(b'\x68\x91\x81\x00\x00\x00\x00\x00')
				#self.sock.send(b'\x68\x40\x81\x00\x00\x00\x00\x00')
				#self.sock.send(b'\x68\x60\x81\x00\x00\x00\x00\x00')
				continue
			print("Unknown command " + hex(data[0]))
			print(data)
			
	def setExchangeHandler(self, ex):
		self.exchangeHandler = ex
pass




def hexDump(x):
		for i in range(0, len(x), 16):
			print('   ' + ''.join('%02x ' % j for j in x[i : min(len(x), i + 16)]))

byte_counter = 0
packet_data = {"id": 0, "size": 0, "data": []}

def myHandler(b, obj):
	print 'myhandler'
	global byte_counter, packet_data
	byte_counter += 1
	if (byte_counter == 1 and b != 0x88):
		byte_counter -= 1
		return 0x81
	if (byte_counter < 7):
		if (byte_counter == 3):
			packet_data["id"] = b;
		if (byte_counter == 5):
			packet_data["size"] = b;
		if (byte_counter == 6):
			packet_data["size"] += b*256;
			print("Incoming packet: ID=%s, size=%i" % (hex(packet_data["id"]), packet_data["size"]))
	else:
		if (packet_data["size"]>0):
			byte_counter -= 1
			packet_data["size"] -= 1
			packet_data["data"].append(b)
		if (byte_counter == 9):
			return 0x81
		if (byte_counter == 10):
			byte_counter = 0
			if (len(packet_data["data"])>0 and packet_data["id"] == 0x4):
				graphics(packet_data["data"])
			packet_data = {"id": 0, "size": 0, "data": []}
	return 0

configuration_data = bytearray()
if(len(configuration_data) != 192):
	print("Configuration data file 'mobilegb.cfg' is invalid or does not exist.")
	print("Creating a blank configuration.\n")
	configuration_data = bytearray([0] * 192)

try:
	link = BGBLinkCable('127.0.0.1',8765)
	link.setExchangeHandler(myHandler)
	link.start()
	while True:
		time.sleep(10)
except KeyboardInterrupt:
	print("Saving configuration to 'mobilegb.cfg'.")
	with open('mobilegb.cfg', 'wb') as out:
		out.write(configuration_data)
	out.closed