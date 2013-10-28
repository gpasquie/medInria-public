/* sislComposerNodeSplineBlender.cpp ---
 *
 * Author: Julien Wintz
 * Copyright (C) 2008-2011 - Julien Wintz, Inria.
 * Created: Sat Jun  2 01:51:59 2012 (+0200)
 * Version: $Id$
 * Last-Updated: 2012 Thu Nov  8 11:48:49 (+0100)
 *           By: Thibaud Kloczko, Inria.
 *     Update #: 69
 */

/* Commentary:
 *
 */

/* Change log:
 *
 */

#include "ComposerNodeImageReader.h"

#include <dtkComposer/dtkComposerTransmitterEmitter.h>
#include <dtkComposer/dtkComposerTransmitterReceiver.h>

#include <dtkCore/dtkAbstractDataFactory.h>
//#include <dtkCore/dtkAbstractProcess.h>



// /////////////////////////////////////////////////////////////////
// sislComposerNodeSplineBlenderPrivate declaration
// /////////////////////////////////////////////////////////////////

class ComposerNodeImageReaderPrivate
{
public:
    dtkSmartPointer<dtkAbstractDataReader> dataReader;

public:
    dtkComposerTransmitterReceiver<QString> receiver_file;

public:
    dtkComposerTransmitterEmitter<dtkAbstractData> emitter_image;
};

// /////////////////////////////////////////////////////////////////
// sislComposerNodeSplineBlender implementation
// /////////////////////////////////////////////////////////////////

ComposerNodeImageReader::ComposerNodeImageReader(void) : dtkComposerNodeLeafData(), d(new ComposerNodeImageReaderPrivate)
{
    d->dataReader = NULL;

    this->appendReceiver(&(d->receiver_file));

    this->appendEmitter(&(d->emitter_image));
}

ComposerNodeImageReader::~ComposerNodeImageReader(void)
{
    delete d;

    d = NULL;
}

bool ComposerNodeImageReader::isAbstractData(void) const
{
    return false;
}

QString ComposerNodeImageReader::abstractDataType(void) const
{
    //return "sislAbstractSplineBlender";
    return "TODO";
}

void ComposerNodeImageReader::run(void)
{
    bool read = false;

    QString filename = *d->receiver_file.data();
    QList<QString> readers = dtkAbstractDataFactory::instance()->readers();

    if ( readers.size() ==0 )
    {
        return;
    }

    // cycle through readers to see if the last used reader can handle the file
    dtkSmartPointer<dtkAbstractDataReader> tempdataReader = NULL;

    for (int i=0; i<readers.size(); i++) {
        tempdataReader = dtkAbstractDataFactory::instance()->readerSmartPointer(readers[i]);
        if (tempdataReader->canRead(filename)) {
            /*d->lastSuccessfulReaderDescription = dataReader->identifier();*/
            tempdataReader->enableDeferredDeletion(false);
            d->dataReader = tempdataReader;
            break;
        }
    }

    if(d->dataReader)
    {
        read = d->dataReader->read(filename);
        d->emitter_image.setData(d->dataReader->data());
        
        if(read)
            qDebug() << "Read success";
        else
            qDebug() << "Read failure";
    }


}

QString ComposerNodeImageReader::type(void)
{
    return "ImageReader";
}

QString ComposerNodeImageReader::titleHint(void)
{
    return "ImageReader";
}

QString ComposerNodeImageReader::inputLabelHint(int port)
{
                                                                                                                                                                                                                                                                switch (port) {
    case 0:
        return "file";
        /*case 1:
            return "rhs";*/
    default:
        return dtkComposerNodeLeaf::inputLabelHint(port);
    }
}

QString ComposerNodeImageReader::outputLabelHint(int port)
{
    switch (port) {
    case 0:
        return "image";
    default:
        return dtkComposerNodeLeaf::outputLabelHint(port);
    }
}
