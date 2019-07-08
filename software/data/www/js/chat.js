
const out = document.getElementById("messages");
var xhr = new XMLHttpRequest();

// we defined the xhr

setInterval(function() {
	    // allow 1px inaccuracy by adding 1
	    const isScrolledToBottom = out.scrollHeight - out.clientHeight <= out.scrollTop + 1;
	    // scroll to bottom if isScrolledToBottom is true
	    console.log(out.scrollHeight - out.clientHeight,  out.scrollTop + 1);
	    if (!isScrolledToBottom) {
	      out.scrollTop = out.scrollHeight - out.clientHeight;
	    }
}, 500)

function startSocket(){
      ws = new WebSocket('ws://'+document.location.host+'/ws',['arduino']);
      ws.binaryType = "arraybuffer";
      ws.onopen = function(e){
        addMessage("SERVER MESSAGE: Connected");
      };
      ws.onclose = function(e){
        addMessage("SERVER MESSAGE: Disconnected");
      };
      ws.onerror = function(e){
        console.log("ws error", e);
        addMessage("Server Message: Error");
      };
      ws.onmessage = function(e){
        var msg = "";
        if(e.data instanceof ArrayBuffer){
          var bytes = new Uint8Array(e.data);
          for (var i = 0; i < bytes.length; i++) {
            msg += String.fromCharCode(bytes[i]);
          }
        } else {
          msg = e.data;
        }
        addMessage(msg);
      };
  }


// setInterval(function(){
// 	xhr.onreadystatechange = function () {
// 	    if (this.readyState != 4) return;

// 	    if (this.status == 200) {
// 	        var message = this.responseText;
// 	        var el = document.getElementById('chat-log');
// 	    	var elChild = document.createElement('div');
// 	    	elChild.append(document.createElement('p'));
// 			var dt = new Date();
// 			var utcDate = dt.toUTCString();
// 			var msg = "(" + utcDate + ") " + "<b>Username: </b>" + message;
// 	 		elChild.lastChild.innerHTML = msg;
// 	 		// Prepend it
// 	 		el.appendChild(elChild);
// 	    }
// 	    // end of state change: it can be after some time (async)
// 	};
// },1000)

function addMessage(message){
    var el = document.getElementById('chat-log');
    var elChild = document.createElement('div');
    elChild.append(document.createElement('p'));
    var dt = new Date().toLocaleTimeString();
    var webmsg = "(" + dt + "): "+ message;
    elChild.lastChild.innerHTML = webmsg;
    el.appendChild(elChild);

}

function send_message() {
		var el = document.getElementById('chat-log');
    var elChild = document.createElement('div');
    elChild.append(document.createElement('p'));
    var message = document.getElementById('input-message').value;
		var dt = new Date().toLocaleTimeString();
		var webmsg = "(" + dt + "): " + message;
 		elChild.lastChild.innerHTML = webmsg;
		var msg = 'msg=' + message;
		xhr.open("POST", "/hthmsg", true);
		xhr.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');
		xhr.send(msg);
 		// Prepend it
 		el.appendChild(elChild);

 		document.getElementById('input-message').value = "";
	}

startSocket();
