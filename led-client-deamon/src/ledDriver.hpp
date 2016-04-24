#ifndef __LED_DRIVER_H__
#define __LED_DRIVER_H__

#include <stdint.h>
#include <linux/types.h>
#include <string>

// Represent the data for a single pixel 
typedef struct pixelEntryStruct 
{
    // uint8_t flag;
  
    uint8_t green;
    uint8_t red;
    uint8_t blue;
} PIXEL_ENTRY_T;


class PixelBuffer
{
    private:
        int            ledCnt;
        size_t      bufLength;
        uint8_t  *bufPtr;

        static bool gammaLookupInitialized;
   
        static uint8_t  redGammaLookup[256];
        static uint8_t  greenGammaLookup[256];
        static uint8_t  blueGammaLookup[256];

        void writePixel( uint16_t pixelIndex, uint8_t red, uint8_t green, uint8_t blue );

    public:
         PixelBuffer( uint16_t ledCount );
      ~PixelBuffer();


};

class LEDDriver
{
    private:
        std::string spiPath;
        int             spifd;

        PixelBuffer  pixelData;

    public:
        LEDDriver( std::string spidev, uint16_t ledCount );
        ~LEDDriver();

        bool start();
        void stop();
    
};

#endif // __LED_DRIVER_H__

