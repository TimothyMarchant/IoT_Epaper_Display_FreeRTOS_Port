//ESP-01S and Server command strings.  Made into a header to save space on other files.
#ifndef ESP_Command_Strings
#define ESP_Command_Strings
#ifndef ATstrings
//nothing should change for this
#define ATString "AT\r\n"
//disable echo
#define ATE0 "ATE0\r\n"
//sleep for 1 second default sleep string will be used later
#define ESPSLEEP "AT+GSLP=1000\r\n"
//These strings would be needed to connect to a network, but this will be done off this microcontroller for privacy (don't want to expose my network)
//default WiFi mode; set to station mode
//const char* const ATCWMODE_DEF="AT+CWMODE_DEF=1";
//connect to this network with the correct SSID and password; for reference only
//const char* const ATCWJAP_DEF="AT+CWJAP_DEF=\"NAME\",\"PASSWORD\"";
//get current status
#define ATCIPSTATUS "AT+CIPSTATUS\r\n"
//start the TCP connection
#define TCPSTART "AT+CIPSTART=\"TCP\",\"IP\",7777\r\n" //7777 is not special or anything it's just the port number I arbitarly chose.
#define TCPSENDSTART "AT+CIPSEND=1\r\n" //send this with the length of the message.
#define CLOSETCPSOCKET "AT+CIPCLOSE\r\n" //make sure to close the socket
#define dummy "a"


//expected response from AT\r\n
#define ATErrorResponse "\r\nERROR\r\n"
#define ATTestResponse "\r\nOK\r\n"
#define ATCloseResponse "\r\nCLOSED\r\n\r\nOK\r\n"
//only the beginning of the response is needed.  The other info is just server information we don't need.
//meant for a quick comparison.
#define ATStatusString "\r\nSTATUS:"
#define ATConnectedToServerResponse "\r\nSTATUS:3\r\n"
#define ATConnectedToWiFi "\r\nSTATUS:2\r\n"
#define ATConnectedstrlen strlen(ATConnectedToServerResponse)
//return obvious failure value that isn't used.
#define FailedToGetStatus 0xFF
#endif
//commands for sending to TCP server program.
#ifndef ServerCommands
#define ServerCommands
//just need to send a single byte of data to acknowledge for the next segment.
#define ACK 'a'
#define ACKSENDSTART "AT+CIPSEND=1\r\n"
#define GetImage_string "GetImage"
#define GetImageSENDSTART "AT+CIPSEND=8\r\n"
#define GetID_string "GetID"
#define GetIDSENDSTART "AT+CIPSEND=5\r\n"
#define SetID_string "SetID"
#define SetIDSENDSTART "AT+CIPSEND=5\r\n"
#define Quit_string "quit"
#define QuitSENDSTART "AT+CIPSEND=4\r\n"
#endif
#ifndef ServerResponses
#define ServerResponses
#define RECVxBytesStringResponse "\r\nRecv x bytes\r\n\r\nSEND OK\r\n\r\n"
#define RECVxBytesLENGTH strlen(RECVxBytesStringResponse)
//response length for getting the ID.
#define IDRESPONSELENGTH strlen("+IPD,4:xxxx")
#define IDFOUND "+IPD,7:IDFOUND"
#define IDNOTFOUND "+IPD,10:IDNOTFOUND"
#define ImageIPDSTRINGlength strlen("+IPD,1000:")
#define QuitResponselength strlen("+IPD,1:c") //c is actually just a place holder.  The response is 1.
#define FAILUREResponselength strlen("+IPD,1:f") //f is actually just a place holder.  The respone is 0xFF.
#define FAILURE 0xFF
#endif
#endif