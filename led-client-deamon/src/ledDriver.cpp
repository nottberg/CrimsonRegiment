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

PixelBuffer::PixelBuffer( uint16_t ledCount )
{
    // Remember the number of leds in the string
    ledCnt = ledCount;

    // Allocate a buffer to hold the led info.
    bufLength = ( ledCnt + 3 ) * sizeof( PIXEL_ENTRY_T );
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

    // Start up with no gamma correction
    gammaLookupInitialized = false;

    return;
}

PixelBuffer::~PixelBuffer()
{
    if( bufPtr )
    {
        free( bufPtr );
    }
}

void 
PixelBuffer::writePixel( uint16_t pixelIndex, uint8_t red, uint8_t green, uint8_t blue ) 
{
    PIXEL_ENTRY_T *p = (PIXEL_ENTRY_T *)( bufPtr + ( ( pixelIndex + 3 ) * sizeof( PIXEL_ENTRY_T )  ) );

    if( pixelIndex >= ledCnt )
        return;

    p->blue  = 0x80 | ( blue & 0x7F );
    p->green = 0x80 | ( green & 0x7F );
    p->red   = 0x80 | ( red & 0x7F );

    printf ("index : %i 0x%x %x %x %x\n", pixelIndex, p, p->red  , p->green, p->blue);
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

void
PixelBuffer::clearAllPixels()
{
    // Clear the buffer to zeroes.
    bzero( bufPtr, bufLength );
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
PixelBuffer::getUpdateBuffer( uint8_t **buf, size_t &length )
{
    *buf   = bufPtr;
    length = bufLength;
}

LEDDriver::LEDDriver( std::string spidev, uint16_t ledCount )
: pixelData( ledCount ) //, updateEvent( 1, "ledDriverUpdateEvent" )
{
    spiPath = spidev;
    spifd   = (-1);
    //ledCnt = ledCount;

    pendingUpdate = false;
}

LEDDriver::~LEDDriver()
{

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
    pixelData.setGammaCorrection( red, green, blue );
}

void 
LEDDriver::clearAllPixels()
{
    pixelData.clearAllPixels();

    signalUpdate();
}

void 
LEDDriver::clearPixel( uint16_t pixelIndex )
{
    pixelData.clearPixel( pixelIndex );

    signalUpdate();
}

void 
LEDDriver::setPixel( uint16_t pixelIndex, uint8_t red, uint8_t green, uint8_t blue )
{
    pixelData.setPixel( pixelIndex, red, green, blue );

    signalUpdate();
}

void 
LEDDriver::processUpdates()
{
    size_t   size;
    size_t   attempt;
    ssize_t  result;
    uint8_t *buf;

    std::cout << "processUpdates: start" << std::endl;

    // Nothing to do.
    if( pendingUpdate == false )
    {
        return;
    }

    std::cout << "processUpdates: 1" << std::endl;

    // Get the pixel data buffer to send
    pixelData.getUpdateBuffer( &buf, attempt );

    std::cout << "processUpdates: 2" << std::endl;

    // Start with the whole buffer length;
    size = attempt; 

    // Do the update process
    while( size > 0 )
    {
        std::cout << "processUpdates - attempt: " << attempt << std::endl;
 
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
                std::cout << "processUpdates: error: " << errno << std::endl;

                // A non recoverable error, exit.
                return;
            }
        }

        std::cout << "processUpdates - result: " << result << std::endl;
 
        // Advance the send pointer
        buf += result;

        // Account for already sent data
        size -= result;

        std::cout << "processUpdates - sent: " << buf << " " << size << std::endl;

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


