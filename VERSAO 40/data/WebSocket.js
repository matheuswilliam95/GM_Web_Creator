/*_______________________ Declarações ____________________________________________*/
var rainbowEnable = false;
var connection = new WebSocket('ws://' + location.hostname + ':81/', ['arduino']);
var mensagensRecebidas;
var resultado = [];
// var temperatura = 0;

/*_______________________ Funções Padrões Web Socket _____________________________*/
connection.onopen = function () {
    connection.send('Connect ' + new Date());
};
connection.onerror = function (error) {
    console.log('WebSocket Error ', error);
};
connection.onmessage = function (e) {

    mensagensRecebidas = e.data;
    updateCounterUI(mensagensRecebidas);

    var separando = mensagensRecebidas;
    resultado = separando.split('KK')
    console.log(resultado);

};


connection.onclose = function () {
    console.log('WebSocket connection closed');
};


/*____________________ Atualiza valor temp ____________________________________*/



function updateCounterUI(counter) {
    // console.log(counter);
    document.getElementById('infor-temperatura').innerHTML = (resultado[1] + resultado[2]);
    document.getElementById('infor-umidade').innerHTML = (resultado[4] + resultado[5]);
    document.getElementById('infor-luminosidade').innerHTML = (resultado[7]);

    /////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////                             /////////////////////////////////////////
    //////////////////////            IMPORTANTE        /////////////////////////////////////////
    //////////////////////                              /////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // var nivelTanque01 = (6 - 6 * (resultado[9]) / 100) + "vh";
    // document.getElementById('tanque-01-icone').style.backgroundPositionY = nivelTanque01;

    /////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////                             /////////////////////////////////////////
    //////////////////////            IMPORTANTE        /////////////////////////////////////////
    //////////////////////                              /////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    var temperatura = resultado[1]; // Temperatura ambiente


    //// Controle temperatura tanque 01 ----------------------------------------------------------------------------
    var TempTanque01 = (resultado[11]); //Temperatura tanque 02
    var TempMaxTanque01 = (resultado[12]); //Temperatura máxima tanque 01
    var TempMinTanque01 = (resultado[14]); //Temperatura mínima tanque 01

    document.getElementById('TempMaxTanque01').innerHTML = (TempMaxTanque01 + " °C    T: " + resultado[13]);
    document.getElementById('TempMinTanque01').innerHTML = (TempMinTanque01 + " °C    T: " + resultado[15]);
    // Alterando a cor de acordo com a termperatura
    if (TempTanque01 < 26) {
        document.getElementById('texto-temperatura-tq-01').innerHTML = TempTanque01 + "°C";
        document.getElementById('tanque-01-icone').style.backgroundImage = "url('water1.png')";
        document.getElementById('texto-temperatura-tq-01').style.color = "black";
        document.getElementById('texto-temperatura-tq-01').style.textShadow = "0.1vh 0.2vh 0.3vh #ffffff";
    } else if (TempTanque01 >= 26 & TempTanque01 < 28) {
        document.getElementById('texto-temperatura-tq-01').innerHTML = TempTanque01 + "°C";
        document.getElementById('tanque-01-icone').style.backgroundImage = "url('water2.png')";
        document.getElementById('texto-temperatura-tq-01').style.color = "black";
        document.getElementById('texto-temperatura-tq-01').style.textShadow = "0.1vh 0.2vh 0.3vh #ffffff";
    } else if (TempTanque01 >= 28 & TempTanque01 < 30) {
        document.getElementById('texto-temperatura-tq-01').innerHTML = TempTanque01 + "°C";
        document.getElementById('tanque-01-icone').style.backgroundImage = "url('water3.png')";
        document.getElementById('texto-temperatura-tq-01').style.color = "white";
        document.getElementById('texto-temperatura-tq-01').style.textShadow = "0.1vh 0.2vh 0.3vh #000000";
    } else if (TempTanque01 >= 30) {
        document.getElementById('texto-temperatura-tq-01').innerHTML = TempTanque01 + "°C";
        document.getElementById('tanque-01-icone').style.backgroundImage = "url('water4.png')";
        document.getElementById('texto-temperatura-tq-01').style.color = "white";
        document.getElementById('texto-temperatura-tq-01').style.textShadow = "0.1vh 0.2vh 0.3vh #000000";
    }




    //// Controle temperatura tanque 02 ----------------------------------------------------------------------------
    var TempTanque02 = (resultado[16]); //Temperatura tanque 02
    var TempMaxTanque02 = (resultado[17]); //Temperatura máxima tanque 02
    var TempMinTanque02 = (resultado[19]); //Temperatura mínima tanque 02

    document.getElementById('TempMaxTanque02').innerHTML = (TempMaxTanque02 + " °C    T: " + resultado[18]);
    document.getElementById('TempMinTanque02').innerHTML = (TempMinTanque02 + " °C    T: " + resultado[20]);
    // Alterando a cor de acordo com a termperatura
    if (TempTanque02 < 26) {
        document.getElementById('texto-temperatura-tq-02').innerHTML = TempTanque02 + "°C";
        document.getElementById('tanque-02-icone').style.backgroundImage = "url('water1.png')";
        document.getElementById('texto-temperatura-tq-02').style.color = "black";
        document.getElementById('texto-temperatura-tq-02').style.textShadow = "0.1vh 0.2vh 0.3vh #ffffff";
    } else if (TempTanque02 >= 26 & TempTanque02 < 28) {
        document.getElementById('texto-temperatura-tq-02').innerHTML = TempTanque02 + "°C";
        document.getElementById('tanque-02-icone').style.backgroundImage = "url('water2.png')";
        document.getElementById('texto-temperatura-tq-02').style.color = "black";
        document.getElementById('texto-temperatura-tq-02').style.textShadow = "0.1vh 0.2vh 0.3vh #ffffff";
    } else if (TempTanque02 >= 28 & TempTanque02 < 30) {
        document.getElementById('texto-temperatura-tq-02').innerHTML = TempTanque02 + "°C";
        document.getElementById('tanque-02-icone').style.backgroundImage = "url('water3.png')";
        document.getElementById('texto-temperatura-tq-02').style.color = "white";
        document.getElementById('texto-temperatura-tq-02').style.textShadow = "0.1vh 0.2vh 0.3vh #000000";
    } else if (TempTanque02 >= 30) {
        document.getElementById('texto-temperatura-tq-02').innerHTML = TempTanque02 + "°C";
        document.getElementById('tanque-02-icone').style.backgroundImage = "url('water4.png')";
        document.getElementById('texto-temperatura-tq-02').style.color = "white";
        document.getElementById('texto-temperatura-tq-02').style.textShadow = "0.1vh 0.2vh 0.3vh #000000";
    }
}



// var s = "<div style='width:200px; heigth:500px; background-color: orange;border-bottom: solid; border-left: solid; transform: rotate(180deg);'><div id='tk' style='width:200px; heigth:300px; background-color:blue; margin-bottom:0px;'></div></div><script> document.getElementById('tk').style.heigth=' " + value + "px ';</script>";
// return s;

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

// /*____________________________Change COlor ______________________________________ */

// function changeBackground() {

//     var names = ["AliceBlue", "AntiqueWhite", "Aqua", "Aquamarine", "Azure", "Beige", "Bisque", "Black", "BlanchedAlmond", "Blue", "BlueViolet", "Brown", "BurlyWood", "CadetBlue", "Chartreuse", "Chocolate", "Coral", "CornflowerBlue", "Cornsilk", "Crimson", "Cyan", "DarkBlue", "DarkCyan", "DarkGoldenRod", "DarkGray", "DarkGrey", "DarkGreen", "DarkKhaki", "DarkMagenta", "DarkOliveGreen", "Darkorange", "DarkOrchid", "DarkRed", "DarkSalmon", "DarkSeaGreen", "DarkSlateBlue", "DarkSlateGray", "DarkSlateGrey", "DarkTurquoise", "DarkViolet", "DeepPink", "DeepSkyBlue", "DimGray", "DimGrey", "DodgerBlue", "FireBrick", "FloralWhite", "ForestGreen", "Fuchsia", "Gainsboro", "GhostWhite", "Gold", "GoldenRod", "Gray", "Grey", "Green", "GreenYellow", "HoneyDew", "HotPink", "IndianRed", "Indigo", "Ivory", "Khaki", "Lavender", "LavenderBlush", "LawnGreen", "LemonChiffon", "LightBlue", "LightCoral", "LightCyan", "LightGoldenRodYellow", "LightGray", "LightGrey", "LightGreen", "LightPink", "LightSalmon", "LightSeaGreen", "LightSkyBlue", "LightSlateGray", "LightSlateGrey", "LightSteelBlue", "LightYellow", "Lime", "LimeGreen", "Linen", "Magenta", "Maroon", "MediumAquaMarine", "MediumBlue", "MediumOrchid", "MediumPurple", "MediumSeaGreen", "MediumSlateBlue", "MediumSpringGreen", "MediumTurquoise", "MediumVioletRed", "MidnightBlue", "MintCream", "MistyRose", "Moccasin", "NavajoWhite", "Navy", "OldLace", "Olive", "OliveDrab", "Orange", "OrangeRed", "Orchid", "PaleGoldenRod", "PaleGreen", "PaleTurquoise", "PaleVioletRed", "PapayaWhip", "PeachPuff", "Peru", "Pink", "Plum", "PowderBlue", "Purple", "Red", "RosyBrown", "RoyalBlue", "SaddleBrown", "Salmon", "SandyBrown", "SeaGreen", "SeaShell", "Sienna", "Silver", "SkyBlue", "SlateBlue", "SlateGray", "SlateGrey", "Snow", "SpringGreen", "SteelBlue", "Tan", "Teal", "Thistle", "Tomato", "Turquoise", "Violet", "Wheat", "White", "WhiteSmoke", "Yellow", "YellowGreen"],
//         color = document.getElementById("color").value.trim(),
//         rxValidHex = /^#(?:[0-9a-f]{3}){1,2}$/i,
//         formattedName = color.charAt(0).toUpperCase() + color.slice(1).toLowerCase();

//     if (names.indexOf(formattedName) !== -1 || rxValidHex.test(color)) {
//         document.getElementById("coltext").style.color = color;

//     } else {
//         alert("Invalid CSS Color");
//     }
// }

document.getElementById("submitColor").addEventListener("click", changeBackground, false);
/*_____________________________Fim change color __________________________________ */

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