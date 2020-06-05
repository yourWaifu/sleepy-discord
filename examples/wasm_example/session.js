function _performRequest(methodName, url, header, headerSize, sizeofPointer, body) {
	var handle = Module.HTTPRequestList.length;
	Module.HTTPRequestList.push(new XMLHttpRequest());
	Module.HTTPRequestList[handle].open(Pointer_stringify(methodName), Pointer_stringify(url), false);
	
	//add headers
	var headerEndPointer = (sizeofPointer * headerSize) + header;
	for (var i = header; i < headerEndPointer; i += sizeofPointer * 2) {
		Module.HTTPRequestList[handle].setRequestHeader(
			Pointer_stringify(getValue(i, "*")),                 //name
			Pointer_stringify(getValue(i + sizeofPointer, "*"))  //value
		);
	}

	Module.HTTPRequestList[handle].send(Pointer_stringify(body));
	
	console.log(Module.HTTPRequestList[handle].responseText);

	return handle;
}

function _getResponseStatusCode(handle) {
	return Module.HTTPRequestList[handle].status;
}

function _getResponseBody(handle) {
	return Module.toCharStar(Module.HTTPRequestList[handle].responseText);
}

function _getResponseHeaders(handle) {
	console.log(Module.HTTPRequestList[handle].getAllResponseHeaders());
	return Module.toCharStar(Module.HTTPRequestList[handle].getAllResponseHeaders());
}

function _destroySession(handle) {
	Module.destroyHandleFromList(handle, Module.HTTPRequestList);
}

mergeInto(LibraryManager.library, {
	performRequest       : _performRequest       ,
	getResponseStatusCode: _getResponseStatusCode,
	getResponseBody      : _getResponseBody      ,
	getResponseHeader    : _getResponseHeaders   ,
	destroySession       : _destroySession
});