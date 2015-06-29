/*
 * UIPEthernet EchoServer example.
 *
 * UIPEthernet is a TCP/IP stack that can be used with a enc28j60 based
 * Ethernet-shield.
 *
 * UIPEthernet uses the fine uIP stack by Adam Dunkels <adam@sics.se>
 *
 *      -----------------
 *
 * This Hello World example sets up a server at 192.168.1.6 on port 1000.
 * Telnet here to access the service.  The uIP stack will also respond to
 * pings to test if you have successfully established a TCP connection to
 * the Arduino.
 *
 * This example was based upon uIP hello-world by Adam Dunkels <adam@sics.se>
 * Ported to the Arduino IDE by Adam Nielsen <malvineous@shikadi.net>
 * Adaption to Enc28J60 by Norbert Truchsess <norbert.truchsess@t-online.de>
 */

#include <UIPEthernet.h>

EthernetServer server = EthernetServer(1000);

void setup()
{
	Serial.begin(9600);

	uint8_t mac[6] = {0x00,0x01,0x02,0x03,0x04,0x05};
	IPAddress myIP(192,168,0,6);

	Serial.println("Iniciando...");

	Ethernet.begin(mac,myIP);

	server.begin();

	Serial.println("Rodando!!!");
}

void loop()
{
	static double v = 0.0;
	static int count = 0;

	EthernetClient client = server.available();
	if (client)
	{
		while(true){

			Serial.print("Printing data... count=");
			Serial.println(count);

			client.print(count++); client.print("; ");
			client.print(0.00 + v, 2); client.print("; ");
			client.print(1.23 + v, 2); client.print("; ");
			client.print(2.098 + v, 3); client.print("; ");
			client.print(3.83974 + v, 5); client.print("; ");
			client.print(1.23 + v, 10); client.print("; ");
			client.println(6.1276512765 + v, 10);
			client.println("\r\n");

			Serial.println("Done!");
			delay(1000);

			v += 0.2;
		}
	}else{
		Serial.print("NO client!!! count=");
		Serial.println(count++);
	}

	delay(1000);

}
