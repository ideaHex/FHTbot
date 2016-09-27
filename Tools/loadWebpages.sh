#!/bin/bash
cd ../
#Copy web pages to data and compress
cp -u ./WebControls/public_html/TurtleMode.html ./data/T/TurtleMode.html
gzip -7 -f -k ./data/T/TurtleMode.html
#Copy CSS to data and compress
cp -u ./WebControls/public_html/controlcss.css ./data/T/controlcss.css
gzip -7 -f -k ./data/T/controlcss.css
#Copy JS to data, compress
cp -u ./WebControls/public_html/js/acorn.js ./data/T/js/acorn.js 
gzip -7 -f -k ./data/T/js/acorn.js
cp -u ./WebControls/public_html/js/blockly.js ./data/T/js/blockly.js 
gzip -7 -f -k ./data/T/js/blockly.js 
cp -u ./WebControls/public_html/js/blocks.js ./data/T/js/blocks.js 
gzip -7 -f -k ./data/T/js/blocks.js 
cp -u ./WebControls/public_html/js/javascript.js ./data/T/js/javascript.js 
gzip -7 -f -k ./data/T/js/javascript.js
cp -u ./WebControls/public_html/js/botBlocks.js ./data/T/js/botBlocks.js 
gzip -7 -f -k ./data/T/js/botBlocks.js
#msg folder
cp -u ./WebControls/public_html/js/msg/messages.js ./data/T/js/msg/messages.js 
gzip -7 -f -k ./data/T/js/msg/messages.js
#msg/js
cp -u ./WebControls/public_html/js/msg/js/en.js ./data/T/js/msg/js/en.js 
gzip -7 -f -k ./data/T/js/msg/js/en.js
cp -u ./WebControls/public_html/js/msg/js/en.js ./data/T/js/msg/js/en-gb.js 
gzip -7 -f -k ./data/T/js/msg/js/en-gb.js 
#msg/json
cp -u ./WebControls/public_html/js/msg/json/en.json ./data/T/js/msg/json/en.json 
gzip -7 -f -k ./data/T/js/msg/json/en.json
cp -u ./WebControls/public_html/js/msg/json/en-gb.json ./data/T/js/msg/json/en-gb.json
gzip -7 -f -k ./data/T/js/msg/json/en-gb.json









