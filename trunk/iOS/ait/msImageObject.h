//=================================================================================================================================
//
// Author: Maurice Ribble
//         3D Application Research Group
//         ATI Research, Inc.
//
// Definitions for the ImageObject class.  This class holds and manages all image data information.
//
//=================================================================================================================================
// $Id: //depot/3darg/Tools/Handheld/esTestApps/common/ImageObject.h#2 $ 
// 
// Last check-in:  $DateTime: 2008/03/21 10:01:35 $ 
// Last edited by: $Author: dginsbur $
//=================================================================================================================================
//   (C) ATI Research, Inc. 2006 All rights reserved. 
//=================================================================================================================================

#ifndef _IMAGEOBJECT_H_
#define _IMAGEOBJECT_H_

#include "msGL.h"

enum CompressedType
{
   IMG_NONE,
   IMG_ATITC_RGB,
   IMG_ATITC_RGBA,
   IMG_ATI2N,
   IMG_ATI1N,
   IMG_ETC1,
   IMG_ETC3,
   IMG_ETC5,
};

class msImageObject
{
public:
   /// \brief Constructor
   msImageObject( GLuint width, GLuint height, GLuint numChannels, GLuint bitsPerChannel );

   /// \brief Destructor
   ~msImageObject();

   /// \brief Access a single component of a single pixel
   unsigned char& Pixel( GLuint x, GLuint y, GLuint channel );

   /// \brief Sets up the type for compressed data
   bool SetupCompressedData( CompressedType type );

   /// \brief Sets up the size and data buffer for a compressed texture
   void SetCompressedSize( GLuint size );

   /// \brief Changes the main data (not compressed data) to red
   void MakeImageRed();

   /// \brief Changes the main data from RGBA -> ARGB
   bool MakeImageARGB();

   GLuint GetWidth()             { return this->width; }
   GLuint GetHeight()            { return this->height; }
   GLuint GetNumChannels()       { return this->numChannels; }
   GLuint GetBitsPerChannel()    { return this->bitsPerChannel; }
   GLuint GetBpp()               { return this->numChannels * this->bitsPerChannel; }
   unsigned char* GetDataPtr()   { return this->data; }
   GLuint GetCompressedType()    { return this->compressedType; }
   GLuint GetCompressedSize()    { return this->compressedSize; }
   unsigned char* GetCompressedDataPtr() { return this->compressedData; }

private:
   /// The width of the texture
   GLuint width;

   /// The height of the texture
   GLuint height;

   /// The number of channels for the texture
   GLuint numChannels;

   /// The bits per channel for the texture
   GLuint bitsPerChannel;

   /// The data (not compressed) for the texture
   unsigned char *data;

   /// The compressed texture type
   CompressedType compressedType;

   /// The size of the data for the compressed texture
   GLuint compressedSize;

   /// The compressed data for the texture
   unsigned char  *compressedData;
};

#endif // _IMAGEOBJECT_H_
