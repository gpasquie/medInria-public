/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medComposer.h"
#include "medComposerPlugin.h"

#include <dtkLog/dtkLog.h>

// /////////////////////////////////////////////////////////////////
// medComposerPluginPrivate
// /////////////////////////////////////////////////////////////////

class medComposerPluginPrivate
{
public:
    // Class variables go here.
    static const char *s_Name;
};

const char * medComposerPluginPrivate::s_Name = "medComposer";

// /////////////////////////////////////////////////////////////////
// medComposerPlugin
// /////////////////////////////////////////////////////////////////

medComposerPlugin::medComposerPlugin(QObject *parent) : dtkPlugin(parent), d(new medComposerPluginPrivate)
{
    
}

medComposerPlugin::~medComposerPlugin()
{
    delete d;
    
    d = NULL;
}

bool medComposerPlugin::initialize()
{
    if(!medComposerWorkspace::registered())
        dtkWarn() << "Unable to register medComposer type";
    
    return true;
}

bool medComposerPlugin::uninitialize()
{
    return true;
}

QString medComposerPlugin::name() const
{
    return "medComposerPlugin";
}

QString medComposerPlugin::description() const
{
    return tr("");
}

QString medComposerPlugin::version() const
{
    return MEDCOMPOSERPLUGIN_VERSION;
}

QString medComposerPlugin::contact() const
{
    return "";
}

QStringList medComposerPlugin::authors() const
{
    QStringList list;
    return list;
}

QStringList medComposerPlugin::contributors() const
{
    QStringList list;
    return list;
}

QString medComposerPlugin::identifier() const
{
    return medComposerPluginPrivate::s_Name;
}


QStringList medComposerPlugin::tags() const
{
    return QStringList();
}

QStringList medComposerPlugin::types() const
{
    return QStringList() << "medComposer";
}
QStringList medComposerPlugin::dependencies() const
{
    return QStringList();
}
Q_EXPORT_PLUGIN2(medComposerPlugin, medComposerPlugin)
