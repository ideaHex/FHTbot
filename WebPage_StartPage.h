/*
 * some images from http://freedesignfile.com/
 * 
 */

const char startPage[] PROGMEM = R"=====(<!doctype html>
<html>
  <head>
    <title>FH@Tbot</title>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, height=device-height, user-scalable=yes">    
    <style>
    body{margin: auto;
      background-color: #333;)=====";
      
const char startPage1[] PROGMEM = R"=====(color: white;
      font-size: 20px;
      text-shadow: 2px 2px Black;
    }
    h1 {
        color: red;
        font-size: 40px;
        text-shadow: 2px 2px Black;
    }
.startButton{
    position: relative;
  }
    </style>
  </head>
  <body>
    <div>
      <h1 align="center"><font face="Helvetica Neue" >FH@Tbot</font></h1>
      <p align="center">
      Welcome to the Flinders / hackerspace at tonsley bot web control interface.<br>
      </p><br>
      <div class="startButton">
  <a href="Start" target="_self"><img src="StartBtn.png" alt="Button" style="width: 50%;height: auto;padding-Left: 25%;"></a>
</div>
    </div>
    <br><br>
    <table style="width:100%"><tr>
    <th><a href="Prog"><button style="width:90%;height:30px;">Program Mode</button></a></th>
    <th><a href="anot1"><button style="width:90%;height:30px;">Another Mode</button></a></th>
    <th><a href="anot2"><button style="width:90%;height:30px;">Another Mode</button></a></th></tr>
    </table>
    <br><br>
    <table style="width:100%"><tr>
    <th><a href="Credits" style="background-color: #333;">Credits</a></th>
    <th><a href="About" style="background-color: #333;">About</a></th>
    </tr>
    </table>
</body>
</html>)=====";
