#ifndef definitions_h
#define definitions_h

// constants and macros as used for beam test in Oct12
#define FADC_MODULES	   2
#define MAX_FADC_DATA	   1024*16


#define FADC_MODE_HIT	    0x04000000	// zero supressed mode --> not used at beam test!
#define FADC_MODE_TRP_2K    0x05000000  //transparent mode --> all beam test data are recorded using this mode
#define FADC_MODE_MASK      0x07000000  // mask for mode camparison
#define FADC_CHK_MODE(dm,m)	((dm&FADC_MODE_MASK)==m)  // macro to check if mode m is set in dm


#define MAX_HYBRIDS     8               /* maximum number of apv25 hybrids */
#define MAX_APV         (4*MAX_HYBRIDS) /* maximum number of APVs (and ADC channels) */
#define ADC_SAMPLES     1024            /* but you can also use 950=adc_samples, since this was the value used for beam test*/

#define MAX_SUBEVENTS   6  		/* maximum number of subevents in one readout buffer */
#define APV_CHANNELS		128		/* number of channels (strips) per APV chip */

#define HS_LIVESCAN        50     /* number of samples for live ticks scan */
#define HS_STARTSEARCH	   50			/* sample where to start header search */
#define HS_STOPSEARCH	   7900			/* sample where to stop header search */
#define HS_MINTICKAMP      40     /* minimum live ticks amplitude for recognition */

#endif