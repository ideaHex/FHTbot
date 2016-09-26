/**
 * Custom Blocks for Blockly, for the FH_Tbot Project
 */

var leftTurnJSON = {
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
    "colour": 0,
    "tooltip": "",
    "helpUrl": "http://www.example.com/"
};

Blockly.JavaScript['bot_turn_left'] = function (block) {
    var angle_left_turn = block.getFieldValue('Left Turn');
    var value_name = Blockly.JavaScript.valueToCode(block, 'NAME', Blockly.JavaScript.ORDER_ATOMIC);
    // TODO: Assemble JavaScript into code variable.
    var code = 'setOutput(' + 'L' + argument0 + ');\n';
    return code;
};

var rightTurnJSON = {
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
    "colour": 0,
    "tooltip": "",
    "helpUrl": "http://www.example.com/"
};

Blockly.JavaScript['bot_turn_right'] = function (block) {
    var angle_rightangle = block.getFieldValue('rightAngle');
    // TODO: Assemble JavaScript into code variable.
    var code = 'setOutput(' + 'R' + argument0 + ');\n';
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
    "previousStatement": null,
    "nextStatement": null,
    "colour": 90,
    "tooltip": "",
    "helpUrl": "http://www.example.com/"
};

Blockly.JavaScript['bot_move_forward'] = function(block) {
  var number_forwardtravel = block.getFieldValue('forwardTravel');
  // TODO: Assemble JavaScript into code variable.
  var code = 'setOutput(' + 'F' + argument0 + ');\n';
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
    "previousStatement": null,
    "nextStatement": null,
    "colour": 20,
    "tooltip": "",
    "helpUrl": "http://www.example.com/"
};

Blockly.JavaScript['bot_move_backward'] = function(block) {
  var number_forwardtravel = block.getFieldValue('forwardTravel');
  // TODO: Assemble JavaScript into code variable.
  var code = 'setOutput(' + 'B' + argument0 + ');\n';
  return code;
};

var sendTextJSON = {
    "type": "send_to_bot",
    "message0": "Send to Bot ⇒",
    "previousStatement": "String",
    "colour": 120,
    "tooltip": "",
    "helpUrl": "http://www.example.com/"
};


Blockly.JavaScript['send_to_bot'] = function (block) {
    // TODO: Assemble JavaScript into code variable.
    var code = 'setOutput(' + +');\n';
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

Blockly.JavaScript['bot_stop'] = function(block) {
  // TODO: Assemble JavaScript into code variable.
  var code = 'setOutput(' + AS +');\n';
  return code;
};