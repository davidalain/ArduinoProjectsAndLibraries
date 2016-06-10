// Do not remove the include below
#include "Arduino.h"
#include "Ethernet28J60.h"

#define URL_YES	"davidalain=1"
#define URL_NO	"davidalain=0"

static uint8_t mac[6] = {0x54, 0x55, 0x58, 0x10, 0x00, 0x24};
static uint8_t ip[4] = {192, 168, 0, 199};
static uint16_t port = 80;

Ethernet28J60 ether;

void setup()
{
	Serial.begin(9600);

	Serial.println("Starting Ethernet...");
	ether.setup(mac, ip, port);

	Serial.println("Ethernet started!!");
}

void loop()
{
	char * request = ether.serviceRequest();
	if (request)
	{
		Serial.print("Request was ");
		Serial.println(request);

		if((strcmp(request, "")) ||( memcmp(request, "/", 1) == 0)){
			ether.print("<H1>Server is running</H1><br/>");
			ether.respond();
		}
		else if(memcmp(request, URL_YES, sizeof(URL_YES)-1) == 0){
			ether.print("<H1>David Alain says YES</H1><br/>");
			ether.respond();
		}
		else if(memcmp(request, URL_NO, sizeof(URL_NO)-1) == 0){
			ether.print("<H1>David Alain says NO</H1><br/>");
			ether.respond();
		}
		else{

			ether.print("<H1>Invalid URL request: ");
			ether.print(ip[0]); ether.print(".");
			ether.print(ip[1]); ether.print(".");
			ether.print(ip[2]); ether.print(".");
			ether.print(ip[3]); ether.print("/");
			ether.print(request);
			ether.print("</H1><br/>");
			ether.respond();
		}
	}
	delay(100);
}
