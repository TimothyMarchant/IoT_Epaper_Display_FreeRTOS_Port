#ifndef ESP_Methods_h
#define ESP_Methods_h
_Bool IsExpectedMessage(const char* ExpectedResponse);
void ClearATResponse(void);
unsigned char GetConnectionStatus(void);
void disable_echo(void);
void StartConnection(void);
void TCPSendstart_UART(void);
void TCP_Close_Socket(void);
void GetImage(void);
#endif