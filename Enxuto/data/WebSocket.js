/*_______________________ Declarações ____________________________________________*/
var rainbowEnable = false;
var connection = new WebSocket('ws://' + location.hostname + ':81/', ['arduino']);

/*_______________________ Funções Padrões Web Socket _____________________________*/
connection.onopen = function () {
    connection.send('Connect ' + new Date());
};
connection.onerror = function (error) {
    console.log('WebSocket Error ', error);
};
connection.onmessage = function (e) {
    console.log('Server: ', e.data);
};
connection.onclose = function () {
    console.log('WebSocket connection closed');
};



/*_________________________ Funções de Send dados ________________________________*/
function amarelo() {
    connection.send("YELLOW")
}


/*_________________________ Miniminiza o menu ____________________________________*/
$(window).scroll(function() {
    if ($(document).scrollTop() > 100) {
      $('header').addClass('smaller');
    } else {
      $('header').removeClass('smaller');
    }
  });