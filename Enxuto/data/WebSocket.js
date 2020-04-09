var rainbowEnable = false;
var connection = new WebSocket('ws://'+location.hostname+':81/', ['arduino']);
connection.onopen = function () {
    connection.send('Connect ' + new Date());
};
connection.onerror = function (error) {
    console.log('WebSocket Error ', error);
};
connection.onmessage = function (e) {  
    console.log('Server: ', e.data);
};
connection.onclose = function(){
    console.log('WebSocket connection closed');
};

function sendRGB() {

}

function vermelho(){
        connection.send("sangue");
}
function verde(){
        connection.send("grama");
}
function azul(){
        connection.send("ceu");
}

function amarelo(){
        connection.send("YELLOW")
}