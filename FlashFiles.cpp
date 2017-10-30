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



void sendFile(String path, WiFiClient * client)
{
    // get content type
    if (path.endsWith("/"))
    {
        path += "index.html";
    }
    String dataType = getContentType(path);

    // check if theres a .gz'd version and send that instead
    String gzPath = path + ".gz";
    File theBuffer;
    if (SPIFFS.exists(gzPath))
    { // test to see if there is a .gz version of the file
        theBuffer = SPIFFS.open(gzPath, "r");
        path = gzPath; // got it, use this path
    }
    else
    { // not here so load the standard file
        theBuffer = SPIFFS.open(path, "r");
        if (!theBuffer)
        { // this one doesn't exist either, abort.
            //Serial.println(path + "Does Not Exist");
            theBuffer.close();
            String notFound = F("HTTP/1.1 404 Not Found\r\nConnection: Close\r\ncontent-length: 0\r\n\r\n");
            client->write(notFound.c_str(), notFound.length());
            //client.write( closeConnectionHeader.c_str(),closeConnectionHeader.length() );
            yield();
            return; // failed to read file
        }
    }
    // make header
    String s = F("HTTP/1.1 200 OK\r\ncache-control: max-age = 3600\r\ncontent-length: ");
    s += theBuffer.size();
    s += F("\r\ncontent-type: ");
    s += dataType;
    s += F("\r\nconnection: close"); // last one added X-Content-Type-Options: nosniff\r\n
    if (path.endsWith(".gz"))
    {
        s += F("\r\nContent-Encoding: gzip\r\n\r\n");
    }
    else
    {
        s += F("\r\n\r\n");
    }
    // send the file
    if (! client->write(s.c_str(), s.length()))
    {
        // failed to send
        theBuffer.close();
        return;
    }
    uint bufferLength = theBuffer.size();
    if ( client->write(theBuffer, 2920) < bufferLength)
    {
        // failed to send all file
    }
    yield();
    theBuffer.close();
}


String getContentType(String path) { // get content type
    String dataType = F("text/html");
    String lowerPath = path.substring(path.length() - 4, path.length());
    lowerPath.toLowerCase();
  
    if (lowerPath.endsWith(".src"))
      lowerPath = lowerPath.substring(0, path.lastIndexOf("."));
    else if (lowerPath.endsWith(".gz"))
      dataType = F("application/x-gzip");
    else if (lowerPath.endsWith(".html"))
      dataType = F("text/html");
    else if (lowerPath.endsWith(".htm"))
      dataType = F("text/html");
    else if (lowerPath.endsWith(".png"))
      dataType = F("image/png");
    else if (lowerPath.endsWith(".js"))
      dataType = F("application/javascript");
    else if (lowerPath.endsWith(".css"))
      dataType = F("text/css");
    else if (lowerPath.endsWith(".gif"))
      dataType = F("image/gif");
    else if (lowerPath.endsWith(".jpg"))
      dataType = F("image/jpeg");
    else if (lowerPath.endsWith(".ico"))
      dataType = F("image/x-icon");
    else if (lowerPath.endsWith(".svg"))
      dataType = F("image/svg+xml");
    else if (lowerPath.endsWith(".mp3"))
      dataType = F("audio/mpeg");
    else if (lowerPath.endsWith(".wav"))
      dataType = F("audio/wav");
    else if (lowerPath.endsWith(".ogg"))
      dataType = F("audio/ogg");
    else if (lowerPath.endsWith(".xml"))
      dataType = F("text/xml");
    else if (lowerPath.endsWith(".pdf"))
      dataType = F("application/x-pdf");
    else if (lowerPath.endsWith(".zip"))
      dataType = F("application/x-zip");
  
    return dataType;
  }
