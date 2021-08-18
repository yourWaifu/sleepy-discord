//The Module is global, making it great for storing all sorts of things
var Module = {
	'print': function (text) { console.log(text); }
};

Module.webSocketsList = [];
Module.HTTPRequestList = [];
Module.heartbeatIntervals = [];
Module.destroyHandleFromList = function(handle, listOfHandles) {
	listOfHandles[handle] = null;
	//remove all trailing nulls (objects that have been destroyed)
	while (listOfHandles[listOfHandles.length - 1] == null) {
		listOfHandles.pop();
		if (listOfHandles.length == 0) return;
	}
};
Module.toCharStar = function(source) {
	var lengthInBytes        = lengthBytesUTF8(source) + 1;
	var target               = _malloc(lengthInBytes);
	stringToUTF8(source, target, lengthInBytes + 1);
	return target;
};