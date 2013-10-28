/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <dtkCore/dtkAbstractData.h>
#include <medWorkspace.h>
#include <dtkComposer/dtkComposerWriter.h>

#include "medComposerPluginExport.h"

class medComposerWorkspacePrivate;
    
class MEDCOMPOSERPLUGIN_EXPORT medComposerWorkspace : public medWorkspace
{
    Q_OBJECT
    
public:
    medComposerWorkspace(QWidget *parent);
    virtual ~medComposerWorkspace();
    
    virtual QString description() const;
    
    static bool registered();

    virtual QString identifier()  const{return "Composer";}
    static bool isUsable(){return true;}
    void setupViewContainerStack ();

public slots:
    bool compositionOpen(void);
    bool compositionOpen(const QString& file);
    bool compositionSave(void);
    bool compositionSaveAs(void);
    bool compositionSaveAs(const QString& file, dtkComposerWriter::Type type = dtkComposerWriter::Ascii);
    bool compositionInsert(void);
    bool compositionInsert(const QString& file);

private:
    medComposerWorkspacePrivate *d;
};

//dtkAbstractData *createMedComposer();


