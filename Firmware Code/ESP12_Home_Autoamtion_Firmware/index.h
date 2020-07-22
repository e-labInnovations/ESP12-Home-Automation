char webpage[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>Home Automation</title>
  <style>
    html {font-family: Arial; display: inline-block; text-align: center;}
    h2 {font-size: 3.0rem;}
    p {font-size: 3.0rem;}
    body {max-width: 600px; margin:0px auto; padding-bottom: 25px;}
    .switch {position: relative; display: inline-block; width: 120px; height: 68px} 
    .switch input {display: none}
    .slider {position: absolute; top: 0; left: 0; right: 0; bottom: 0; background-color: #ccc; border-radius: 34px}
    .slider:before {position: absolute; content: ""; height: 52px; width: 52px; left: 8px; bottom: 8px; background-color: #fff; -webkit-transition: .4s; transition: .4s; border-radius: 68px}
    input:checked+.slider {background-color: #2196F3}
    input:checked+.slider:before {-webkit-transform: translateX(52px); -ms-transform: translateX(52px); transform: translateX(52px)}
  </style>
</head>
<body>
  <h2>Home Automation</h2>
  <h4>Device 1</h4>
  <label class="switch">
    <input type="checkbox" onchange="toggleCheckbox(1, load1Status)" id="device1"  \>
    <span class="slider"></span>
  </label>
  <h4>Device 2</h4>
  <label class="switch">
    <input type="checkbox" onchange="toggleCheckbox(2, load2Status)" id="device2"  \>
    <span class="slider"></span>
  </label>
  <h4>Device 3</h4>
  <label class="switch">
    <input type="checkbox" onchange="toggleCheckbox(3, load3Status)" id="device3"  \>
    <span class="slider"></span>
  </label>
  <h4>Device 4</h4>
  <label class="switch">
    <input type="checkbox" onchange="toggleCheckbox(4, load4Status)" id="device4"  \>
    <span class="slider"></span>
  </label>

<script>
  var connection = new WebSocket('ws://'+location.hostname+':81/');
  var device1 = document.getElementById("device1");
  var device2 = document.getElementById("device2");
  var device3 = document.getElementById("device3");
  var device4 = document.getElementById("device4");
  
  var load1Status = false;
  var load2Status = false;
  var load3Status = false;
  var load4Status = false;
  
  connection.onmessage = function(event){
    var full_data = event.data;
    console.log(full_data);
    var data = JSON.parse(full_data);
    updateComp(data);
  }

  function toggleCheckbox(device, status) {
    load1Status = device==1?(!status):load1Status;
    load2Status = device==2?(!status):load2Status;
    load3Status = device==3?(!status):load3Status;
    load4Status = device==4?(!status):load4Status;
    
    var reqObj = {
      "Load1" : load1Status?"HIGH":"LOW",
      "Load2" : load2Status?"HIGH":"LOW",
      "Load3" : load3Status?"HIGH":"LOW",
      "Load4" : load4Status?"HIGH":"LOW"
    }
    connection.send(JSON.stringify(reqObj));
  };
  
  function updateComp(res) {
    load1Status = res.Load1=='HIGH'?true:false;
    load2Status = res.Load2=='HIGH'?true:false;
    load3Status = res.Load3=='HIGH'?true:false;
    load4Status = res.Load4=='HIGH'?true:false;
    device1.checked = load1Status;
    device2.checked = load2Status;
    device3.checked = load3Status;
    device4.checked = load4Status;
  }
  
</script>
</body>
</html>
)=====";
