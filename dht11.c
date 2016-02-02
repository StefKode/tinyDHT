/*
 * The Tinusaur Project
 *
 * TinuDHT - Tinusaur DHT11 Library
 *
 * @created: 2014-07-08
 * @author: Neven Boyanov
 *
 * Source code available at: https://bitbucket.org/tinusaur/tinudht
 *
 */

// ----------------------------------------------------------------------------

#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>

#include "dht11.h"

// ----------------------------------------------------------------------------

#define TINUDHT_RCV_TIMEOUT 255
#define TINUDHT_RCV_DELAY  5 
#define TINUDHT_RCV_LENGTH 5

// ----------------------------------------------------------------------------

uint8_t tinudht_read(TinuDHT *ptinudht, uint8_t dht_pin)
{
	uint8_t data[5];
 
	// Empty the buffer
	for (uint8_t i=0; i< 5; i++) data[i] = 0;	// Another way to empty the data buffer.

	// Send request
	DDRB |= (1 << dht_pin);	// Set port as output
	PORTB &= ~(1 << dht_pin);	// Set to 0
	_delay_ms(18);	// Wait 18 ms
	PORTB |= (1 << dht_pin);	// Set to 1
	_delay_us(40);	// Wait 40 us
 
	// Receive response
	DDRB &= ~(1 << dht_pin);	// Set port as input
//		PORTB |= (1 << dht_pin);	// Set to 1, optional pull-up.

	uint8_t timeout;

    ptinudht->dbg = 0;
	// Acknowledge
	timeout = TINUDHT_RCV_TIMEOUT;
	while(bit_is_clear(PINB, dht_pin))	// Wait for 1
		if (timeout-- == 0)
			return TINUDHT_ERROR_TIMEOUT;
	
    //ptinudht->dbg++;
	timeout = TINUDHT_RCV_TIMEOUT;
	while(bit_is_set(PINB, dht_pin))	// Wait for 0
		if (timeout-- == 0)
			return TINUDHT_ERROR_TIMEOUT;

	uint8_t bit_index = 7;
	uint8_t byte_index = 0;
	// READ OUTPUT - 40 BITS => 5 BYTES or TIMEOUT
	for (uint8_t i = 0; i < 40; i++)
	{
        ptinudht->dbg++;

		// Wait for start
		timeout = TINUDHT_RCV_TIMEOUT;
		while(bit_is_clear(PINB, dht_pin))	// Wait for 1
			if (timeout-- == 0)
				return TINUDHT_ERROR_TIMEOUT;

        ptinudht->dbg++;

		// Determine the bit value
		uint8_t len = 0;
		while(bit_is_set(PINB, dht_pin)) {	// Wait for 0
			_delay_us(TINUDHT_RCV_DELAY);
			if (len++ == TINUDHT_RCV_TIMEOUT)
				return TINUDHT_ERROR_TIMEOUT;
			}

		if (len >= TINUDHT_RCV_LENGTH) data[byte_index] |= (1 << bit_index);

		// Experiments:
		// delay =    0,     len = 3..6 - 7:unstable
		// delay =  1us,     len = 3..5
		// delay =  2us,     len = 3..5
		// delay =  5us,     len = 3..5
		// delay = 10us,     len = 2..4
		// delay = 15us,     len = 2..3
		// delay = 20us,     len = 2    - 3:unstable
		// delay =  9..29us, len = 2
		
		// NOTE: The values of TINUDHT_RCV_DELAY and TINUDHT_RCV_LENGTH
		//       may need to be adjusted if the F_CPU changes.
		
		if (bit_index == 0)	// next byte?
		{
			bit_index = 7;	// restart at MSB
			byte_index++;	// next byte!
		}
		else bit_index--;

        //ptinudht->dbg -= 2;
	}

    ptinudht->dbg++;
	// DEBUGGING
	// DEBUGGING_STRING("DH:");
	// DEBUGGING_NUMDECP(data[0]);
	// DEBUGGING_NUMDECP(data[1]);
	// DEBUGGING_STRING("\n\r");
	// DEBUGGING_STRING("DT:");
	// DEBUGGING_NUMDECP(data[2]);
	// DEBUGGING_NUMDECP(data[3]);
	// DEBUGGING_STRING("\n\r");

	uint8_t checksum = data[0] + data[2];	// TODO: Fix this, does not work in border cases.
	if (data[4] != checksum) return TINUDHT_ERROR_CHECKSUM;

    ptinudht->dbg++;
	// DEBUGGING
	// DEBUGGING_STRING("CS:");
	// DEBUGGING_NUMDECP(data[4]);
	// DEBUGGING_NUMDECP(checksum);
	// DEBUGGING_STRING("\n\r");

	// On DHT11 data[1],data[3] are always zero so not used.
	ptinudht->humidity = data[0] + 5; 
	ptinudht->temperature = data[2] + 2; 

	return TINUDHT_OK;
}
