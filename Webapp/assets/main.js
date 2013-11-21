// Settings
var arduinoUrl = "http://192.168.0.120/";
var myParameters = "?format=json";
var maxTemperature = 120; //this is celsius
var bestTemperature = 83;


$().ready(function() {

    $("#start_page").on("pageshow", function(event, ui) {
        showLoader();
        fetch("start_page");
        // localFetch("start_page");
    });
    
    $("#detailed_page").on("pageshow", function(event, ui) {
        showLoader();
        fetch("detailed_page");
        // localFetch("detailed_page");
    });

    $("#start_page .refresh_button").click(function(e) {
        // e.preventDefault();
        console.log(e);
        // $('#start_page .detailed_button').addClass("ui-btn-active");
        location.reload();
        // fetch("start_page");
        // localFetch("start_page");
    });
    $("#detailed_page .refresh_button").click(function(e) {
        // e.preventDefault();
        // localFetch("detailed_page");
        fetch("detailed_page");
        // location.reload();
    });

});

function localFetch(page) {
    $.mobile.loading( "hide" );

    if(page == "start_page") {
        $("#cup").empty();
        $("#red_box").hide();
        $("#open_message").empty();
        showCoffee();
        // showClose();
        // showOpen(30);
        // if(json.door == 0) {
        // // showCoffee();
        // showOpen();
        // }else if(json.door == 1) {
        //     showClose();
        //     // showCoffee();
        // }
    }else if(page == "detailed_page") {
        $("#sensor_1").empty();
        $("#sensor_2").empty();
        $("#sensor_door").empty();

        $("#sensor_1").html("32.56");
        $("#sensor_2").html("89.43");
        $("#sensor_door").html("1");
    }else{
        showError("wrong pagename in requestArduino(page) in main.js");
    }
}

function showLoader() {
    $.mobile.loading( "show", {
        text: "connecting",
        textVisible: true,
        theme: "a",
        html: ""
        });
}

function setAnimation(elm, xPos, yPos, toX, toY) {
    // $("#myStyle").html('@keyframes myfirst{ from {top:' + xPos + 'px; height: ' + yPos + 'px;} to {top:'+ toX +'px; height:'+ toY +'px;}}');
    // $("#myStyle").append('@-webkit-keyframes myfirst {from {top:' + xPos + 'px; height: ' + yPos + 'px;} to {top:'+ toX +'px; height:'+ toY +'px;}}');
    $("#myStyle").html('@keyframes myfirst{ from {height: ' + yPos + 'px;} to {height:'+ toY +'px;}}');
    $("#myStyle").append('@-webkit-keyframes myfirst {from {height: ' + yPos + 'px;} to {height:'+ toY +'px;}}');
    $(elm).css("-webkit-animation", "myfirst 1s forwards");
}

function fetch(page) {
    $.ajax({
                type: 'GET',
                url: arduinoUrl,
                data: myParameters,
                timeout: 3000,
                async: true,
                jsonpCallback: 'arduinoEthernetComCallback',
                contentType: "application/json",
                dataType: 'jsonp',
                success: function(json) {
                    $.mobile.loading( "hide" );

                    if(page == "start_page") {
                        if(json.door === 0) {
                        //showOpen();
                        showCoffee();
                        }else if(json.door == 1) {
                            // showOpen(100);
                            showCoffee();
                            //showClose();
                        }
                    }else if(page == "detailed_page") {
                        $("#sensor_1").html('<i style="color:#CCC;">temperature-sensors will be applied in August</i>');
                        $("#sensor_2").html('<i style="color:#CCC;">temperature-sensors will be applied in August</i>');
                        $("#sensor_door").html(json.door);
                    }else{
                        showCoffee();
                        //showError("wrong pagename in requestArduino(page) in main.js");
                    }
                },
                error: function(e) {
                    console.log(e.message);
                    showCoffee();
                    //showError("Cannot reach ArduinoBoard");
                }
            });
}

function clearDisplay() {
    $("#cup").empty();
    $("#red_box").hide();
    $("#open_message").empty();
}


function showOpen(percent) {
    $("#cup").html('<img class="tasse" src="assets/StubeCup.svg" alt="Tasse">');
    var tmpHeight = percent * 0.75; //100% = 75px
    setAnimation("#red_box", 129, 0, 54, tmpHeight);
    $("#red_box").show();
    $("#open_message").html('<h4>Open, still heating up!</h4>');
}

function showCoffee() {
    $("#cup").html('<img class="tasse" src="assets/StubeCup.svg" alt="Tasse">');
    setAnimation("#red_box", 120, 20, 50, 80);
    $("#red_box").show();
    $("#open_message").html('<h4>We\'re open</h4>');
}

function showClose() {
    $("#cup").empty();
    $("#cup").html('<img class="tasse" src="assets/StubeCup_closed.svg" alt="Tasse">');
    $("#red_box").hide();
    $("#open_message").html('<h3 style="color:#364851;">Stube is closed!</h3>');
}

function showError(message) {
    $("#cup").empty();
    $("#cup").html('<img class="tasse" src="assets/StubeCup_noConnect.svg" alt="Tasse">');
    $("#red_box").hide();
    $("#message").html('<h3 style="color:#364851;">' + message + '</h3>');
}