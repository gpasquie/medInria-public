/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <dtkCore/dtkAbstractProcess.h>
#include "medCoreExport.h"

class medAbstractProcessPrivate;

class medAbstractProcess : public dtkAbstractProcess
{
    Q_OBJECT

public:
    medAbstractProcess(      medAbstractProcess *parent = 0);
    medAbstractProcess(const medAbstractProcess& other);

    virtual QWidget * widget();


private:
    DTK_DECLARE_PRIVATE(medAbstractProcess);

};
