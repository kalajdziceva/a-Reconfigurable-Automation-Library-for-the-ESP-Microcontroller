#include <Arduino.h>
#include <LittleFS.h>
#include <parse.h>
#ifdef ESP32
  #include <WiFi.h>
  #include <AsyncTCP.h>
#else
  #include <ESP8266WiFi.h>
  #include <ESPAsyncTCP.h>
#endif
#include <ESPAsyncWebServer.h>

#ifdef DEBUG_ESP_PORT
#define DEBUG_MSG(...) DEBUG_ESP_PORT.printf( __VA_ARGS__ )
#else
#define DEBUG_MSG(...)
#endif

extern "C" {
#include <peripherycommon.h>

#include <cpu.h>
#include <peripherygpio.h>
#include <peripherytimer.h>
}


struct CpuState CPU;
struct Line actualline;

char _actualFilename[200];
int _actualFileLineNumber;
FILE *_actualFile;

int GpioDataNumber=0;
struct GpioData gpiodata[10];

int TimerDataNumber=0;
struct TimerData timerdata[10];

int cycleStart = 0;

#define INPUTFILE "input.txt"

#define DEBUG 1
//#define DEBUG_GPIO 0

/* web part */

// REPLACE WITH YOUR NETWORK CREDENTIALS
const char* ssid = "ssid";
const char* password = "password";
const char* PARAM_INPUT_1 = "Code";
AsyncWebServer server(80);

// HTML web page to handle 3 input fields (input1, input2, input3)
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html><head>
  <title>Virtual Microcontroller interpreter</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  </head>
  <body>

  <form action="/get">
    code: <textarea name="Code" cols="60" rows="5" style="background-color:white";></textarea>
    </br>
    <input type="submit" value="Run">
  </form>
  
  </br>
  <form action="/getInstr">
    <input type="submit" value="Get instruction file">
  </form>

  <form action="/reload">
    <input type="submit" value="Reload">
  </form>

  <br>
</body>
</html>)rawliteral";

void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

void setup() {
  // put your setup code here, to run once:
    //i need here serial.begin
    //
    Serial.begin(115200);
    DEBUG_MSG("setup\r\n");
    initCPU(&CPU, INPUTFILE);

    /* web part */
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("WiFi Failed!");
    return;
  }
  Serial.println();
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Send web page with input fields to client
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html);
  });

   server.on("/getInstr", HTTP_GET, [] (AsyncWebServerRequest *request) {
     char instructionFile[1000] = R"rawliteral(
       <textarea name="Code" cols="80" rows="100" readonly>
)rawliteral";

     strncat(instructionFile, readFilex(CPU.filename), 800);
     strncat(instructionFile,"</textarea>", 15);
     strncat(instructionFile, "<br><a href=\"/\">Return to Home Page</a>", 30);

     request->send_P(200, "text/html", instructionFile);
    });

  server.on("/reload", HTTP_GET, [](AsyncWebServerRequest *request){
     initCPU(&CPU, INPUTFILE);
    request->send_P(200, "text/html", "Instruction file reloaded sucessfully <br><a href=\"/\">Return to Home Page</a>");
  });

  // Send a GET request to <ESP_IP>/get?input1=<inputMessage>
  server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String inputMessage;
    String inputParam;
    // GET input1 value on <ESP_IP>/get?input1=<inputMessage>
    if (request->hasParam(PARAM_INPUT_1)) {
      inputMessage = request->getParam(PARAM_INPUT_1)->value();
      inputParam = PARAM_INPUT_1;
    }
    else {
      inputMessage = "No message sent";
      inputParam = "none";
    }
    Serial.println("Received:" + inputMessage);
    
    /*
    request->send(200, "text/html", "HTTP GET request sent to your ESP on input field (" 
                                     + inputParam + ") with value: " + inputMessage +
                                     "<br><a href=\"/\">Return to Home Page</a>");
    */


   char command[512];
   strncpy(command, inputMessage.c_str(), 512);
   //printf("raw string:%s", command);


  char *token = strtok(command, "\r\n");

   while (token != NULL) {
      printf("split token: %s\n", token);

      appendLine(INPUTFILE, token);

      token = strtok(NULL, "\r\n");
  }


    //appendLine(INPUTFILE, command);
  ///  initCPU(&CPU, INPUTFILE);

  //  printFile(INPUTFILE);
    request->send(200, "text/html", "Command"  + inputParam + " submitted to input file successfully. Check the terminal log for results."
                                     + "<br><a href=\"/\">Return to Home Page</a>");

  });
  server.onNotFound(notFound);
  server.begin();
}

void loop() {

    //printf("loop called\n");

   // load the next line (first if you reached the end)
    // increment the instruction pointer (number)
    // find the instruction
    // execute the instruction

    //later handle peripheries
    //DEBUG_MSG("loop\r\n");

   //printFile(INPUTFILE);
    struct Line line;

    bool br = readLinex(CPU.filename, CPU.nextinstructionlinenumber, &line);

    if (br == false) {
        //printPeripheryState(&CPU);
        //printPeripheryState(&CPU);
        //printf("------------------\n");

        //printf("CPU halted: finished reading instruction file\n");
        //initCPU(&CPU, INPUTFILE);

        //printf("CPU halted: reading file again\n");
        //initCPU(&CPU, INPUTFILE);
       // getchar();


            for (int i = 0; i < CPU.peripherylistnumber; i++) {
                #ifdef DEBUG_GPIO
                printf("calling gpio loop %s\n", CPU.instructionlist[getIndexByName(&CPU, line.instruction)].name);
                #endif
                CPU.peripherylist[i].pType->loop(&CPU, &CPU.peripherylist[i], cycleStart); 
            }
            cycleStart = !cycleStart;
             //printPeripheryState(&CPU);
     }
    else {

   


    CPU.nextinstructionlinenumber++;

    int index = getIndexByName(&CPU, line.instruction);

    if (index != -1) {

        CPU.instructionlist[getIndexByName(&CPU, line.instruction)].function(&CPU, &line);
        //printLine(&line);

    }
 }


    //printCpuState(&CPU);
    //gpio_set(&CPU, CPU.peripherylist, &line);
}

// int main() {

//       return 0;
// }