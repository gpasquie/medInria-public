// /////////////////////////////////////////////////////////////////
// Generated by medPluginGenerator
// /////////////////////////////////////////////////////////////////

#pragma once

#include <medRegistrationAbstractToolBox.h>
#include <undoRedoRegistration.h>
#include <undoRedoRegistrationPluginExport.h>

class undoRedoRegistrationToolBoxPrivate;

class UNDOREDOREGISTRATIONPLUGIN_EXPORT undoRedoRegistrationToolBox : public medRegistrationAbstractToolBox
{
    Q_OBJECT
    
public:
    undoRedoRegistrationToolBox(QWidget *parent = 0);
    ~undoRedoRegistrationToolBox(void);
    
public:
    static bool registered(void);
    void updatePositionArrow(int);
    undoRedoRegistration::RegImageType::Pointer output(void);
    
    virtual void setRegistrationToolBox(medRegistrationSelectorToolBox *toolbox);

public slots:
    void onUndo(void);
    void onRedo(void);
    void onTransformationStackReset(void);
    void addTransformationIntoList(int, QString);
    void onRegistrationSuccess();

private:
    undoRedoRegistrationToolBoxPrivate *d;
};


