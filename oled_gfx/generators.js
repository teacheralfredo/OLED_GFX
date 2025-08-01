Blockly.JavaScript['oled_gfx.clear'] = function(block) {
	var adr = block.getFieldValue("Address");
	var chp = block.getFieldValue("ChipSet");
	var pin = block.getFieldValue("Rotation");
	return 'DEV_I2C1.OLED_GFX(0, ' +adr+ ').clear(' +chp+','+pin+ ');\n';
};
Blockly.JavaScript['oled_gfx.print'] = function(block) {
	var adr = block.getFieldValue("Address");
	var lin = Blockly.JavaScript.valueToCode(block, 'Row', Blockly.JavaScript.ORDER_ASSIGNMENT) || '0';
	var col = Blockly.JavaScript.valueToCode(block, 'Col', Blockly.JavaScript.ORDER_ASSIGNMENT) || '0';
	var msg = Blockly.JavaScript.valueToCode(block, 'Str', Blockly.JavaScript.ORDER_ASSIGNMENT) || '""';
	var siz = block.getFieldValue("Size");
	return 'DEV_I2C1.OLED_GFX(0, ' +adr+ ').print(' +lin+','+col+','+msg+','+siz+ ');\n';
};
Blockly.JavaScript['oled_gfx.printGFX'] = function(block) {
	var adr = block.getFieldValue("Address");
	var lin = Blockly.JavaScript.valueToCode(block, 'Row', Blockly.JavaScript.ORDER_ASSIGNMENT) || '0';
	var col = Blockly.JavaScript.valueToCode(block, 'Col', Blockly.JavaScript.ORDER_ASSIGNMENT) || '0';
	var msg = Blockly.JavaScript.valueToCode(block, 'Str', Blockly.JavaScript.ORDER_ASSIGNMENT) || '""';
	return 'DEV_I2C1.OLED_GFX(0, ' +adr+ ').printGFX(' +lin+','+col+','+msg+ ');\n';
};

Blockly.JavaScript['oled_gfx.num2str'] = function(block) {
	var fmt = block.getFieldValue("Format");
	var dec = block.getFieldValue("Decimals");
	var wid = Blockly.JavaScript.valueToCode(block, 'Width', Blockly.JavaScript.ORDER_ASSIGNMENT) || '6';
	var val = Blockly.JavaScript.valueToCode(block, 'Value', Blockly.JavaScript.ORDER_ASSIGNMENT) || '0';
	return ['OLED_GFX::num2str('+ val +','+ wid +','+ dec +','+ fmt +')', Blockly.JavaScript.ORDER_ATOMIC];
};