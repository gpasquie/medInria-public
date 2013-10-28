/* sislComposerNodeSplineBlender.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008-2011 - Julien Wintz, Inria.
 * Created: Sat Jun  2 01:50:32 2012 (+0200)
 * Version: $Id$
 * Last-Updated: 2012 Thu Nov  8 10:38:23 (+0100)
 *           By: Thibaud Kloczko, Inria.
 *     Update #: 20
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef COMPOSERNODEIMAGEREADERH
#define COMPOSERNODEIMAGEREADER_H

#include "medComposerPluginExport.h"

#include <dtkComposer/dtkComposerNodeLeaf.h>
#include <dtkComposer/dtkComposerNodeLeafData.h>

#include <QtCore>

class ComposerNodeImageReaderPrivate;

class MEDCOMPOSERPLUGIN_EXPORT ComposerNodeImageReader : public dtkComposerNodeLeafData
{
public:
     ComposerNodeImageReader(void);
    ~ComposerNodeImageReader(void);

public:
    bool isAbstractData(void) const;

    QString abstractDataType(void) const;

public:
    void run(void);

public:
    QString type(void);
    QString titleHint(void) ;

public:
    QString  inputLabelHint(int port);
    QString outputLabelHint(int port);

private:
    ComposerNodeImageReaderPrivate *d;
};

#endif
