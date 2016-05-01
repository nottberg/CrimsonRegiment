#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include <errno.h>
#include <math.h>
#include <stdlib.h>
#include <strings.h>

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

    // Write out the zeros on next opportunity
    updateFlag = true;

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

    p->blue  = blue;
    p->green = green;
    p->red   = red;
    // printf ("index : %i %i %i %i\n",p, p->red  , p->green, p->blue);
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

bool 
PixelBuffer::updatePending()
{
    return updateFlag;
}

void 
PixelBuffer::clearUpdatePending()
{
    updateFlag = false;
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

    updateFlag = true;
}

void
PixelBuffer::clearPixel( uint16_t pixelIndex )
{
    writePixel( pixelIndex, 0, 0, 0 );

    updateFlag = true;
}

void
PixelBuffer::setPixel( uint16_t pixelIndex, uint8_t red, uint8_t green, uint8_t blue )
{
    if( gammaLookupInitialized == true )
    {
        writeGammaPixel( pixelIndex, 0, 0, 0 );
    }
    else
    {
        writePixel( pixelIndex, 0, 0, 0 );
    }

    updateFlag = true;
}

void 
PixelBuffer::getUpdateBuffer( uint8_t **buf, size_t &length )
{
    *buf   = bufPtr;
    length = bufLength;
}

LEDDriver::LEDDriver( std::string spidev, uint16_t ledCount )
: pixelData( ledCount )
{
    spiPath = spidev;
    spifd   = (-1);
    //ledCnt = ledCount;
}

LEDDriver::~LEDDriver()
{

}

bool 
LEDDriver::start()
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

    return false;
}

void 
LEDDriver::stop()
{


}

PixelBuffer& 
LEDDriver::getPixelBuffer()
{
    return pixelData;
}

void 
LEDDriver::processUpdates()
{
    size_t   size;
    size_t   attempt;
    ssize_t  result;
    uint8_t *buf;

    // Check if there is anything to do.
    if( pixelData.updatePending() == false )
        return;

    // Get the pixel data buffer to send
    pixelData.getUpdateBuffer( &buf, attempt );

    // Start with the whole buffer length;
    size = attempt; 

    // Do the update process
    while( size > 0 )
    {
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
                // A non recoverable error, exit.
                return;
            }
        }
 
        // Advance the send pointer
        buf += result;

        // Account for already sent data
        size -= result;

        // Take care of bit at the end.
        if( attempt > size ) 
            attempt = size;
    }

    // Mark the update as complete
    pixelData.clearUpdatePending();
}

