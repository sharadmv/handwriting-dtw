WEB_SOCKET_SWF_LOCATION = "web-socket-js/WebSocketMain.swf";

window.leap = {
	lastMessage: 0,				// timestamp of last frame received
	flushTimeout: 500,			// time until a hand gets deleted from history, after disappearing
	memory: 60,					// # of frames kept in history for each hand
	sampleRate: 1,				// how often frames get saved to history
	refreshTimeout: 1000,		// wait time to try and reconnect if server fails
	refreshTries: 10,			// # of times client will try to reconnect
	refreshCounter: 0,			
	debug: false,				// if true, will print every JSON object received and keep hands in history
	
	currentFrame: {},			// latest fram received
	hands: {},					// stores all hands currently in view
	fingers: {},				// data structure to easily access fingers
	events: {}, 				// "event": boolean function(handID, data) returns boolean
	handlers: {
		'enter': function(handID, data){}, 
		'exit': function(handID, data){},
		'frame': function(handID, data) {
			return data;
		},
		'ready': function(handID, data) {}
	}, 							// "event": function(handID, data)
	
	event: function(eventName, eventFunc) {				// leap.event(name, function) creates new event
		this.events[eventName] = eventFunc;
	},
	on: function(eventName, handlerFunc) {				// leap.on(name, function) assigns handler to event
		this.handlers[eventName] = handlerFunc;
	},
	hand: function(handID, index) {						// leap.hand(handID, index) gets a hand "index" frames ago
		if (index != undefined) {
			var history = this.hands[handID].history;
			return history[(history.length-index-1) % history.length];
		}
		else {
			return this.hands[handID].latest;
		}
	},
	finger: function(fingerID, index) {					// leap.finger(fingerID, index) gets a finger "index" frames ago
		var fingerRef = this.fingers[fingerID];
		try {
			var fingerObj = this.hand(fingerRef.parent, index).fingers[fingerRef.index];
			fingerObj.parent = fingerRef.parent;
			return fingerObj;
		}
		catch(e) {
			console.log("finger", fingerID, "no longer exists");
		}
	},
	palm: function(handID, index) {						// leap.palm(handID, index) gets a palm "index" frames ago
		return this.hand(handID, index).palm;
	},
	fire: function(eventName, handID, data) {			// leap.fire(name, handID, data) manually fires an event
		this.handlers[eventName](handID, data);
	},
}

// Travel back in time and kill yourself
function looper(handID, past) {
	var now = leap.hand(handID).time;
	if (now == past) {
		leap.fire('exit', handID, leap.hands[handID]);
		delete leap.hands[handID];
	}
	else {
		setTimeout(function(){
			looper(handID, past);
		}, leap.flushTimeout);
	}
}

function onopen(message) {
	console.log("connected to leap");
	leap.socket.send("hello");
	leap.fire("ready");
	leap.refreshCounter = 0;
};

function onmessage(message) {
	currentFrame = JSON.parse(message.data);
	leap.currentFrame = currentFrame;
	leap.lastMessage = currentFrame.timestamp;
	leap.fire('frame', undefined, currentFrame);
	
	if (leap.debug) {
		console.log(currentFrame);
	}
	
	for (var i=0; i<currentFrame.hands.length; i++) {
		var hand = currentFrame.hands[i],
			handID = hand.id;
	
		var latest = {
			time: currentFrame.timestamp / 1000,
			frameID: currentFrame.id,
			fingers: hand.fingers,
			palm: hand.palm,
		};
		
		if (handID in leap.hands) {
			leap.hands[handID].latest = latest;
			if (currentFrame.id % leap.sampleRate == 0) {
				leap.hands[handID].history.push(latest);
			}
			if (leap.hands[handID].history.length > leap.memory) {
				leap.hands[handID].history.shift();
			}
		}
		else {
			leap.hands[handID] = {
				latest: latest,
				history: [latest],
			};
			leap.fire('enter', handID, latest);
			if (!leap.debug) {
				looper(handID, latest.time);
			}
		}
		
		for (var j=0; j<hand.fingers.length; j++) {
			leap.fingers[hand.fingers[j].id] = {
				parent: handID,
				index: j,
			};
		}
		
		for (var eventName in leap.events) {
			if (leap.handlers.hasOwnProperty(eventName)) {
				var result = leap.events[eventName](handID);
				if (result.ready) {
					leap.fire(eventName, handID, result.data);
				}
			}
		}
	}
};

function onclose() {
	if (leap.refreshCounter < leap.refreshTries) {
		leap.refreshCounter++;
		setTimeout(start, leap.refreshTimeout);
	}
	else {
		console.log("leap disconnected");
	}
};

function start() {
	console.log("connecting...");
	leap.socket = new WebSocket("ws://localhost:6437");
	leap.socket.onopen = onopen;
	leap.socket.onmessage = onmessage;
	leap.socket.onclose = onclose;
};

start();
