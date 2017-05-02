#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include "./RF24.h"

using namespace std;

// Hardware configuration
// Configure the appropriate pins for your connections

// Radio CE Pin, CSN Pin

//RPi with SPIDEV set 'this->device = "/dev/spidev0.0";;' or as listed in /dev
RF24 radio(22,0);


// Radio pipe addresses for the 2 nodes to communicate.
const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };

char receive_payload[32+1]; // +1 to allow room for a terminating NULL char

int main(int argc, char** argv){

  bool transmit = 1, recieve = 0;
  bool role = 0;
  cout << "\n\n*******nRf24L01+ communication with arduino********\n\n";

  // Setup and configure rf radio
  radio.begin();
  radio.enableDynamicPayloads();
  radio.setRetries(5,15);
  radio.printDetails();


/********* Role chooser ***********/

  printf("\n ************ Role Setup ***********\n");
  string input = "";
  char myChar = {0};
  cout << "Choose a role: Enter 0 for receiver, 1 for transmitter (CTRL+C to exit) \n>";
  getline(cin,input);

  if(input.length() == 1) {
	myChar = input[0];
	if(myChar == '0'){
		cout << "Role: recieve, awaiting transmission " << endl << endl;
	}else{  cout << "Role: transmit, starting transmission " << endl << endl;
		role = transmit;
	}
  }
/***********************************/

    if ( role == transmit )    {
      radio.openWritingPipe(pipes[0]);
      radio.openReadingPipe(1,pipes[1]);
    } else {
      radio.openWritingPipe(pipes[1]);
      radio.openReadingPipe(1,pipes[0]);
      radio.startListening();

    }


// forever loop
	while (1)
	{
	if (role == transmit)
	{
    // The payload will always be the same
    static char send_payload[] = "hello from rpi";

    // First, stop listening so we can talk.
    radio.stopListening();

    // Take the time, and send it.  This will block until complete
    printf("Now sending...\n");
    radio.write( send_payload, 32 );

    // Now, continue listening for acknowlegdement
    radio.startListening();
    

    // Wait here until we get a response, or timeout
    unsigned long started_waiting_at = millis();
    bool timeout = false;
    while ( ! radio.available() && ! timeout )
      if (millis() - started_waiting_at > 500 )
        timeout = true;

    // Describe the results
    if ( timeout )
    {
      printf("Failed, response timed out.\n\r");
    }
    else
    {
      // Grab the response, compare, and send to debugging spew
      uint8_t len = radio.getDynamicPayloadSize();
      radio.read( receive_payload, len );

      // Put a zero at the end for easy printing
      receive_payload[len] = 0;

      // Spew it
      printf("Got response size=%i value=%s\n\r",len,receive_payload);
    }


    // Try again 1s later
    delay(100);
  }

  //
  // recieve role.  Receive each packet, dump it out, and send it back
  //

  if ( role == recieve )
  {
    // if there is data ready
    if ( radio.available() )
    {
      // Dump the payloads until we've gotten everything
      uint8_t len;

      while (radio.available())
      {
        // Fetch the payload, and see if this was the last one.
	len = radio.getDynamicPayloadSize();
	radio.read( receive_payload, len );

	// Put a zero at the end for easy printing
	receive_payload[len] = 0;

	// Spew it
	printf("Got payload size=%i value=%s\n\r",len,receive_payload);
      }

      // First, stop listening so we can talk
      radio.stopListening();

      // Send the final one back.
      radio.write( receive_payload, len );
      printf("Sent response.\n\r");

      // Now, resume listening so we catch the next packets.
      radio.startListening();
    }
  }
}
}


