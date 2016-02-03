#include <avr/io.h>
#include <util/delay.h>
#include "vccmeasure.h"

/*
  uint16_t MeasureVCC(void)
  
  Measures the supply voltage of an ATtiny85 internall using the ADC.
  
  Returns the ADC-value of the 1.1 band gap input measured with VCC as reference voltage.
  This value can be converted to the VCC voltage by using this equation:
  
  VCC=1024*1.1/ADC;
  
  cpldcpu@gmail.com - Nov 23, 2013
  
  s-koch@gmx.net - Feb 3, 206
  - Added VCC Rating function to return battery percentage

*/

uint16_t getVccADCRaw(void)
{
    PRR    &=~_BV(PRADC);                 // ADC power on
    ADCSRA  =_BV(ADEN)|_BV(ADPS2)|_BV(ADPS1)|_BV(ADPS0); // Enable ADC, Set prescaler to 128
    ADMUX   =_BV(REFS2) | 0x0c; // Use VCC as voltage reference, select bandgap reference as ADC input
    _delay_ms(1);  // Settling time, see p.134 in ATtiny Datasheet
    
    ADCSRA  |=_BV(ADSC); // Start conversion
    while (!(ADCSRA&_BV(ADIF)));  //~100 us
    ADCSRA  |=_BV(ADIF); // Clear ADIF
    return ADC;
}

/* returns VCC*10 */
uint8_t getVcc()
{
    uint16_t v;

    //double costs  600kB */
    //v = (10.0f * 1024.0f * 1.1f) / (double)getVccADCRaw();
    v = (11 * 1024) / getVccADCRaw();

    return (uint8_t) (0xff & v);
}

/* returns vcc rating all input values *10, returns percentage */
uint8_t getVccRating(uint8_t nominal, uint8_t undervoltage)
{
    uint16_t vcc, uv, rating, nom, range;

    nom   = nominal;
    uv    = undervoltage;
    range = nom - uv;
    vcc   = getVcc();
    
    if (vcc >= nom) return 100;
    if (vcc <=  uv) return 0;

    rating = (100 * (vcc - uv)) / range;

    return (uint8_t) rating;
}
