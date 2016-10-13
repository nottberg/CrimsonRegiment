#ifndef __LED_DRIVER_H__
#define __LED_DRIVER_H__

#include <stdint.h>
#include <linux/types.h>
#include <string>

#include "ClientEvents.hpp"

typedef enum LEDDriverProtocolTypeEnum
{
    LD_PROTOCOL_NOT_SET,
    LD_PROTOCOL_LPD8806,
    LD_PROTOCOL_APA102
}LD_PROTOCOL_T;


class PixelBuffer
{
    private:
        uint32_t ledCnt;

    protected:

        size_t   bufLength;
        uint8_t *bufPtr;

        bool gammaLookupInitialized;
   
        uint8_t  redGammaLookup[256];
        uint8_t  greenGammaLookup[256];
        uint8_t  blueGammaLookup[256];

        virtual void writePixel( uint16_t pixelIndex, uint8_t red, uint8_t green, uint8_t blue ) = 0;
        virtual void writeGammaPixel( uint16_t pixelIndex, uint8_t red, uint8_t green, uint8_t blue ) = 0;

        virtual void readPixel( uint16_t pixelIndex, uint8_t &red, uint8_t &green, uint8_t &blue ) = 0;

    public:
        PixelBuffer();
       ~PixelBuffer();

        virtual void initialize( uint16_t ledCount );

        void setGammaCorrection( double red, double green, double blue );

        uint32_t getPixelCount();

        void clearAllPixels();
        void clearPixel( uint16_t pixelIndex );

        void setPixel( uint16_t pixelIndex, uint8_t red, uint8_t green, uint8_t blue );

        void getPixel( uint16_t pixelIndex, uint8_t &red, uint8_t &green, uint8_t &blue );

        void getUpdateBuffer( uint8_t **buf, size_t &length ); 

        
};

// Represent the data for a single pixel 
typedef struct pixelEntryStructLPD8806 
{
    //uint8_t flag;
  
    uint8_t blue;
    uint8_t red;
    uint8_t green;
} PIXEL_ENTRY_LPD8806_T;

class PixelBufferLPD8806 : public PixelBuffer
{
    private:
        virtual void writePixel( uint16_t pixelIndex, uint8_t red, uint8_t green, uint8_t blue );
        virtual void writeGammaPixel( uint16_t pixelIndex, uint8_t red, uint8_t green, uint8_t blue );

        virtual void readPixel( uint16_t pixelIndex, uint8_t &red, uint8_t &green, uint8_t &blue );

    public:
        PixelBufferLPD8806();
       ~PixelBufferLPD8806();

        virtual void initialize( uint16_t ledCount );
};

// Represent the data for a single pixel 
typedef struct pixelEntryStructAPA102 
{
    uint8_t global; 
    uint8_t blue;
    uint8_t green;
    uint8_t red;
} PIXEL_ENTRY_APA102_T;

class PixelBufferAPA102 : public PixelBuffer
{
    private:
        virtual void writePixel( uint16_t pixelIndex, uint8_t red, uint8_t green, uint8_t blue );
        virtual void writeGammaPixel( uint16_t pixelIndex, uint8_t red, uint8_t green, uint8_t blue );

        virtual void readPixel( uint16_t pixelIndex, uint8_t &red, uint8_t &green, uint8_t &blue );

    public:
        PixelBufferAPA102();
       ~PixelBufferAPA102();

        virtual void initialize( uint16_t ledCount );
};

class LEDDriver : public EventNotify
{
    private:
        std::string spiPath;
        int         spifd;
        
        PixelBuffer *pixelData;

//        SoftEventSource updateEvent;

        bool pendingUpdate;

    public:
        LEDDriver();
        ~LEDDriver();

        void initialize( LD_PROTOCOL_T protocol, std::string spidev, uint16_t stringSize );

        bool start( EventLoop &loop );
        void stop();
    
        void setGammaCorrection( double red, double green, double blue );

        uint32_t getPixelCount();

        void clearAllPixels();
        void clearPixel( uint16_t pixelIndex );

        void setPixel( uint16_t pixelIndex, uint8_t red, uint8_t green, uint8_t blue );

        void getPixel( uint16_t pixelIndex, uint8_t &red, uint8_t &green, uint8_t &blue );

        // Test and Initialize the connected leds.
        void testSequence();

        // Signal that an update is pending
        void signalUpdate();

        // Handle events
        virtual void eventAction( uint32_t EventID );

        // Perform updates to hardware, if needed.
        void processUpdates();
};

#endif // __LED_DRIVER_H__

