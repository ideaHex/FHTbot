#!/bin/bash
cd ../
#Copy web pages to data and compress
cp -u ./WebControls/public_html/TurtleMode.html ./data/T/TurtleMode.html
gzip -7 -f ./data/T/TurtleMode.html
#Copy JS to data, compress
cp -u ./WebControls/public_html/js/blocklyBall.js ./data/T/js/blocklyBall.js 
gzip -7 -f ./data/T/js/blocklyBall.js
#msg folder
cp -u ./WebControls/public_html/js/msg/messages.js ./data/T/js/msg/messages.js 
gzip -7 -f ./data/T/js/msg/messages.js
#msg/js
cp -u ./WebControls/public_html/js/msg/js/en.js ./data/T/js/msg/js/en.js 
gzip -7 -f ./data/T/js/msg/js/en.js
cp -u ./WebControls/public_html/js/msg/js/en.js ./data/T/js/msg/js/en-gb.js 
gzip -7 -f ./data/T/js/msg/js/en-gb.js 
#msg/json
cp -u ./WebControls/public_html/js/msg/json/en.json ./data/T/js/msg/json/en.json 
gzip -7 -f ./data/T/js/msg/json/en.json
cp -u ./WebControls/public_html/js/msg/json/en-gb.json ./data/T/js/msg/json/en-gb.json
gzip -7 -f ./data/T/js/msg/json/en-gb.json









