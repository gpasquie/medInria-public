
// /////////////////////////////////////////////////////////////////
// Generated by medPluginGenerator
// /////////////////////////////////////////////////////////////////

#pragma once

#include <dtkCore/dtkPlugin.h>

#include "undoRedoRegistrationPluginExport.h"

class UNDOREDOREGISTRATIONPLUGIN_EXPORT undoRedoRegistrationPluginPrivate;

class UNDOREDOREGISTRATIONPLUGIN_EXPORT undoRedoRegistrationPlugin : public dtkPlugin
{
    Q_OBJECT
    Q_INTERFACES(dtkPlugin)
    
public:
    undoRedoRegistrationPlugin(QObject *parent = 0);
    ~undoRedoRegistrationPlugin(void);
    
    virtual bool initialize(void);
    virtual bool uninitialize(void);
    
    virtual QString name(void) const;
    virtual QString identifier(void) const;
    virtual QString description(void) const;
    virtual QString contact(void) const;
    virtual QString version (void) const;
    virtual QStringList authors(void) const;
    virtual QStringList contributors(void) const;
    virtual QStringList dependencies(void) const;
    
    virtual QStringList tags(void) const;
    virtual QStringList types(void) const;
    
private:
    undoRedoRegistrationPluginPrivate *d;
};


