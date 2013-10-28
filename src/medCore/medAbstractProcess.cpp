#include "medAbstractProcess.h"
#include "medAbstractProcess_p.h"


medAbstractProcess::medAbstractProcess(medAbstractProcess *parent) : dtkAbstractProcess(*new medAbstractProcessPrivate(this), parent)
{
    DTK_D(medAbstractProcess);

    d->widget = NULL;
}

medAbstractProcess::medAbstractProcess(const medAbstractProcess& other) : dtkAbstractProcess(*new medAbstractProcessPrivate(*other.d_func()), other)
{
    DTK_D(medAbstractProcess);

    d->widget = NULL;
}

/*
QWidget * medAbstractProcess::widget()
{
    DTK_D(medAbstractProcess);

    return d->widget;
}*/

/*
void medAbstractProcess::setWidget(QWidget *widget)
{
    DTK_D(medAbstractProcess);

    d->widget = widget;
}*/
