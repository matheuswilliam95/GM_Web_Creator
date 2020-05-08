/*_______________________ Declarações ____________________________________________*/
var rainbowEnable = false;
var connection = new WebSocket('ws://' + location.hostname + ':81/', ['arduino']);
var mensagensRecebidas;
var resultado = [];


/*_______________________ Funções Padrões Web Socket _____________________________*/
connection.onopen = function() {
    connection.send('Connect ' + new Date());
};
connection.onerror = function(error) {
    console.log('WebSocket Error ', error);
};
connection.onmessage = function(e) {

    mensagensRecebidas = e.data;
    console.log("recebido " + mensagensRecebidas);
    updateCounterUI(mensagensRecebidas);

    var separando = mensagensRecebidas;
    resultado = separando.split('KK')
    console.log(resultado);

};


connection.onclose = function() {
    console.log('WebSocket connection closed');
};


/*____________________ Atualiza valor temp ____________________________________*/


function updateCounterUI(counter) {
    // console.log(counter);
    document.getElementById('infor-temperatura').innerHTML = (resultado[1] + resultado[2]);
    // console.log(resultado[1]);
    document.getElementById('infor-umidade').innerHTML = (resultado[4] + resultado[5]);
    document.getElementById('infor-luminosidade').innerHTML = (resultado[7]);
    console.log(resultado[10]);
    document.getElementById('RW1').innerHTML = ("Nível: " + resultado[9] + "%");
}


/*________________________ Função JS ____________________________________________*/
function myFunction(valor) {

    var paginaID = document.getElementsByClassName("IDPAGE")[0].innerHTML;
    console.log(paginaID);

    var checkBox = document.getElementById(valor);
    var valorReal = valor;


    if (checkBox.checked == true) {
        var ligaReal = valorReal + 'ligado';
        connection.send(ligaReal);
    } else {
        var desligaReal = valorReal + 'desligado';
        connection.send(desligaReal);
    }
}

/*_________________________ Construtor ___________________________________________*/
function construir() {



    var mydata = JSON.parse(conteudo);
    var contador = Object.keys(mydata).length;
    for (var i = 0; i < contador; i++) {
        var oir = document.getElementsByClassName("icone")[i].src = mydata[i].icon;
        var oir = document.getElementsByClassName("item-descricao")[i].innerHTML = mydata[i].descricao;
    }
}



/* Gouge */
// var opts = {
//     // options here
// };
// var target = document.getElementById('demo'); 
// var gauge = new Gauge(target).setOptions(opts);

// document.getElementById("preview-textfield").className = "preview-textfield";
// gauge.setTextField(document.getElementById("preview-textfield"));

// gauge.maxValue = 3000;
// gauge.setMinValue(0); 
// gauge.set(1250);    
// gauge.animationSpeed = 32

// var opts = {

//     // color configs
//     colorStart: "#6fadcf",
//     colorStop: void 0,
//     gradientType: 0,
//     strokeColor: "#e0e0e0",
//     generateGradient: true,
//     percentColors: [[0.0, "#a9d70b" ], [0.50, "#f9c802"], [1.0, "#ff0000"]],

//     // customize pointer
//     pointer: {
//       length: 0.8,
//       strokeWidth: 0.035,
//       iconScale: 1.0
//     },

//     // static labels
//     staticLabels: {
//       font: "10px sans-serif",
//       labels: [200, 500, 2100, 2800],
//       fractionDigits: 0
//     },

//     // static zones
//     staticZones: [
//       {strokeStyle: "#F03E3E", min: 0, max: 200},
//       {strokeStyle: "#FFDD00", min: 200, max: 500},
//       {strokeStyle: "#30B32D", min: 500, max: 2100},
//       {strokeStyle: "#FFDD00", min: 2100, max: 2800},
//       {strokeStyle: "#F03E3E", min: 2800, max: 3000}
//     ],

//     // render ticks
//     renderTicks: {
//       divisions: 5,
//       divWidth: 1.1,
//       divLength: 0.7,
//       divColor: #333333,
//       subDivisions: 3,
//       subLength: 0.5,
//       subWidth: 0.6,
//       subColor: #666666
//     }

//     // the span of the gauge arc
//     angle: 0.15,

//     // line thickness
//     lineWidth: 0.44,

//     // radius scale
//     radiusScale: 1.0,

//     // font size
//     fontSize: 40,

//     // if false, max value increases automatically if value > maxValue
//     limitMax: false,

//     // if true, the min value of the gauge will be fixed
//     limitMin: false,

//     // High resolution support
//     highDpiSupport: true

// };








/*_____________________________ teste reciver _____________________________________*/

// $(document).ready(function () {

//     let ledStatus = 0; // Status do LED 0 = desligado / 1 = ligado
//     let buttonText = ['OFF', 'ON']; // Texto do botão do LED de acordo com o status
//     let buttonStyle = ['btn btn-lg btn-secondary', 'btn btn-lg btn-success']; // Estilo do botão do LED de acordo com o status

//     // Coloque aqui o IP obtino no ESP8266
//     let con = new WebSocket('ws://192.168.0.31:81/', ['arduino']);

//     // Evento que ocorre quando a placa envia dados
//     let previous = "";
//     con.onmessage = function (evt) {

//         console.log(evt.data);
//         //   if (evt.data != "ON" && evt.data != "OFF"){
//         //       $('#temperature').html(evt.data);
//         //   }
//         //       previous = $('#log').html();
//         //       $('#log').html(previous + '\n' + evt.data);

//     };

//     //$('#led-status').html(buttonText[ledStatus]);

//     // Clique no botão do LED
//     $('#led-status').click(function () {

//         // Muda a classe e texto do botão de acordo com o status
//         ledStatus = (ledStatus + 1) % 2; // Alterna entre 0 e 1
//         $('#led-status').removeClass();
//         $('#led-status').addClass(buttonStyle[ledStatus]);
//         $('#led-status').html(buttonText[ledStatus]);

//         // envia o comando para a placa
//         con.send(buttonText[ledStatus]);


//     });
// });

/*_______________________________Fim test reciver ____________________________________*/