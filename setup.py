#/usr/bin/env python3
from collections import namedtuple
import urllib.request
import shutil
import zipfile
import os
from subprocess import run
import sys

#values for OS specific stuff
windows = False
linux   = False

def setWindows():
	global windows
	#TODO ask what version of visual studio they are using
	windows = True

def setLinux():
	global linux
	linux   = True

#define functions for installing libraries
def download(url, filename=None):
	if filename == None:
		filename = url.split('/')[-1]
	with urllib.request.urlopen(url) as response, open(filename, 'wb') as outputFile:
		shutil.copyfileobj(response, outputFile)
	return filename

class unzip():
	def __inti__(self, filename, path=None):
		self.filename = filename
		self.path = path
	def __enter__(self):
		with zipfile.ZipFile(filename) as outputZip:
			outputZip.extractall(path)
	def __exit__(self):
		os.remove(filename)

def convertFromZip(filename, path=None):
	with zipfile.ZipFile(filename) as outputZip:
		outputZip.extractall(path)
	os.remove(filename)

def mergeDirectories(source, target):
	for file in os.listdir(source):
		shutil.move(source + '/' + file, target)

def installCPR():
	#download and unzip cpr to deps
	filename = download("https://github.com/whoshuu/cpr/archive/master.zip", "cpr-master.zip")
	convertFromZip(filename, "deps")
	os.rename("deps/cpr-master", "deps/cpr")
	shutil.move("deps/cpr/include/cpr", "deps/include")
	#download and overwrite curl folder and build config and compile
	filename = download("https://github.com/curl/curl/archive/master.zip", "curl-master.zip")
	os.rmdir("deps/cpr/opt/curl")
	convertFromZip(filename, "deps/cpr/opt")
	os.rename("deps/cpr/opt/curl-master", "deps/cpr/opt/curl")
	global windows
	global linux
	if windows:
		run("deps\\cpr\\opt\\curl\\buildconf.bat")
		#run("C:\\Program Files (x86)\\Microsoft Visual Studio 12.0\\VC\\bin\\vcvars32.bat")
	if linux:
		run("./deps/cpr/opt/curl/buildconf")

def installWebsocketPP():
	#download and unzip websocketpp
	filename = download("https://github.com/zaphoyd/websocketpp/archive/master.zip", "websocketpp-master.zip")
	convertFromZip(filename, "deps")
	shutil.move("deps/websocketpp-master/websocketpp", "deps/include")
	shutil.rmtree("deps/websocketpp-master")
	#download and upzip asio
	filename = download("https://github.com/chriskohlhoff/asio/archive/master.zip", "asio-master.zip")
	convertFromZip(filename, "deps")
	shutil.move("deps/asio-master/asio/include/asio", "deps/include")
	shutil.move("deps/asio-master/asio/include/asio.hpp", "deps/include")
	shutil.rmtree("deps/asio-master")
	#download and unzip openssl
	global windows
	if windows:
		#just get it precompiled
		print("This is going to take a while")
		filename = download("https://github.com/David-Reguera-Garcia-Dreg/Precompiled-OpenSSL-Windows/archive/master.zip", "Precompiled-OpenSSL-Windows-master.zip")
		convertFromZip(filename, "deps")
		shutil.move("deps/Precompiled-OpenSSL-Windows-master/openssl-1.1.0e-vs2015/include/openssl", "deps/include")
		mergeDirectories("deps/Precompiled-OpenSSL-Windows-master/openssl-1.1.0e-vs2015/lib", "deps/lib")
		shutil.rmtree("deps/Precompiled-OpenSSL-Windows-master")


def installUWebsockets():
	return

#questions
Option   = namedtuple("Option", ["name", "function"])
def ask(question, options, default=False):
	while True: #makes sure we get a valid answer 
		#ask question
		print('\n', question, sep='')
		for i, option in enumerate(options, 0):
			print('(', i, ") ", option.name, sep='')
		if default != False:
			print("press enter to default")
		#get answer
		chosenOptions = [int(answer) for answer in input('>').split()]
		if chosenOptions == "":
			if default == False:
				continue
			else:
				return default()
		#call option functions
		for option in chosenOptions:
			options[option].function()
		return

#print out instructions
print("***Instructions***\n\
A few questions will be ask and options will be presented. When this\n\
happens, you may press enter to not answer or choose by inputting the number\n\
that comes before the name of option. You can also use spaces to separate\n\
multiple choices.\n")
input("press enter to continue...")

#create deps folder
try:
	os.mkdir("deps")
	os.mkdir("deps/include")
	os.mkdir("deps/lib")
except FileExistsError:
	pass

#ask and install
#ask libraries to get and for what os
ask("What OS are you building Sleepy Discord for?", [
	Option("Windows", setWindows),
	Option("Linux"  , setLinux  ),
	Option("macOS"  , setLinux  ),
],
None)
if windows or linux:
	ask("What libraries do you want to use?", [
		Option("CPR"        , installCPR        ),
		Option("Websocket++", installWebsocketPP),
		Option("uWebSockets", installUWebsockets) 
	],
	None)