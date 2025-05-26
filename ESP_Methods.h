#ifndef ESP_Methods_h
#define ESP_Methods_h
_Bool IsExpectedMessage(const char* ExpectedResponse);
void ClearATResponse(void);
void SetID(void);
unsigned char GetConnectionStatus(void);
void disable_echo(void);
unsigned char StartConnection(void);
void TCPSendstart_UART(const char* TCPSENDSTARTLEN);
void TCP_Close_Socket(void);
void GetImage(void);
#endif