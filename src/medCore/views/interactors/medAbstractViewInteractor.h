/*=========================================================================

medInria

Copyright (c) INRIA 2013. All rights reserved.
See LICENSE.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractExtraInteractor.h>

#include "medCoreExport.h"

#include <medAbstractView.h>

class MEDCORE_EXPORT medAbstractViewInteractor : public medAbstractExtraInteractor
{
    Q_OBJECT
public:
            medAbstractViewInteractor(medAbstractView* parent);
    virtual ~medAbstractViewInteractor();

    /**
     * @brief implementationOf
     * @return Upper abstract class it derives from.
     * Do NOT reimplement it in non abstract class.
     * Used by the factory to kwnow what can be create.
     */
    static QString implementationOf()
    {
        return "medAbstractExtraInteractor";
    }
};
