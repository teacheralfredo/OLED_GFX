Blockly.Blocks["oled_gfx.clear"] = {
	init: function() {
		this.appendDummyInput().appendField(Blockly.Msg.OLED_GFX_TITLE);
	this.appendDummyInput().appendField(Blockly.Msg.OLED_GFX_CLEAR);
        this.appendDummyInput().appendField(Blockly.Msg.OLED_GFX_I2CADDR)
		.appendField(new Blockly.FieldDropdown([
			["0x3C" , "60"],
			["0x3D" , "61"]
			]), "Address");
        this.appendDummyInput().appendField(Blockly.Msg.OLED_GFX_CHIP)
		.appendField(new Blockly.FieldDropdown([
			["SSD1306", "0"],
			["SH1106" , "1"]
			]), "ChipSet");
        this.appendDummyInput().appendField(Blockly.Msg.OLED_GFX_PIN)
		.appendField(new Blockly.FieldDropdown([
			[Blockly.Msg.OLED_GFX_UP , "0"],
			[Blockly.Msg.OLED_GFX_DOWN , "1"]
			]), "Rotation");
	this.setInputsInline(true);
	this.setPreviousStatement(true);
	this.setNextStatement(true);
	this.setColour(160);
	this.setTooltip("");
	this.setHelpUrl("");
}};

Blockly.Blocks["oled_gfx.print"] = {
	init: function() {
		this.appendDummyInput().appendField(Blockly.Msg.OLED_GFX_TITLE);
	this.appendDummyInput().appendField(Blockly.Msg.OLED_GFX_PRINT);
        this.appendDummyInput().appendField(Blockly.Msg.OLED_GFX_I2CADDR)
		.appendField(new Blockly.FieldDropdown([
			["0x3C" , "60"],
			["0x3D" , "61"]
			]), "Address");
	this.appendValueInput("Row").setCheck("Number" ).appendField(Blockly.Msg.OLED_GFX_LINE);
	this.appendValueInput("Col").setCheck("Number" ).appendField(Blockly.Msg.OLED_GFX_COLUMN);
	this.appendValueInput("Str").setCheck("String" ).appendField(Blockly.Msg.OLED_GFX_MESSAGE);
	this.appendDummyInput().appendField(Blockly.Msg.OLED_GFX_FONT)
		.appendField(new Blockly.FieldDropdown(
			[ [ Blockly.Msg.OLED_GFX_SMALL, "0" ], [ Blockly.Msg.OLED_GFX_LARGE, "1" ] ]), "Size");
	this.setInputsInline(true);
	this.setPreviousStatement(true);
	this.setNextStatement(true);
	this.setColour(160);
	this.setTooltip("");
	this.setHelpUrl("");
}};

Blockly.Blocks["oled_gfx.printGFX"] = {
	init: function() {
		this.appendDummyInput().appendField(Blockly.Msg.OLED_GFX_TITLE);
	this.appendDummyInput().appendField(Blockly.Msg.OLED_GFX_PRINTGFX);
        this.appendDummyInput().appendField(Blockly.Msg.OLED_GFX_I2CADDR)
		.appendField(new Blockly.FieldDropdown([
			["0x3C" , "60"],
			["0x3D" , "61"]
			]), "Address");
	this.appendValueInput("Row").setCheck("Number" ).appendField(Blockly.Msg.OLED_GFX_LINE);
	this.appendValueInput("Col").setCheck("Number" ).appendField(Blockly.Msg.OLED_GFX_COLUMN);
	this.appendValueInput("Str").setCheck("String" ).appendField(Blockly.Msg.OLED_GFX_MESSAGETHEN);
	this.setInputsInline(true);
	this.setPreviousStatement(true);
	this.setNextStatement(true);
	this.setColour(160);
	this.setTooltip("");
	this.setHelpUrl("");
}};

Blockly.Blocks['oled_gfx.num2str'] = {
	init: function() {
		this.appendDummyInput().appendField(Blockly.Msg.OLED_GFX_TITLE);
		this.appendDummyInput().appendField(Blockly.Msg.OLED_GFX_NUM2STR);
		//read this in generator V       type check V                     V display this in block
		this.appendValueInput("Value" ).setCheck("Number").appendField(Blockly.Msg.OLED_GFX_NUMBER);
		this.appendValueInput("Width" ).setCheck("Number").appendField(Blockly.Msg.OLED_GFX_WIDTH);
		this.appendDummyInput().appendField(Blockly.Msg.OLED_GFX_DECIMALS)
	 		.appendField(new Blockly.FieldDropdown([
	 			["0", "0"],
				["1", "1"],
	  			["2", "2"],
	  			["3", "3"],
	  			["4", "4"],
	  			["5", "5"],
	  			["6", "6"]
				]), "Decimals");
		this.appendDummyInput().appendField(Blockly.Msg.OLED_GFX_FORMAT)
			.appendField(new Blockly.FieldDropdown([
				[Blockly.Msg.OLED_GFX_INTEGER, "0"],
				[Blockly.Msg.OLED_GFX_HEXADECIMAL, "1"],
				[Blockly.Msg.OLED_GFX_HEXWITH0S, "2"],
				[Blockly.Msg.OLED_GFX_FIXEDPOINT, "3"],
				[Blockly.Msg.OLED_GFX_EXPONENTIAL, "4"]
			   ]), "Format");
		this.setInputsInline(true);
		this.setOutput(true, "String");
		this.setColour(160);
		this.setTooltip("");
		this.setHelpUrl("");
}};
