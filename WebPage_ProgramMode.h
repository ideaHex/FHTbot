const char programMode[] PROGMEM = R"=====(<!DOCTYPE HTML>
<html>
<head>
<style>
#div1, #div2 {
    float: left;
    width: 100px;
    height: 460px;
    margin: 10px;
    padding: 10px;
    border: 2px solid black;
}
</style>
<script>
function allowDrop(ev) {
    ev.preventDefault();
}drag1

function drag(ev) {
    ev.dataTransfer.setData("text", ev.target.id);
}

function drop(ev) {
    ev.preventDefault();
    var data = ev.dataTransfer.getData("text");
   // ev.target.appendChild(document.getElementById(data));
   ev.target.appendChild(document.getElementById(data).cloneNode(true));
}
document.body.addEventListener('touchmove', function(event) {
  event.preventDefault();
}, false);
</script>
</head>
<body>

<h2>Drag and Drop</h2>
<p>Drag the images from the first to the second frame.</p>

<div id="div1" ondrop="drop(event)" >
  <img alt="ball" src="data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAADIAAAAyCAMAAAAp4XiDAAAAGXRFWHRTb2Z0d2FyZQBBZG9iZSBJbWFnZVJlYWR5ccllPAAAAAZQTFRFDQ0N////E72QbwAAAAJ0Uk5T/wDltzBKAAAAdUlEQVR42uzWMRLAIAwDweP/n04VQgyTyCo8FKhmSyzRYnilzeHr/VrxB2aEIIJBAAGhidEgisGgiscgi27QxW1MAinjEcgZi0DSFJFsDjlkT7LtFyu6MEXXsujyO5XkFJ9Tr06JO1PBGSTW7HHGlTbhLgEGAIRYAh3jhqewAAAAAElFTkSuQmCC" draggable="true" ondragstart="drag(event)" id="drag1">
<img alt="ball" src="data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAADIAAAAyCAMAAAAp4XiDAAAAGXRFWHRTb2Z0d2FyZQBBZG9iZSBJbWFnZVJlYWR5ccllPAAAAAZQTFRFDQ0N////E72QbwAAAAJ0Uk5T/wDltzBKAAAAdUlEQVR42uzWMRLAIAwDweP/n04VQgyTyCo8FKhmSyzRYnilzeHr/VrxB2aEIIJBAAGhidEgisGgiscgi27QxW1MAinjEcgZi0DSFJFsDjlkT7LtFyu6MEXXsujyO5XkFJ9Tr06JO1PBGSTW7HHGlTbhLgEGAIRYAh3jhqewAAAAAElFTkSuQmCC" draggable="true" ondragstart="drag(event)" id="drag2" width="88" height="31">
<img alt="ball" src="data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAADIAAAAyCAMAAAAp4XiDAAAAGXRFWHRTb2Z0d2FyZQBBZG9iZSBJbWFnZVJlYWR5ccllPAAAAAZQTFRFDQ0N////E72QbwAAAAJ0Uk5T/wDltzBKAAAAdUlEQVR42uzWMRLAIAwDweP/n04VQgyTyCo8FKhmSyzRYnilzeHr/VrxB2aEIIJBAAGhidEgisGgiscgi27QxW1MAinjEcgZi0DSFJFsDjlkT7LtFyu6MEXXsujyO5XkFJ9Tr06JO1PBGSTW7HHGlTbhLgEGAIRYAh3jhqewAAAAAElFTkSuQmCC" draggable="true" ondragstart="drag(event)" id="drag3" width="88">
</div>

<div id="div2" ondrop="drop(event)" ondragover="allowDrop(event)"></div>

</body>
</html>)=====";
