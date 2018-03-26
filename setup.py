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
	def __init__(self, filename, path=None):
		self.filename = filename
		self.path = path
		self.fileType = filename.split('.')[-1]
	def __enter__(self):
		if   self.fileType == "zip":
			with zipfile.ZipFile(self.filename) as outputZip:
				outputZip.extractall(self.path)
		elif self.fileType == "7z" :
			global windows
			if not windows:
				print("7z files is only supported on windows for now")
				return
			#download 7zip for command line
			if not os.path.exists("7za.exe"):
				sevenZipFilename = download("http://www.7-zip.org/a/7za920.zip", "7za920.zip")
				with zipfile.ZipFile(sevenZipFilename) as outputZip:
					outputZip.extract("7za.exe")
			#run 7zip command
			run(["7za.exe", "-y", "x", self.filename, "-o" + self.path if self.path != None else "-o."])
		else:
			print(self.fileType, "is not supported")
	def __exit__(self, type, value, traceback):
		try:
			os.remove(self.filename)
		except:
			pass

def convertFromZip(filename, path=None):
	with unzip(filename, path):
		pass

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
		os.chmod("./deps/cpr/opt/curl/buildconf", 0o777)
		os.chdir("./deps/cpr/opt/curl")
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
		#add support for 64 bit
		filename = download("https://www.npcglib.org/~stathis/downloads/openssl-1.1.0f-vs2015.7z")
		convertFromZip(filename, "deps")
		shutil.move("deps/openssl-1.1.0f-vs2015/include/openssl", "deps/include")
		mergeDirectories("deps/openssl-1.1.0f-vs2015/lib", "deps/lib")
		shutil.rmtree("deps/openssl-1.1.0f-vs2015")


def installUWebsockets():
	print("setup.py doesn't support uWebSockets yet")
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
ask("What OS are you using right now?", [
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
