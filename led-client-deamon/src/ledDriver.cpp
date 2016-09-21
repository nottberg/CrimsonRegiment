#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include <errno.h>
#include <math.h>
#include <stdlib.h>
#include <strings.h>

#include <iostream>

#include "ledDriver.hpp"

PixelBuffer::PixelBuffer()
{
    ledCnt = 0;
    gammaLookupInitialized = false;
}

PixelBuffer::~PixelBuffer()
{
    if( bufPtr )
    {
        free( bufPtr );
    }
}

void
PixelBuffer::initialize( uint16_t ledCount )
{
    printf( "PixelBuffer::intialize: %d\n", ledCount );

    // Remember the number of leds in the string
    ledCnt = ledCount;

    // Start up with no gamma correction
    gammaLookupInitialized = false;
}

#if 0
void 
PixelBuffer::writePixel( uint16_t pixelIndex, uint8_t red, uint8_t green, uint8_t blue ) 
{
    PIXEL_ENTRY_T *p = (PIXEL_ENTRY_T *)( bufPtr + ( ( pixelIndex + 3 ) * sizeof( PIXEL_ENTRY_T )  ) );

    if( pixelIndex >= ledCnt )
        return;

    p->blue  = 0x80 | ( blue & 0x7F );
    p->green = 0x80 | ( green & 0x7F );
    p->red   = 0x80 | ( red & 0x7F );

//    printf ("index : %i 0x%x %x %x %x\n", pixelIndex, p, p->red  , p->green, p->blue);
}

void 
PixelBuffer::writeGammaPixel( uint16_t pixelIndex, uint8_t red, uint8_t green, uint8_t blue ) 
{
    PIXEL_ENTRY_T *p = (PIXEL_ENTRY_T *)( bufPtr + ( ( pixelIndex + 3 ) * sizeof( PIXEL_ENTRY_T )  ) );

    if( pixelIndex >= ledCnt )
        return;

    p->blue  = blueGammaLookup[ blue ];
    p->green = greenGammaLookup[ green ];
    p->red   = redGammaLookup[ red ];
    // printf ("index : %i %i %i %i\n",p, p->red  , p->green, p->blue);
}
#endif

void
PixelBuffer::setGammaCorrection( double red, double green, double blue )
{
    for( int i=0; i < sizeof( redGammaLookup ); i++ ) 
    {
        redGammaLookup[i]   = ( 0x80 | (int)( pow( ( (float) i / 255.0 ), red ) * 127.0 + 0.5 ) );
        greenGammaLookup[i] = ( 0x80 | (int)( pow( ( (float) i / 255.0 ), green ) * 127.0 + 0.5 ) );
        blueGammaLookup[i]  = ( 0x80 | (int)( pow( ( (float) i / 255.0 ), blue ) * 127.0 + 0.5 ) );
    }

    gammaLookupInitialized = true;
}

uint32_t 
PixelBuffer::getPixelCount()
{
    return ledCnt;
}

void
PixelBuffer::clearAllPixels()
{
    std::cout << "PixelBuffer::clearAllPixels: " << ledCnt << std::endl;

    // Initialize all of the pixels to black
    for( uint32_t i = 0; i < ledCnt; i++ )
    {
        writePixel( i, 0x00, 0x00, 0x00 );
    }
}

void
PixelBuffer::clearPixel( uint16_t pixelIndex )
{
    writePixel( pixelIndex, 0, 0, 0 );
}

void
PixelBuffer::setPixel( uint16_t pixelIndex, uint8_t red, uint8_t green, uint8_t blue )
{
    if( gammaLookupInitialized == true )
    {
        writeGammaPixel( pixelIndex, red, green, blue );
    }
    else
    {
        writePixel( pixelIndex, red, green, blue );
    }
}

void 
PixelBuffer::getPixel( uint16_t pixelIndex, uint8_t &red, uint8_t &green, uint8_t &blue )
{
    readPixel( pixelIndex, red, green, blue );
}

void 
PixelBuffer::getUpdateBuffer( uint8_t **buf, size_t &length )
{
    *buf   = bufPtr;
    length = bufLength;
}

PixelBufferLPD8806::PixelBufferLPD8806()
{

}

PixelBufferLPD8806::~PixelBufferLPD8806()
{

}

void
PixelBufferLPD8806::initialize( uint16_t ledCount )
{
    printf( "PixelBufferLPD8806::intialize: %d\n", ledCount );

    // Remember the number of leds in the string
    PixelBuffer::initialize( ledCount );

    // Allocate a buffer to hold the led info.
    bufLength = ( getPixelCount() + 3 ) * sizeof( PIXEL_ENTRY_LPD8806_T );
    bufPtr = (uint8_t *) malloc( bufLength );
  
    if( bufPtr == NULL ) 
    {
        bufLength = 0;
        return;
    }

    // Clear the buffer to zeroes.
    bzero( bufPtr, bufLength );

    // Initialize all of the pixels to black
    for( uint32_t i = 0; i < ledCount; i++ )
    {
        writePixel( i, 0x00, 0x00, 0x00 );
    }
}

void 
PixelBufferLPD8806::writePixel( uint16_t pixelIndex, uint8_t red, uint8_t green, uint8_t blue ) 
{
    PIXEL_ENTRY_LPD8806_T *p = (PIXEL_ENTRY_LPD8806_T *)( bufPtr + ( ( pixelIndex + 3 ) * sizeof( PIXEL_ENTRY_LPD8806_T )  ) );

    if( pixelIndex >= getPixelCount() )
        return;

    p->blue  = 0x80 | ( blue & 0x7F );
    p->green = 0x80 | ( green & 0x7F );
    p->red   = 0x80 | ( red & 0x7F );

//    printf ("index : %i 0x%x %x %x %x\n", pixelIndex, p, p->red  , p->green, p->blue);
}

void 
PixelBufferLPD8806::writeGammaPixel( uint16_t pixelIndex, uint8_t red, uint8_t green, uint8_t blue ) 
{
    PIXEL_ENTRY_LPD8806_T *p = (PIXEL_ENTRY_LPD8806_T *)( bufPtr + ( ( pixelIndex + 3 ) * sizeof( PIXEL_ENTRY_LPD8806_T )  ) );

    if( pixelIndex >= getPixelCount() )
        return;

    p->blue  = blueGammaLookup[ blue ];
    p->green = greenGammaLookup[ green ];
    p->red   = redGammaLookup[ red ];
    // printf ("index : %i %i %i %i\n",p, p->red  , p->green, p->blue);
}

void 
PixelBufferLPD8806::readPixel( uint16_t pixelIndex, uint8_t &red, uint8_t &green, uint8_t &blue ) 
{
    PIXEL_ENTRY_LPD8806_T *p = (PIXEL_ENTRY_LPD8806_T *)( bufPtr + ( ( pixelIndex + 3 ) * sizeof( PIXEL_ENTRY_LPD8806_T )  ) );

    if( pixelIndex >= getPixelCount() )
        return;

    blue  = p->blue;
    green = p->green;
    red   = p->red;

//    printf ("index : %i 0x%x %x %x %x\n", pixelIndex, p, p->red  , p->green, p->blue);
}


PixelBufferAPA102::PixelBufferAPA102()
{

}

PixelBufferAPA102::~PixelBufferAPA102()
{

}

void
PixelBufferAPA102::initialize( uint16_t ledCount )
{
    printf( "PixelBufferAPA102::intialize: %d\n", ledCount );

    // Remember the number of leds in the string
    PixelBuffer::initialize( ledCount );

    // For APA-102 we need one 32-bit start word,
    // followed by 32-bits per pixel,
    // followed by at least (ledCnt/2) bits of 1s to
    // generate the forwarding clocks to end of strip.
    bufLength  = 4;
    bufLength += ( 4 * getPixelCount() );

    uint32_t trailCnt = (( getPixelCount() / 2 ) / 8 ) + 1; // Add a 1 byte pad to round up.
    bufLength += trailCnt; 

    bufPtr = (uint8_t *) malloc( bufLength );

    if( bufPtr == NULL ) 
    {
        bufLength = 0;
        return;
    }

    // Clear the buffer to zeroes.
    bzero( bufPtr, bufLength );

    // Write the ones at the end
    uint8_t *trailPtr = &bufPtr[ 4+(4*getPixelCount()) ]; 
    for( uint32_t i = 0; i < trailCnt; i++ )
    {
        *trailPtr = 0xFF;
        trailPtr++;
    }

    // Initialize all of the pixels to black
    for( uint32_t i = 0; i < ledCount; i++ )
    {
        writePixel( i, 0x00, 0x00, 0x00 );
    }
}

void 
PixelBufferAPA102::writePixel( uint16_t pixelIndex, uint8_t red, uint8_t green, uint8_t blue ) 
{
    PIXEL_ENTRY_APA102_T *p = (PIXEL_ENTRY_APA102_T *)( bufPtr + ( 4 + ( 4 * pixelIndex ) ) );

    //printf( "apa102 - writePixel: %d  %d\n", pixelIndex, getPixelCount() );

    if( pixelIndex >= getPixelCount() )
        return;

    //printf( "apa102 - writePixel: 0x%x  0x%x\n", bufPtr, p );

    p->global = 0xFF;
    p->blue   = blue;
    p->green  = green;
    p->red    = red;

//    printf ("index : %i 0x%x %x %x %x\n", pixelIndex, p, p->red  , p->green, p->blue);
}

void 
PixelBufferAPA102::writeGammaPixel( uint16_t pixelIndex, uint8_t red, uint8_t green, uint8_t blue ) 
{
    PIXEL_ENTRY_APA102_T *p = (PIXEL_ENTRY_APA102_T *)( bufPtr + ( 4 + ( 4 * pixelIndex ) ) );

    if( pixelIndex >= getPixelCount() )
        return;

    p->global = 0xFF;
    p->blue   = blueGammaLookup[ blue ];
    p->green  = greenGammaLookup[ green ];
    p->red    = redGammaLookup[ red ];

//    printf ("index : %i 0x%x %x %x %x\n", pixelIndex, p, p->red  , p->green, p->blue);
}

void 
PixelBufferAPA102::readPixel( uint16_t pixelIndex, uint8_t &red, uint8_t &green, uint8_t &blue ) 
{
    PIXEL_ENTRY_APA102_T *p = (PIXEL_ENTRY_APA102_T *)( bufPtr + ( 4 + ( 4 * pixelIndex ) ) );

    //printf( "apa102 - writePixel: %d  %d\n", pixelIndex, getPixelCount() );

    if( pixelIndex >= getPixelCount() )
        return;

    //printf( "apa102 - writePixel: 0x%x  0x%x\n", bufPtr, p );

    blue  = p->blue;
    green = p->green;
    red   = p->red;

//    printf ("index : %i 0x%x %x %x %x\n", pixelIndex, p, p->red  , p->green, p->blue);
}


LEDDriver::LEDDriver()
{
    spifd   = (-1);
    pixelData = NULL;
    pendingUpdate = false;
}

LEDDriver::~LEDDriver()
{

}

void
LEDDriver::initialize( LD_PROTOCOL_T protocol, std::string spidev, uint16_t ledCount )
{
    spiPath = spidev;
    spifd   = (-1);

    if( protocol == LD_PROTOCOL_LPD8806 )
        pixelData = new PixelBufferLPD8806;
    else if( protocol == LD_PROTOCOL_APA102 )
        pixelData = new PixelBufferAPA102;
    else
        return;

    pixelData->initialize( ledCount );

    pendingUpdate = false;
}

bool 
LEDDriver::start( EventLoop &loop )
{
    int ret;
    const uint8_t mode   = SPI_MODE_0;
    const uint8_t bits   = 8;
    const uint32_t speed = 15000000;

    // Open the SPI bus
    spifd = open( spiPath.c_str(), O_WRONLY );
    if( spifd < 0 ) 
    {
        fprintf(stderr, "Can't open device.\n");
        return true;
    }

    // Set up SPI mode
    ret = ioctl( spifd, SPI_IOC_WR_MODE, &mode );
    if( ret == -1 ) 
    {
        return true;
    }

    // Set up word width
    ret = ioctl( spifd, SPI_IOC_WR_BITS_PER_WORD, &bits );
    if( ret == -1 ) 
    {
        return true;
    }

    // Set up clock speed
    ret = ioctl( spifd, SPI_IOC_WR_MAX_SPEED_HZ, &speed );
    if( ret == -1 ) 
    {
        return true;
    }

    // Add a callback for us
//    updateEvent.addObserver( this );

//    updateEvent.setup();

    // Register our events
//    loop.addSource( &updateEvent );

    return false;
}

void 
LEDDriver::stop()
{


}

void 
LEDDriver::setGammaCorrection( double red, double green, double blue )
{
    pixelData->setGammaCorrection( red, green, blue );
}

uint32_t 
LEDDriver::getPixelCount()
{
    return pixelData->getPixelCount();
}

void 
LEDDriver::clearAllPixels()
{
    std::cout << "LEDDriver::clearAllPixels" << std::endl;

    pixelData->clearAllPixels();

    signalUpdate();
}

void 
LEDDriver::clearPixel( uint16_t pixelIndex )
{
    pixelData->clearPixel( pixelIndex );

    signalUpdate();
}

void 
LEDDriver::setPixel( uint16_t pixelIndex, uint8_t red, uint8_t green, uint8_t blue )
{
    pixelData->setPixel( pixelIndex, red, green, blue );

    signalUpdate();
}

void 
LEDDriver::getPixel( uint16_t pixelIndex, uint8_t &red, uint8_t &green, uint8_t &blue )
{
    pixelData->getPixel( pixelIndex, red, green, blue );
}

void 
LEDDriver::testSequence()
{
   // Perform the sequence multiple times
   for( uint32_t i = 0; i < 4; i++ )
   {
       uint32_t pIndx;

       // Turn all LEDs on
       for( pIndx = 0; pIndx < getPixelCount(); pIndx++ )
       {
           setPixel( pIndx, 0xff, 0xff, 0xff );
       }

       processUpdates();

       // Sleep 1 seconds
       sleep(1);

       // Turn all LED off
       for( pIndx = 0; pIndx < getPixelCount(); pIndx++ )
       {
           setPixel( pIndx, 0, 0, 0 );
       }

       processUpdates();
       
       // Sleep 1 seconds
       sleep(1);
   }
}

void 
LEDDriver::processUpdates()
{
    size_t   size;
    size_t   attempt;
    ssize_t  result;
    uint8_t *buf;

    //std::cout << "processUpdates: start" << std::endl;

    // Nothing to do.
    if( pendingUpdate == false )
    {
        return;
    }

    //std::cout << "processUpdates: 1" << std::endl;

    // Get the pixel data buffer to send
    pixelData->getUpdateBuffer( &buf, attempt );

    //std::cout << "processUpdates: 2" << std::endl;

    // Start with the whole buffer length;
    size = attempt; 

    // Do the update process
    while( size > 0 )
    {
        //std::cout << "processUpdates - attempt: " << attempt << std::endl;
 
        // Attempt to send the buffer on the SPI bus
        result = write( spifd, buf, attempt );

        // Check if there was an error.
        if( result < 0 )
        {
            if( errno == EINTR )
                continue;
            else if( errno == EMSGSIZE )
            {
                attempt = attempt/2;
                result  = 0;
            }
            else
            {
                //std::cout << "processUpdates: error: " << errno << std::endl;

                // A non recoverable error, exit.
                return;
            }
        }

        //std::cout << "processUpdates - result: " << result << std::endl;
 
        // Advance the send pointer
        buf += result;

        // Account for already sent data
        size -= result;

        //std::cout << "processUpdates - sent: " << buf << " " << size << std::endl;

        // Take care of bit at the end.
        if( attempt > size ) 
            attempt = size;
    }

}

// Signal that an update is pending
void 
LEDDriver::signalUpdate()
{
    //updateEvent.trigger();
    pendingUpdate = true;
}

// Handle events
void 
LEDDriver::eventAction( uint32_t EventID )
{
    processUpdates();

    //updateEvent.clear();
}


