/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medItkImageData.h>
#include <medItkImageExport.h>

extern const char medItkChar3ImageDataName[] = "medItkChar3ImageData";
typedef MEDITKIMAGEPLUGIN_EXPORT medItkImageData<3,char,medItkChar3ImageDataName> medItkChar3ImageData;


