#include "medComposerNodeFilteringProcess.h"

#include "dtkComposer/dtkComposerTransmitterEmitter.h"
#include "dtkComposer/dtkComposerTransmitterReceiver.h"

#include <dtkLog/dtkLog.h>

#include <dtkCore>

// /////////////////////////////////////////////////////////////////
// dtkComposerNodeProcessPrivate interface
// /////////////////////////////////////////////////////////////////

class medComposerNodeFilteringProcessPrivate
{
public:
    dtkComposerTransmitterReceiver<dtkAbstractData> receiver_data;

public:
    dtkComposerTransmitterEmitter<dtkAbstractData> emitter_data;

public:
    dtkAbstractProcess *process;

public:
    qlonglong index;
    qreal value;
};

// /////////////////////////////////////////////////////////////////
// dtkComposerNodeProcess implementation
// /////////////////////////////////////////////////////////////////

medComposerNodeFilteringProcess::medComposerNodeFilteringProcess(void) : dtkComposerNodeLeafProcess(), d(new medComposerNodeFilteringProcessPrivate)
{
    this->appendReceiver(&(d->receiver_data));

    this->appendEmitter(&(d->emitter_data));

    d->process = NULL;
}

medComposerNodeFilteringProcess::~medComposerNodeFilteringProcess(void)
{
    if (d->process)
        delete d->process;

    delete d;

    d = NULL;
}

bool medComposerNodeFilteringProcess::isAbstractProcess(void) const
{
    return true;
}

QString medComposerNodeFilteringProcess::abstractProcessType(void) const
{
    return "dtkAbstractProcess";
}

void medComposerNodeFilteringProcess::run(void)
{
    if (this->implementationHasChanged())
        d->process = this->process();

    if (!d->process) {
        dtkWarn() << "no process, abort "<< this->currentImplementation();
        return;
    }

    if (!d->receiver_data.isEmpty()) {
        dtkAbstractData *data = d->receiver_data.data();
        d->process->setInput(data);
    }

    // TODO: cast process to medAbstractProcess
    // and set parameters retrieved from toolbox
    d->index = d->process->run();

    if (d->process->data(0))
        d->value = *(static_cast<qreal *>(d->process->data(0)));

    if (d->process->output())
        d->emitter_data.setData(d->process->output());
}

QString medComposerNodeFilteringProcess::type(void)
{
    return "process";
}

QString medComposerNodeFilteringProcess::titleHint(void)
{
    return "Process";
}

QString medComposerNodeFilteringProcess::inputLabelHint(int port)
{
    if(port == 0)
        return "data";

    return dtkComposerNodeLeaf::inputLabelHint(port);
}

QString medComposerNodeFilteringProcess::outputLabelHint(int port)
{
    if(port == 0)
        return "data";

    return dtkComposerNodeLeaf::outputLabelHint(port);
}

