#!/usr/bin/env python3
# -*- coding: utf-8 -*-

#
# This application demonstrates how to communicate with an oximeter
# using HDP.  This application has been tested against a
# Nonin Onyx II 9560 pulse oximeter.
#

import sys
import os
from gi.repository import GLib
import dbus
import socket
import dbus.service
from gi.repository import GObject
from dbus.mainloop.glib import DBusGMainLoop
from dbus.exceptions import DBusException
import argparse

# from hdp_utils import *
from random import randint

BUS_NAME = 'org.bluez'
PATH = '/org/bluez'
HEALTH_MANAGER_INTERFACE = 'org.bluez.HealthManager1'
HEALTH_DEVICE_INTERFACE = 'org.bluez.HealthDevice1'
HEALTH_CHANNEL_INTERFACE = 'org.bluez.HealthChannel1'

class MessageType:
    (Association, Configuration, Release_Request,
     Release_Confirmation, Data, Unknown) = range(0, 6)

class HdpMessage:
	def getAssociationResponse(self, invokeId):
		return bytes((
			0xe3, 0x00, #APDU CHOICE Type(AareApdu)
			0x00, 0x2c, #CHOICE.length = 44
			0x00, 0x00, #result=accept (known config)
			0x50, 0x79, #data-proto-id = 20601
			0x00, 0x26, #data-proto-info length = 38
			0x80, 0x00, 0x00, 0x00, #protocolVersion
			0x80, 0x00, #encoding rules = MDER
			0x80, 0x00, 0x00, 0x00, #nomenclatureVersion
			0x00, 0x00, 0x00, 0x00, #functionalUnits, normal Association
			0x80, 0x00, 0x00, 0x00, #systemType = sys-type-manager
			0x00, 0x08, #system-id length = 8 and value (manufacturer- and device- specific)
			0x88, 0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x11,
			0x00, 0x00, #Manager's response to config-id is always 0
			0x00, 0x00, #Manager's response to data-req-mode-flags is always 0
			0x00, 0x00, #data-req-init-agent-count and data-req-init-manager-count are always 0
			0x00, 0x00, 0x00, 0x00, #optionList.count = 0 | optionList.length = 0
			))

	def getConfigurationResp(self, invokeId):
		return bytes((
			0xe7, 0x00, #APDU CHOICE Type(PrstApdu)
			0x00, 0x16, #CHOICE.length = 22
			0x00, 0x14, #OCTET STRING.length = 20
			invokeId[0], invokeId[1], #invoke-id (mirrored from invocation)
			0x02, 0x01, #CHOICE(Remote Operation Response | Confirmed Event Report)
			0x00, 0x0e, #CHOICE.length = 14
			0x00, 0x00, #obj-handle = 0 (MDS object)
			0x00, 0x00, 0x00, 0x00, #currentTime = 0
			0x0d, 0x1c, #event-type = MDC_NOTI_CONFIG
			0x00, 0x04, #event-reply-info.length = 4
			0x40, 0x00, # ConfigReportRsp.config-report-id=0x4000
			0x00, 0x00  # ConfigReportRsp.config-result = accepted-config
			))

	def getReleaseRequest(self, invokeId):
		return bytes((0xe4, 0x00, 0x00, 0x02, 0x00, 0x00))

	def getReleaseResponse(self, invokeId):
		return bytes((0xe5, 0x00, 0x00, 0x02, 0x00, 0x00))

	def getDataResponse(self, invokeId):
		return bytes((
			0xe7, 0x00, #APDU CHOICE Type(PrstApdu)
			0x00, 0x12, #CHOICE.length = 18
			0x00, 0x10, #OCTET STRING.length = 16
			invokeId[0], invokeId[1], #invoke-id (mirrored from invocation)
			0x02, 0x01, #CHOICE(Remote Operation Response | Confirmed Event Report)
			0x00, 0x0a, #CHOICE.length = 10
			0x00, 0x00, #obj-handle = 0 (MDS object)
			0x00, 0x00, 0x00, 0x00, #currentTime = 0
			0x0d, 0x1d, #event-type = MDC_NOTI_SCAN_REPORT_FIXED
			0x00, 0x00, #event-reply-info.length = 0
			))

	def parse(self, string_msg):
		#
		# Parse a receive message.  This is just example code and
		# should not be used in any real application.  HDP messages
		# are encoded using IEEE 11073 encoding rules.  Since these
		# rules use ASN.1 encoding, it is possible that fields could
		# move in messages.  This routine does not handle that
		# possibility.  Instead it assumes the fields are in fixed
		# locations, which they appear to be in the units we tested
		# against.  However, in a shipping medical device, you would
		# want to handle things like this.
		#
		msg_type = MessageType.Unknown
		invokeId = (0, 0)
		sp02 = 0
		pulse = 0
		if debugOn:
			print("IEEE opcode received: %x, length = %d" % (int(string_msg[0]), len(string_msg)))
			for i in range(len(string_msg)):
				if ((i & 15) == 0):
					print
				print('%2.2X' % int(string_msg[i]),)
			print

		if int(string_msg[0]) == 0xe2:
			msg_type = MessageType.Association
		elif int(string_msg[0]) == 0xe7:
			invokeId = int(string_msg[6]), int(string_msg[7])
			if int(string_msg[18]) == 0x0d and int(string_msg[19]) == 0x1c:
				msg_type = MessageType.Configuration
			else:
				msg_type = MessageType.Data
				sp02 = int(string_msg[35])
				pulse = int(string_msg[49])
		elif int(string_msg[0]) == 0xe4:
			msg_type = MessageType.Release_Request
		elif int(string_msg[0]) == 0xe5:
			msg_type = MessageType.Release_Confirmation
		else:
			msg_type = MessageType.Unknown

		return (msg_type, invokeId, sp02, pulse)



def receive_data(sk, evt):
	#
	# This method is called when we receive an event on our socket.
	# It's probably either a message from the oximeter or a disconnect
	# indicate.  Deal with it.
	#
	data = None
	disconnecting = False
	hdp = HdpMessage()
	if evt & GLib.IO_IN:
		try:
			data = sk.recv(1024)
		except IOError:
			data = ""
		if data:
			result = hdp.parse(data)
			msgType = result[0]
			invokeId = result[1]
			if msgType == MessageType.Association:
				if debugOn:
					print("Oximeter has associated")
				sk.send(hdp.getAssociationResponse(invokeId))
			elif msgType == MessageType.Configuration:
				if debugOn:
					print("Received configuration data")
				sk.send(hdp.getConfigurationResponse(invokeId))
			elif msgType == MessageType.Release_Request:
				if debugOn:
					print("Received release request")
				sk.send(hdp.getReleaseResponse(invokeId))
				disconnecting = True
			elif msgType == MessageType.Release_Confirmation:
				if debugOn:
					print("Received release confirmation")
				disconnecting = True
			elif msgType == MessageType.Data:
				sk.send(hdp.getDataResponse(invokeId))
				sp02 = result[2]
				pulse = result[3]
				if debugOn:
					print("Received data from oximeter")
				print("SpO2 Level: %d, Beats/second: %d" % \
				(result[2], result[3]))
				if debugOn:
					print("Sending disconnect")
				sk.send(hdp.getReleaseRequest(invokeId))
			else:
				print("Received unknown message, disconnecting")
				sk.send(hdp.getReleaseRequest(invokeId))
				disconnecting = True

	if disconnecting or evt != GLib.IO_IN or not data:
		try:
			sk.shutdown(2)
		except IOError:
			pass
		sk.close()
		print("Disconnected from oximeter")
		return False
	else:
		return True


class SignalHandler(object):
	def __init__(self):
		bus.add_signal_receiver(self.ChannelConnected,
			signal_name="ChannelConnected",
			bus_name=BUS_NAME,
			path_keyword="device",
			interface_keyword="interface",
			dbus_interface=HEALTH_DEVICE_INTERFACE)

		bus.add_signal_receiver(self.ChannelDeleted,
			signal_name="ChannelDeleted",
			bus_name=BUS_NAME,
			path_keyword="device",
			interface_keyword="interface",
			dbus_interface=HEALTH_DEVICE_INTERFACE)

	def ChannelConnected(self, channel, interface, device):
		print("%s has connected" % device)
		if debugOn:
			print("Channel: %s" % channel)
		#
		# The oximeter has connected to us.  Let's get
		# a socket for the connection.
		#
		try:
			channel = bus.get_object(BUS_NAME, channel)
			channel = dbus.Interface(channel, HEALTH_CHANNEL_INTERFACE)
			fd = channel.Acquire()
			fd = fd.take()
			sk = socket.fromfd(fd, socket.AF_UNIX, socket.SOCK_STREAM)
			os.close(fd)

			# Now set up our receiver function to be called
			# when interesting events are detected on that
			# socket
			watch_bitmap = GLib.IO_IN | GLib.IO_ERR| GLib.IO_HUP |  GLib.IO_NVAL
			GLib.io_add_watch(sk, watch_bitmap, receive_data)
		except DBusException:
			print("Error communicating with Oximeter.")
			print("Please make sure the Oximeter has fresh batteries.")

	def ChannelDeleted(self, channel, interface, device):
		print("Device %s channel %s deleted" % (device, channel))

parser = argparse.ArgumentParser()
parser.add_argument("-d", "--debug", help="supply debug output",
                    action="store_true")
args = parser.parse_args()
debugOn = args.debug

DBusGMainLoop(set_as_default=True)
loop = GObject.MainLoop()
bus = dbus.SystemBus()

signal_handler = SignalHandler()

#
# This dictionary holds the parameters that will be passed to the
# Bluez CreateApplication method.  Note that we explicitly set the
# signature to indicate each entry is a string/variant pair.  If
# you don't do this then you will get a cryptic error when you call
# the CreateApplication method.
#
config = dbus.Dictionary({"Role": "Sink", "DataType": dbus.types.UInt16(0x1004),
		"Description": "Oximeter sink"}, signature='sv')

manager = dbus.Interface(bus.get_object(BUS_NAME, PATH),
					HEALTH_MANAGER_INTERFACE)
app = manager.CreateApplication(config)
print("HDP application created, waiting for connection from")
print("a pulse oximeter.  Press control-c to terminate.")

try:
	loop = GLib.MainLoop()
	loop.run()
except KeyboardInterrupt:
	pass
finally:
	manager.DestroyApplication(app)
	print
	print("Application stopped")
	print
