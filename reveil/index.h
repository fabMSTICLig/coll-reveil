
const char MAIN_page[] PROGMEM = R"=====(
<html>
  <head>
    <title>Réveil</title>
    <script>
  var update = null;
  var minuteinput =  null;
  var heureinput = null;
  var aheure =0;
  var aminute =0;
    var horloge = null;
  
    function setAlarme(){
    var xhr = new XMLHttpRequest();
    xhr.open("GET", "/setalarme?heure="+heureinput.value+"&minute="+minuteinput.value, true);
    xhr.addEventListener('readystatechange', function() {
      if (xhr.readyState === XMLHttpRequest.DONE) { 
        getAlarme();
      }
    });
    
    xhr.send(null);
  }
    function getAlarme(){
    var xhr = new XMLHttpRequest();
    xhr.open("GET", "/alarme", true);
    xhr.addEventListener('readystatechange', function() {
      if (xhr.readyState === XMLHttpRequest.DONE) { 
        aheure=xhr.responseText.split(":")[0];
        aminute=xhr.responseText.split(":")[1];
        heureinput.value=aheure;
        minuteinput.value=aminute;
      }
    });
    xhr.send(null);
  }
    function getHorloge(){
    var xhr = new XMLHttpRequest();
    xhr.open("GET", "/horloge", true);
    xhr.addEventListener('readystatechange', function() {
      if (xhr.readyState === XMLHttpRequest.DONE) { 
        horloge.innerHTML=xhr.responseText;
      }
    });
    xhr.send(null);
  }
    window.addEventListener('load', function(){
        update = document.getElementById('modifier');
        minuteinput = document.getElementById('minute');
        heureinput = document.getElementById('heure');
        horloge = document.getElementById('horloge');
        modifier = document.getElementById('modifier');
    
    modifier.addEventListener("click", function()
    {
      setAlarme();
    });
    getAlarme();
    getHorloge();
     
    }, false);
  </script>
  </head>
  <body>
    <div>
    <span id="horloge">08:00</span>
    </div>
  <div>
  <form>
    Alarme:<input type="text" id="heure">:<input type="text" id="minute">
    <br>  
    <input id="modifier" type="button" value="Modifier">
  </form>
  </div>
  </body>
</html>
)=====";
