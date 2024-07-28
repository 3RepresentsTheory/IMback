var cookie;
var hostname = window.location.hostname;
function clearAllCookies() {
    var cookies = document.cookie.split(";");

    for (var i = 0; i < cookies.length; i++) {
        var cookie = cookies[i];
        var eqPos = cookie.indexOf("=");
        var name = eqPos > -1 ? cookie.substr(0, eqPos) : cookie;
        document.cookie = name + "=;expires=Thu, 01 Jan 1970 00:00:00 GMT;path=/";
    }
}

window.addEventListener("load", clearAllCookies);


function sendLoginRequest() {
    //rewrite code below using fetch api
    var username = document.getElementById("username").value;
    var password = document.getElementById("password").value;
    var json = {
        "username": username,
        "password": password,
    };

    var xhr = new XMLHttpRequest();
    // get the cookie from the response(which is a json)
    xhr.onreadystatechange = function () {
        if (xhr.readyState == 4) {
            var response = JSON.parse(xhr.responseText);
            if (xhr.status == 200) {
                cookie = response.cookie;
                debug("LOGIN SUCCESS,TRY TO LISTEN");
                initWebSocket();
            } else {
                debug("LOGIN FAILED WITH MSG:" + response.msg);
            }
        }
    }
    xhr.open("POST", "http://"+hostname+":1235/user/login");
    xhr.send(JSON.stringify(json));
    xhr.onerror = function (evt) {
        debug('ERROR: ' + evt.data);
    };
}

function sendRegisterRequest(){
    var username = document.getElementById("username").value;
    var nickname = document.getElementById("nickname").value;
    var password = document.getElementById("password").value;
    var color    = document.getElementById("color").value;
    var avatar   = document.getElementById("avatar").value;

    var json = {
        "username": username,
        "nickname": nickname,
        "password": password,
        "color"   : color,
        "avatar"  : avatar

    };

    fetch("http://"+hostname+":1235/user/register",{
        method: "POST",
        body: JSON.stringify(json)
    })
        .then(response => {
            if (response.status == 200) {
                response.json().then(data => debug("REGISTER SUCCESS:"+JSON.stringify(data,null,2)));
            } else {
                response.json().then(data => debug("REGISTER FAILED:"+data.msg));
            }
        })
        .catch(error => debug('Error:', error))
}

function sendMessage() {
    // set the cookie to its self
    document.cookie = cookie;

    debug("MYSELF-SEND: " + document.getElementById("inputText").value);
    var gid = parseInt(document.getElementById("groupid").value, 10);
    if (isNaN(gid)) {
        debug("PLEASE INPUT GROUPID");
        return;
    }
    var type = "text/plain";
    var content = document.getElementById("inputText").value;
    var json = {
        "gid": gid,
        "type": type,
        "content": content
    };
    var strToSend = JSON.stringify(json);
    // send http request use XMLHttpRequest
    var xhr = new XMLHttpRequest();
    // send request to http://localhost:1235,
    // and the header has to take the cookie in header, use http proxy
    xhr.open("POST", "http://"+hostname+":1235/message/send");
    xhr.send(JSON.stringify(json))
    xhr.onreadystatechange = function () {
        if (xhr.readyState == 4) {
            var response = JSON.parse(xhr.responseText);
            if (xhr.status == 200) {
                debug("SEND SUCCESS");
            } else {
                debug("SEND FAILED WITH MSG:" + response.msg);
            }
        }
    }
    xhr.onerror = function (evt) {
        debug('SEND ERROR: ' + evt.data);
    };
}

function getuserinfoinbatch(){
    var userlist = document.getElementById("numberList").value;

    var json = {
        "uids"  : userlist
    };

    fetch("http://"+hostname+":1235/user/infos",{
        method: "POST",
        body: JSON.stringify(json)
    })
        .then(response => {
            if (response.status == 200) {
                response.json().then(data => debug("QUERY SUCCESS:"+JSON.stringify(data,null,2)));
            } else {
                response.json().then(data => debug("QUERY FAILED:"+data.msg));
            }
        })
        .catch(error => debug('Error:', error))
}

function getOnlineUser(){
    var userlist = document.getElementById("numberList").value;

    var json = {
        "uids"  : userlist
    };
    fetch("http://"+hostname+":1235/user/onlines",{
        method: "POST",
        body: JSON.stringify(json)
    })
        .then(response => {
            if (response.status == 200) {
                response.json().then(data => debug("QUERY SUCCESS:"+JSON.stringify(data,null,2)));
            } else {
                response.json().then(data => debug("QUERY FAILED:"+data.msg));
            }
        })
        .catch(error => debug('Error:', error))
}

function requestAddFriend(){
    document.cookie = cookie;
    var friendname = document.getElementById("friendname").value;
    var json = {
        "username"  : friendname,
        "text"      : "服务端已经不处理申请text了"
    };

    fetch("http://"+hostname+":1235/friend/request",{
        method: "POST",
        body: JSON.stringify(json)
    })
        .then(response => {
            if (response.status == 200) {
                response.json().then(data => debug("ADDFRIEND SUCCESS:"+JSON.stringify(data,null,2)));
            } else {
                response.json().then(data => debug("ADDFRIEND FAILED:"+data.msg));
            }
        })
        .catch(error => debug('Error:', error))
}

function getHistoryMessage(){
    document.cookie = cookie
    var gid    = parseInt(document.getElementById("group_hist_id").value, 10);
    var startid= parseInt(document.getElementById("startid").value,10)    ;
    var endid  = parseInt(document.getElementById("endid").value,10);
    if(isNaN(startid)) startid=null
    if(isNaN(endid))   endid=null
    if (isNaN(gid)) {
        debug("PLEASE INPUT GROUPID");
        return;
    }

    rurl = new URL("http://"+hostname+":1235/message/history");
    rurl.searchParams.append('gid',gid)
    rurl.searchParams.append('start',startid)
    rurl.searchParams.append('gid',endid)


    fetch(rurl)
        .then(response => {
            if (response.status == 200) {
                response.json().then(data => debug("QUERY SUCCESS:"+JSON.stringify(data,null,2)));
            } else {
                response.json().then(data => debug("QUERY FAILED:"+data.msg));
            }
        })
        .catch(error => debug('Error:', error))
}


function createGroup(){
    document.cookie = cookie
    var groupname   = document.getElementById("groupname").value;
    var groupcolor  = document.getElementById("groupcolor").value;
    var groupavatar = document.getElementById("groupavatar").value;
    var json = {
        "name"  : groupname,
        "avatar": groupavatar,
        "color" : groupcolor,
    };

    fetch("http://"+hostname+":1235/group/new",{
        method: "POST",
        body: JSON.stringify(json)
    })
        .then(response => {
            if (response.status == 200) {
                response.json().then(data => debug("CREATE GROUP SUCCESS:"+JSON.stringify(data,null,2)));
            } else {
                response.json().then(data => debug("CREATE GROUP FAILED:"+data.msg));
            }
        })
        .catch(error => debug('Error:', error))
}


function sendcookie(msg) {
    var strToSend = msg;
    var json = {
        "cookie":msg,
        "ip":"112.21.12.24",
        "port":"8170",
    }
    console.log(msg);
    if (websocket != null) {
        debug("SEND COOKIE: " + JSON.stringify(json,null,2));
        websocket.send(JSON.stringify(json));
    }
}

var websocket = null;

function initWebSocket() {
    try {
        if (typeof MozWebSocket == 'function')
            WebSocket = MozWebSocket;
        if (websocket && websocket.readyState == 1)
            websocket.close();
        var wsUri = "ws://"+hostname+":1234";
        websocket = new WebSocket(wsUri);
        websocket.onopen = function (evt) {
            debug("WS CONNECTED");
            document.getElementById("sendButton").disabled = false;
            sendcookie(cookie.toString());
        };
        websocket.onclose = function (evt) {
            debug("WS DISCONNECTED");
            document.getElementById("sendButton").disabled = true;
        };
        websocket.onmessage = function (evt) {
            debug("RECEIVE BROADCAST");
            debug(evt.data);
        };
        websocket.onerror = function (evt) {
            debug('WS ERROR: ' + evt.data);
        };
    } catch (exception) {
        debug('ERROR: ' + exception);
    }
}

function stopWebSocket() {
    if (websocket)
        websocket.close();
}

function debug(message) {
    var debugTextArea = document.getElementById("debugTextArea");
    debugTextArea.value += message + "\n";
    debugTextArea.scrollTop = debugTextArea.scrollHeight;
}

var numbers = [];

function addNumber() {
    var numberInput = document.getElementById("numberInput");
    var number = numberInput.value;

    // Add the number to the array
    numbers.push(number);

    // Update the string representation
    updateStringRepresentation();

    // Clear the input field
    numberInput.value = "";
}

function updateStringRepresentation() {
    var stringRepresentation = numbers.join(",");
    var numberList = document.getElementById("numberList");
    numberList.value = stringRepresentation;
}