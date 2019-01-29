/*
!!! NOTE !!!
This is not a noramal javascript script. You much use this using Windows Script Host.
*/
var wscriptShell = new ActiveXObject("WScript.Shell");
var fileSystem = new ActiveXObject("Scripting.FileSystemObject");

// Get git
var git = "git";

function testGit() {
	try {
		wscriptShell.Exec(git + " --version");
	} catch (error) {
		return false;
	}
	return true;
}

if (!testGit()) {
	try {
		git = wscriptShell.RegRead("HKLM\\SOFTWARE\\GitForWindows\\InstallPath") + "\\bin\\git.exe";
	} catch (error) { /*do notthing*/ }
	if (!testGit()) {
		WScript.Echo("Could not find git for windows values in windows registry. Try installing git for windows\n");
		WScript.Quit(1);
	}
}

function readCommandLine(command) {
	try {
		return wscriptShell.Exec(command).StdOut.ReadLine();
	} catch (error) {
		WScript.Echo("Failed to exec " + command);
		WScript.Quit(1);
	}
}

var isInGitRepo = readCommandLine(git + " rev-parse --is-inside-work-tree");
if (isInGitRepo !== "true") {
	WScript.Echo("Not in a git repo");
	WScript.Quit(0);
}
var build  = readCommandLine(git + " rev-list --count HEAD");
var branch = readCommandLine(git + " rev-parse --abbrev-ref HEAD");
var hash   = readCommandLine(git + " rev-parse HEAD");
var isMaster = branch === "master" ? "1" : "0";
var description = readCommandLine(git + " describe --always --long --dirty");
var descriptionConcat = description === "" ? "" : " ";
var outputContent = "/*\n\
!!!!!!WARNING!!!!!!\n\
version.h is an auto generated file.Any changes in version.h while be replaced.\n\
However, version.h.in, not be confused with version.h, is ok to make changes to.\n\
*/\n\
\n\
#pragma once\n\
\n\
#define SLEEPY_DISCORD_VERSION_BUILD " + build + "\n\
#define SLEEPY_DISCORD_VERSION_BRANCH \"" + branch + "\"\n\
#define SLEEPY_DISCORD_VERSION_HASH \"" + hash + "\"\n\
#define SLEEPY_DISCORD_VERSION_IS_MASTER " + isMaster + "\n\
#define SLEEPY_DISCORD_VERSION_DESCRIPTION_CONCAT \"" + descriptionConcat + "\"\n\
#define SLEEPY_DISCORD_VERSION_DESCRIPTION \"" + description + "\"";

var outputFile = "../include/sleepy_discord/version.h";
var outputFileContent;
try {
	outputFileContent = fileSystem.OpenTextFile(outputFile).ReadAll();
} catch (error) {
	outputFileContent = ""; //file doesn't exist
}

if (outputContent === outputFileContent) {
	WScript.Echo(outputFile + " doesn't need to be updated. " + description);
} else {
	fileSystem.CreateTextFile(outputFile, true).Write(outputContent);
	WScript.Echo(outputFile + " updated to " + description);
}
