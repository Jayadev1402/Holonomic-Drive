/*
   HTML and Javascript code
*/
const char body[] PROGMEM = R"===(
<!DOCTYPE html>
<html>
  <style>
  button {
   text-align: center;
   font-size: 50px;
   cursor: pointer;
  }
  </style>
  
  <body>
    <h1>Speed Control</h1>
    <input type="range" min="0" max="100" value="1"  id="sliderSpeed">
    <span id="sliderlabelSpeed">  </span> <br>
    
    <h1>Direction Button</h1>
    <button type="button" onmousedown="mouseDownForward()" onmouseup="mouseUp()"> Forward </button>
    <br>
    <button type="button" onmousedown="mouseDownLeft()" onmouseup="mouseUp()"> Left </button>
    <button type="button" onmousedown="mouseDownLeftTurn()" onmouseup="mouseUp()"> Left Turn </button>
    <button type="button" onmousedown="mouseDownRightTurn()" onmouseup="mouseUp()"> Right Turn </button>
    <button type="button" onmousedown="mouseDownRight()" onmouseup="mouseUp()"> Right </button>
    <br>
    <button type="button" onmousedown="mouseDownBackward()" onmouseup="mouseUp()"> Backward </button><br>
    X-Coordinate: <input type="text" id="xcoord" value="0"><br>
    Y-Coordinate: <input type="text" id="ycoord" value="0"><br><br>
    <p id="autonomousNavigationLabel"> waiting... </p> <br> 
    <button onclick="sendXcoord()">Send X-Coord</button>
    <button onclick="sendYcoord()">Send Y-Coord</button>        
    <button onclick="calibrate()">Calibrate</button>
    <button onclick="startNavigation()">Start Navigation</button><br>
    <button onclick="wallFollowing()">Wall Following</button><br>
    <button onclick="beacon23()">23HZ Beacon Tracking</button><br>

    X-Coord: <p id="viveX"> waiting... </p><br>
    Y-Coord: <p id="viveY"> waiting... </p><br>

  </body>
  
  <script>
    sliderSpeed.onchange = function() {
      var xhttp = new XMLHttpRequest();
      xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
          document.getElementById("sliderlabelSpeed").innerHTML = this.responseText;
        }
      };
      var str = "sliderSpeed?val=";
      var res = str.concat(this.value);
      xhttp.open("GET", res, true);
      xhttp.send();
    }
    function wallFollowing() {
      var xhttp = new XMLHttpRequest();
      xhttp.open("GET", "wallFollowing", true);
      xhttp.send();
    }
    function mouseDownForward() {
      var xhttp = new XMLHttpRequest();
      xhttp.open("GET", "mouseDownForward", true);
      xhttp.send();
    }
    function mouseDownBackward() {
      var xhttp = new XMLHttpRequest();
      xhttp.open("GET", "mouseDownBackward", true);
      xhttp.send();
    }
    function mouseDownRight() {
      var xhttp = new XMLHttpRequest();
      xhttp.open("GET", "mouseDownRight", true);
      xhttp.send();
    }
    function mouseDownLeft() {
      var xhttp = new XMLHttpRequest();
      xhttp.open("GET", "mouseDownLeft", true);
      xhttp.send();
    }
    function mouseDownLeftTurn() {
      var xhttp = new XMLHttpRequest();
      xhttp.open("GET", "mouseDownLeftTurn", true);
      xhttp.send();
    }
    function mouseDownRightTurn() {
      var xhttp = new XMLHttpRequest();
      xhttp.open("GET", "mouseDownRightTurn", true);
      xhttp.send();
    }     
    function mouseUp() {
      var xhttp = new XMLHttpRequest();
      xhttp.open("GET", "mouseUp", true);
      xhttp.send();
    }
    function beacon23() {
      var xhttp = new XMLHttpRequest();
      xhttp.open("GET", "beacon23", true);
      xhttp.send();
    }
    function sendXcoord(){
      var xhttp = new XMLHttpRequest();
      xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
          document.getElementById("autonomousNavigationLabel").innerHTML = this.responseText;
        }
      };     
      var str ="xcoord?val=";
      var num1 =  document.getElementById("xcoord").value;
      var res = str.concat(num1);
      xhttp.open("GET", res, true);
      xhttp.send();      
      }

     function sendYcoord(){
      var xhttp = new XMLHttpRequest();
      xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
          document.getElementById("autonomousNavigationLabel").innerHTML = this.responseText;
        }
      };
      var str ="ycoord?val=";
      var num =  document.getElementById("ycoord").value;
      var res = str.concat(num);
      xhttp.open("GET", res, true);
      xhttp.send();      
      }

     function calibrate(){
      var xhttp = new XMLHttpRequest();
      xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
          document.getElementById("autonomousNavigationLabel").innerHTML = this.responseText;
        }
      };
      xhttp.open("GET", "calibrate", true);
      xhttp.send();      
      }
            
    setInterval(updateX,2500);
    function updateX(){
     var xhttp = new XMLHttpRequest();
      xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
          document.getElementById("viveX").innerHTML = this.responseText;
        }
      };
      xhttp.open("GET", "updateX", true);
      xhttp.send();      
    }
    
    setInterval(updateY,2500);
    function updateY(){
     var xhttp = new XMLHttpRequest();
      xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
          document.getElementById("viveY").innerHTML = this.responseText;
        }
      };
      xhttp.open("GET", "updateY", true);
      xhttp.send();      
    }
    

    function startNavigation(){
      var xhttp = new XMLHttpRequest();
      xhttp.open("GET", "startNavigation", true);
      xhttp.send();      
    }

  </script>
  
</html>
)===";
