/**
 * Custom Blocks for Blockly, for the FH_Tbot Project
 */

leftTurnJSON= {
  "type": "bot_left_turn",
  "message0": "Left Turn %1 %2",
  "args0": [
    {
      "type": "field_angle",
      "name": "leftAngle",
      "angle": 180
    },
    {
      "type": "input_value",
      "name": "NAME"
    }
  ],
  "previousStatement": null,
  "nextStatement": null,
  "colour": 0,
  "tooltip": "",
  "helpUrl": "http://www.example.com/"
};

Blockly.JavaScript['bot_turn_left'] = function(block) {
  var angle_left_turn = block.getFieldValue('Left Turn');
  var value_name = Blockly.JavaScript.valueToCode(block, 'NAME', Blockly.JavaScript.ORDER_ATOMIC);
  // TODO: Assemble JavaScript into code variable.
  var code = '...;\n';
  return code;
};

rightTurnJSON= {
  "type": "bot_right_turn",
  "message0": "Right Turn %1 %2",
  "args0": [
    {
      "type": "field_angle",
      "name": "rightAngle",
      "angle": 90
    },
    {
      "type": "input_value",
      "name": "NAME"
    }
  ],
  "previousStatement": null,
  "nextStatement": null,
  "colour": 0,
  "tooltip": "",
  "helpUrl": "http://www.example.com/"
};

Blockly.JavaScript['bot_right_turn'] = function(block) {
  var angle_rightangle = block.getFieldValue('rightAngle');
  var value_name = Blockly.JavaScript.valueToCode(block, 'NAME', Blockly.JavaScript.ORDER_ATOMIC);
  // TODO: Assemble JavaScript into code variable.
  var code = '...;\n';
  return code;
};