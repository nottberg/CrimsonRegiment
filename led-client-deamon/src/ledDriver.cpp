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

bool PixelBuffer::gammaLookupInitialized = false;

uint8_t  PixelBuffer::redGammaLookup[256];
uint8_t  PixelBuffer::greenGammaLookup[256];
uint8_t  PixelBuffer::blueGammaLookup[256];

PixelBuffer::PixelBuffer( uint16_t ledCount )
{
    ledCnt = ledCount;
    bufLength = ( ledCnt + 3 ) * sizeof( PIXEL_ENTRY_T );
    bufPtr = (uint8_t *) malloc( bufLength );
  
    if( bufPtr == NULL ) 
    {
        bufLength = 0;
        return;
    }

    bzero( bufPtr, bufLength );

    if( gammaLookupInitialized == false )
    {
        for( int i=0; i < sizeof( redGammaLookup ); i++ ) 
        {
            redGammaLookup[i]     = ( 0x80 | (int)( pow( ( (float) i / 255.0 ), 2.5 ) * 127.0 + 0.5 ) );
            greenGammaLookup[i] = ( 0x80 | (int)( pow( ( (float) i / 255.0 ), 2.5 ) * 127.0 + 0.5 ) );
            blueGammaLookup[i]   = ( 0x80 | (int)( pow( ( (float) i / 255.0 ), 2.5 ) * 127.0 + 0.5 ) );
        }

        gammaLookupInitialized = true;
    }

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
    PIXEL_ENTRY_T *p = (PIXEL_ENTRY_T *)( bufPtr + ( pixelIndex * sizeof( PIXEL_ENTRY_T )  ) );

    p->blue   = blue;
    p->green = green;
    p->red    = red;
    // printf ("index : %i %i %i %i\n",p, p->red  , p->green, p->blue);
}

LEDDriver::LEDDriver( std::string spidev, uint16_t ledCount )
: pixelData( ledCount )
{
    spiPath = spidev;
    spifd = (-1);
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
    const uint8_t bits       = 8;
    const uint32_t speed = 15000000;

    spifd = open( spiPath.c_str(), O_WRONLY );
    if( spifd < 0 ) 
    {
        fprintf(stderr, "Can't open device.\n");
        return true;
    }

    ret = ioctl( spifd, SPI_IOC_WR_MODE, &mode );
    if( ret == -1 ) 
    {
        return true;
    }

    ret = ioctl( spifd, SPI_IOC_WR_BITS_PER_WORD, &bits );
    if( ret == -1 ) 
    {
        return true;
    }

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

