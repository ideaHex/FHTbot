/*
Copyright 2017, Tilden Groves, Alexander Battarbee.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/
#include "FlashFiles.h"

void sendFile(String path, ESP8266WebServer * server){
  #ifdef Diag
  Serial.println("handleFileRead: " + path);
  #endif
  if(path.endsWith("/")) path += "index.html";
  #ifdef Diag
  Serial.println(path);
  #endif
  String contentType = getContentType(path);
  String pathWithGz = path + ".gz";
  if(SPIFFS.exists(pathWithGz) || SPIFFS.exists(path)){
    if(SPIFFS.exists(pathWithGz))
      path += ".gz";
    File file = SPIFFS.open(path, "r");
	server->setContentLength(file.size());
    size_t sent = server->streamFile(file, contentType);
    file.close();
    return;
  }
  #ifdef Diag
  Serial.println("Failed to read file" + path);
  #endif
  server->send(404, "text/plain", "Failed to read file.");
  return;
}

String getContentType(String path) { // get content type
    String dataType = F("text/html");
    String lowerPath = path.substring(path.length() - 4, path.length());
    lowerPath.toLowerCase();
    if (lowerPath.endsWith(".src")) lowerPath = lowerPath.substring(0, path.lastIndexOf("."));
    else if (lowerPath.endsWith(".gz")) dataType = F("application/x-gzip");
    else if (lowerPath.endsWith(".html")) dataType = F("text/html");
    else if (lowerPath.endsWith(".htm")) dataType = F("text/html");
    else if (lowerPath.endsWith(".png")) dataType = F("image/png");
    else if (lowerPath.endsWith(".js")) dataType = F("application/javascript");
    else if (lowerPath.endsWith(".css")) dataType = F("text/css");
    else if (lowerPath.endsWith(".gif")) dataType = F("image/gif");
    else if (lowerPath.endsWith(".jpg")) dataType = F("image/jpeg");
    else if (lowerPath.endsWith(".ico")) dataType = F("image/x-icon");
    else if (lowerPath.endsWith(".svg")) dataType = F("image/svg+xml");
    else if (lowerPath.endsWith(".mp3")) dataType = F("audio/mpeg");
    else if (lowerPath.endsWith(".wav")) dataType = F("audio/wav");
    else if (lowerPath.endsWith(".ogg")) dataType = F("audio/ogg");
    else if (lowerPath.endsWith(".xml")) dataType = F("text/xml");
    else if (lowerPath.endsWith(".pdf")) dataType = F("application/x-pdf");
    else if (lowerPath.endsWith(".zip")) dataType = F("application/x-zip");
    return dataType;
  }