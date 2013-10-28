/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include "medComposerPluginExport.h"

#include "dtkComposer/dtkComposerNodeLeafProcess.h"

class medComposerNodeFilteringProcessPrivate;

// /////////////////////////////////////////////////////////////////
// dtkComposerNodeProcess interface
// /////////////////////////////////////////////////////////////////

class MEDCOMPOSERPLUGIN_EXPORT medComposerNodeFilteringProcess : public dtkComposerNodeLeafProcess
{
public:
     medComposerNodeFilteringProcess(void);
    ~medComposerNodeFilteringProcess(void);

public:
    bool isAbstractProcess(void) const;

    QString abstractProcessType(void) const;

public:
    void run(void);

public:
    QString type(void);
    QString titleHint(void);

public:
    QString inputLabelHint(int);
    QString outputLabelHint(int);

private:
    medComposerNodeFilteringProcessPrivate *d;
};
