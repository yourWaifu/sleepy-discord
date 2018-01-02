function _SendWebSocket(handle, message) {
	console.log("send");
    if (typeof Module.webSocketsList[handle].send != "function") return;
	Module.webSocketsList[handle].send(Pointer_stringify(message));
}

function _closeWebSocket(handle, code, reason) {
	console.log("close");
	Module.webSocketsList[handle].close(code, Pointer_stringify(reason));
	clearInterval(Module.heartbeatIntervals[handle]);
	Module.destroyHandleFromList(handle, Module.webSocketsList    );
	Module.destroyHandleFromList(handle, Module.heartbeatIntervals);
}

function _connectWebSocket(uri) {
	console.log("connect " + Pointer_stringify(uri));
	var handle = Module.webSocketsList.length;
	Module.webSocketsList.push(new WebSocket(Pointer_stringify(uri)));
	Module.webSocketsList[handle].onopen = function (event) {
		console.log("onopen");
	};
	Module.webSocketsList[handle].onmessage = function (event) {
		_passMessageToClient(handle, Module.toCharStar(event.data));
	}
	Module.webSocketsList[handle].onerror = function (event) {
		console.log("onerror");
	}
	Module.webSocketsList[handle].onclose = function (event) {
		console.log("onclose");
	}

	Module.heartbeatIntervals.push("for later");

	return handle;
}

function _runWebSocket(handle) {
	console.log("run");
}

function _heartbeat(handle, heartbeatInterval) {
	console.log("heartbeat" + heartbeatInterval);
	_heartbeatClient(handle);  //send first heartbeat
	Module.heartbeatIntervals[handle] = setInterval(() => { _heartbeatClient(handle) }, heartbeatInterval);
}

mergeInto(LibraryManager.library, {
	SendWebSocket         : _SendWebSocket   ,
	closeWebSocket        : _closeWebSocket  ,
	connectWebSocket      : _connectWebSocket,
	runWebSocket          : _runWebSocket    ,
	heartbeat             :_heartbeat        ,
});