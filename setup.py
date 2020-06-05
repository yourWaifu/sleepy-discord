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

def unzipDownload(url, path=None):
	with unzip(download(url), path):
		pass

def mergeDirectories(source, target):
	for file in os.listdir(source):
		shutil.move(source + '/' + file, target)

def installCPR():
	#download and unzip cpr to deps
	unzipDownload("https://github.com/whoshuu/cpr/archive/master.zip", "deps")
	os.rename("deps/cpr-master", "deps/cpr")
	shutil.move("deps/cpr/include/cpr", "deps/include")
	#download and overwrite curl folder and build config and compile
	os.rmdir("deps/cpr/opt/curl")
	unzipDownload("https://github.com/curl/curl/archive/master.zip", "deps/cpr/opt")
	os.rename("deps/cpr/opt/curl-master", "deps/cpr/opt/curl")
	global windows
	global linux
	if windows:
		run("deps\\cpr\\opt\\curl\\buildconf.bat")
		#run("C:\\Program Files (x86)\\Microsoft Visual Studio 12.0\\VC\\bin\\vcvars32.bat")
	if linux:
		os.chmod("./deps/cpr/opt/curl/buildconf", 0o777)
		os.chdir("./deps/cpr/opt/curl")
		run("./buildconf")
		os.chdir("../../../../")

def installWebsocketPP():
	#download and unzip websocketpp
	unzipDownload("https://github.com/zaphoyd/websocketpp/archive/master.zip", "deps")
	shutil.move("deps/websocketpp-master/websocketpp", "deps/include")
	shutil.rmtree("deps/websocketpp-master")
	#download and upzip asio
	unzipDownload("https://github.com/chriskohlhoff/asio/archive/master.zip", "deps")
	shutil.move("deps/asio-master/asio/include/asio", "deps/include")
	shutil.move("deps/asio-master/asio/include/asio.hpp", "deps/include")
	shutil.rmtree("deps/asio-master")
	#download and unzip openssl
	global windows
	if windows:
		#add support for 64 bit
		unzipDownload("https://www.npcglib.org/~stathis/downloads/openssl-1.1.0f-vs2015.7z", "deps")
		shutil.move("deps/openssl-1.1.0f-vs2015/include/openssl", "deps/include")
		mergeDirectories("deps/openssl-1.1.0f-vs2015/lib", "deps/lib")
		shutil.rmtree("deps/openssl-1.1.0f-vs2015")

def installUWebsockets():
	unzipDownload("https://github.com/uNetworking/uWebSockets/archive/master.zip", "deps")
	unzipDownload("https://github.com/madler/zlib/archive/master.zip")

def installOpus():
	#to do unzip and compile to a lib file from the script
	unzipDownload("https://archive.mozilla.org/pub/opus/opus-1.2.1.tar.gz", "deps")
	return

def installSodium():
	global windows
	if windows:
		mkdir("deps/libsodium")
		#download and upzip libSodium
		unzipDownload("https://download.libsodium.org/libsodium/releases/libsodium-1.0.16-msvc.zip", "deps/libsodium")
		mergeDirectories("deps/libsodium/include", "deps/include")
		#to do add support for 64 bit
		mergeDirectories("deps/libsodium/Win32/Release/v141/static", "deps/lib")
		shutil.rmtree("deps/libsodium")

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
	ask("What libraries do you want to use?\n\
		You will most likly want a WebSocket and a HTTP library\n\
		If you plan on using voice, you'll need Opus and Sodium", [
		Option("CPR         : HTTP"        , installCPR        ),
		Option("Websocket++ : WebSocket"   , installWebsocketPP),
		Option("uWebSockets : WebSocket"   , installUWebsockets),
		Option("Opus        : Audio Codec" , installOpus       ),
		Option("Sodium      : Cryptography", installSodium     ),
	],
	None)