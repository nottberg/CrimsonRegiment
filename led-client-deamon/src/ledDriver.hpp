#ifndef __LED_DRIVER_H__
#define __LED_DRIVER_H__

#include <stdint.h>
#include <linux/types.h>
#include <string>

#include "ClientEvents.hpp"

// Represent the data for a single pixel 
typedef struct pixelEntryStruct 
{
    //uint8_t flag;
  
    uint8_t blue;
    uint8_t red;
    uint8_t green;
} PIXEL_ENTRY_T;


class PixelBuffer
{
    private:
        int      ledCnt;

        size_t   bufLength;
        uint8_t *bufPtr;

        bool gammaLookupInitialized;
   
        uint8_t  redGammaLookup[256];
        uint8_t  greenGammaLookup[256];
        uint8_t  blueGammaLookup[256];

        void writePixel( uint16_t pixelIndex, uint8_t red, uint8_t green, uint8_t blue );
        void writeGammaPixel( uint16_t pixelIndex, uint8_t red, uint8_t green, uint8_t blue );

    public:
        PixelBuffer( uint16_t ledCount );
       ~PixelBuffer();

        void setGammaCorrection( double red, double green, double blue );

        void clearAllPixels();
        void clearPixel( uint16_t pixelIndex );

        void setPixel( uint16_t pixelIndex, uint8_t red, uint8_t green, uint8_t blue );

        void getUpdateBuffer( uint8_t **buf, size_t &length ); 
};

class LEDDriver : public EventNotify
{
    private:
        std::string spiPath;
        int         spifd;
        
        PixelBuffer pixelData;

//        SoftEventSource updateEvent;

        bool pendingUpdate;

    public:
        LEDDriver( std::string spidev, uint16_t ledCount );
        ~LEDDriver();

        bool start( EventLoop &loop );
        void stop();
    
        void setGammaCorrection( double red, double green, double blue );

        void clearAllPixels();
        void clearPixel( uint16_t pixelIndex );

        void setPixel( uint16_t pixelIndex, uint8_t red, uint8_t green, uint8_t blue );

        // Signal that an update is pending
        void signalUpdate();

        // Handle events
        virtual void eventAction( uint32_t EventID );

        // Perform updates to hardware, if needed.
        void processUpdates();
};

#endif // __LED_DRIVER_H__

