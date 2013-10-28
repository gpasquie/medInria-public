/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <dtkCore/dtkAbstractProcess_p.h>
#include "medCoreExport.h"

////////////////////////////////////////////////////
// medAbstractProcessPrivate interface
////////////////////////////////////////////////////

class MEDCORE_EXPORT medAbstractProcessPrivate : public dtkAbstractProcessPrivate
{
public:
    medAbstractProcessPrivate(medAbstractProcess *q = 0) : dtkAbstractProcessPrivate(q) {}
    medAbstractProcessPrivate(const medAbstractProcessPrivate& other) : dtkAbstractProcessPrivate(other) {}

public:
    virtual ~medAbstractProcessPrivate(void) {}

public:
   QWidget *widget;
};

////////////////////////////////////////////////////
// dtkAbstractProcess protected constructors
////////////////////////////////////////////////////

DTK_IMPLEMENT_PRIVATE(medAbstractProcess, dtkAbstractProcess)
