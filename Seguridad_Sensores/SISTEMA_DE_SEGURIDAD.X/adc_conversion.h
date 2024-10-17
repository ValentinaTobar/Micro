/* 
 * File:   adc_conversion.h
 * Author: valen
 *
 * Created on 17 de octubre de 2024, 09:11 AM
 */

#ifndef ADC_CONVERSION_H
#define	ADC_CONVERSION_H

#ifdef	__cplusplus
extern "C" {
#endif

void adc_init(void);
unsigned int adc_read(int channel);


#ifdef	__cplusplus
}
#endif

#endif	/* ADC_CONVERSION_H */
