//=================================================================================================================================
//
// Author: Maurice Ribble
//         3D Application Research Group
//         ATI Research, Inc.
//
// Definetions for the SimpleImageLoader class.  This class loads only handles tga files and does not use FreeImage.
//
//=================================================================================================================================
// $Id: //depot/3darg/Tools/Handheld/esTestApps/common/SimpleImageLoader.h#1 $ 
// 
// Last check-in:  $DateTime: 2008/01/23 11:45:40 $ 
// Last edited by: $Author: dginsbur $
//=================================================================================================================================
//   (C) ATI Research, Inc. 2006 All rights reserved. 
//=================================================================================================================================

#ifndef _SIMPLEIMAGELOADER_H_
#define _SIMPLEIMAGELOADER_H_

#include "msImageObject.h"

class msSimpleImageLoader
{
public:
   /// \brief Constructor
   msSimpleImageLoader();

   /// \brief Deconstructor
   ~msSimpleImageLoader();

   /// \brief Loads an image from a file.
   msImageObject* LoadImageFile( const char *fileName );

   /// \brief Saves an image to a file.
   bool SaveImage( const char *fileName, msImageObject *imgObj );
};

#endif // _SIMPLEIMAGELOADER_H_

