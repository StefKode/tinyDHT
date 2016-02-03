#include <avr/io.h>
#include <util/delay.h>

#include "dbg_putchar.h"
#include "printf.h"
#include "dht11.h"
#include "vccmeasure.h"

#define TINUDHT_PIN PB3

void sleep(int millisec)
{
    while(millisec) {
            _delay_ms(1);/* 1 ms delay */
            millisec--;
    }
}

void serPutc(void *p, char c)
{
    dbg_putchar(c);
}

int main(int argc, char *argv[])
{
    TinuDHT tinudht;
    uint8_t ret;

    dbg_tx_init();
    init_printf((void*)0, serPutc);
    sleep(10);
    tfp_printf("DHT11 run\n\r");

    while(1) {
        ret = tinudht_read(&tinudht, TINUDHT_PIN);

        if (ret == TINUDHT_OK) {
            tfp_printf("T=%d H=%d BAT=%d BAT=%d%%\n\r", 
                    tinudht.temperature,
                    tinudht.humidity,
                    getVcc(),
                    getVccRating(50, 45));
        } else {
            tfp_printf("ret = %d   dbg = %d\n\r", ret, tinudht.dbg);
        }

        sleep(4000);
    }
    return 0;
}
