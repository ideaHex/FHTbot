/**
 * Custom Blocks for Blockly, for the FH_Tbot Project
 */
console.log("Loading custom blocks");

var botTurnLeftJson = {
    "type": "bot_turn_left",
    "message0": "Bot Turn Left: %1",
    "args0": [
        {
            "type": "field_angle",
            "name": "leftAngle",
            "angle": 90
        }
    ],
    "previousStatement": "null",
    "nextStatement": "null",
    "colour": 100,
    "tooltip": "",
    "helpUrl": "http://www.example.com/"
};

Blockly.Blocks['bot_turn_left'] ={
  init: function(){
      this.jsonInit(botTurnLeftJson);
      //Any further JS modification to the BLOCK is done here.
  }  
};

Blockly.JavaScript['bot_turn_left'] = function (block) {
    var angle_leftangle = block.getFieldValue('leftAngle');
    // TODO: Assemble JavaScript into code variable.
    var code = 'sendText(' + '\"L,\" +' + angle_leftangle + '\",\"' + ');\n';
    return code;
};

var botTurnRightJson = {
    "type": "bot_turn_right",
    "message0": "Bot Turn Right: %1",
    "args0": [
        {
            "type": "field_angle",
            "name": "rightAngle",
            "angle": 90
        }
    ],
    "previousStatement": "null",
    "nextStatement": "null",
    "colour": 100,
    "tooltip": "",
    "helpUrl": "http://www.example.com/"
};

Blockly.Blocks['bot_turn_right'] ={
  init: function(){
      this.jsonInit(botTurnRightJson);
      //Any further JS modification to the BLOCK is done here.
  }  
};

Blockly.JavaScript['bot_turn_right'] = function (block) {
    var angle_rightangle = block.getFieldValue('rightAngle');
    // TODO: Assemble JavaScript into code variable.
    var code = 'sendText(' + '\"R,\" +' + angle_rightangle + '\",\"' +');\n';
    return code;
};

var bot_move_forwardJSON = {
    "type": "bot_move_forward",
    "message0": "Travel forward  %1 mm",
    "args0": [
        {
            "type": "field_number",
            "name": "forwardTravel",
            "value": 0
        }
    ],
    "previousStatement": "null",
    "nextStatement": "null",
    "colour": 100,
    "tooltip": "",
    "helpUrl": "http://www.example.com/"
};

Blockly.Blocks['bot_move_forward'] ={
  init: function(){
      this.jsonInit(bot_move_forwardJSON);
      //Any further JS modification to the BLOCK is done here.
  }  
};

Blockly.JavaScript['bot_move_forward'] = function(block) {
  var number_forwardtravel = block.getFieldValue('forwardTravel');
  // TODO: Assemble JavaScript into code variable.
  var code = 'sendText(' + '\"F,\" +' + number_forwardtravel + '\",\"' + ');\n';
  return code;
};

var bot_move_backwardJSON = {
    "type": "bot_move_backward",
    "message0": "Travel backward  %1 mm",
    "args0": [
        {
            "type": "field_number",
            "name": "backwardTravel",
            "value": 0
        }
    ],
    "previousStatement": "null",
    "nextStatement": "null",
    "colour": 100,
    "tooltip": "",
    "helpUrl": "http://www.example.com/"
};

Blockly.Blocks['bot_move_backward'] ={
  init: function(){
      this.jsonInit(bot_move_backwardJSON);
      //Any further JS modification to the BLOCK is done here.
  }  
};

Blockly.JavaScript['bot_move_backward'] = function(block) {
  var number_backwardtravel = block.getFieldValue('backwardTravel');
  // TODO: Assemble JavaScript into code variable.
  var code = 'sendText(' + '\"B,\" +' + number_backwardtravel + '\",\"' + ');\n';
  return code;
};

var botStopJSON = {
  "type": "bot_stop",
  "message0": "Stop Bot",
  "previousStatement": null,
  "nextStatement": null,
  "colour": 0,
  "tooltip": "",
  "helpUrl": "http://www.example.com/"
};

Blockly.Blocks['bot_stop'] ={
  init: function(){
      this.jsonInit(botStopJSON);
      //Any further JS modification to the BLOCK is done here.
  }  
};

Blockly.JavaScript['bot_stop'] = function(block) {
  // TODO: Assemble JavaScript into code variable.
  var code = 'sendText(' + '\"AS,\"' +');\n';
  return code;
};