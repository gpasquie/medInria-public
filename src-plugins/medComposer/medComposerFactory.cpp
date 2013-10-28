#include "medComposerFactory.h"



#include "ComposerNodeImageReader.h"


#include <dtkCore/dtkAbstractView.h>
#include <dtkCore/dtkAbstractViewFactory.h>
#include <dtkComposer/dtkComposerNode.h>
#include <medComposerNodeFilteringProcess.h>

#include <dtkLog/dtkLog.h>

// /////////////////////////////////////////////////////////////////
//
// /////////////////////////////////////////////////////////////////

class medComposerFactoryPrivate
{
public:
    void initialize(void);

public:
    QList<QString> nodes;
    QHash<QString, QString> descriptions;
    QHash<QString, QStringList> tags;
    QHash<QString, QString> types;
};

void medComposerFactoryPrivate::initialize(void)
{
  /*  if(!dtkAbstractViewFactory::instance()->registerViewType("medView", createmedView))
        dtkWarn() << "Unable to register medView type";

    qRegisterMetaType<medView>("medView");*/
}

// /////////////////////////////////////////////////////////////////
//
// /////////////////////////////////////////////////////////////////

medComposerFactory::medComposerFactory(void) : dtkComposerFactory(), d(new medComposerFactoryPrivate)
{
    d->initialize();

    // Initialize attributes on super class

    d->nodes = QList<QString>(dtkComposerFactory::nodes());
    d->descriptions = QHash<QString, QString>(dtkComposerFactory::descriptions());
    // d->tags = QHash<QString, QStringList>(dtkComposerFactory::tags());
    d->types = QHash<QString, QString>(dtkComposerFactory::types());

    // Extend attributes

    d->nodes << "Image Reader";
    d->descriptions["Image Reader"] = "<p>image Reader</p>";
    d->tags["Image Reader"] = QStringList() << "ITK" << "Filter" << "view";
    d->types["Image Reader"] = "ImageReader";

    d->nodes << "Image Writer";
    d->descriptions["Image Writer"] = "<p>testFilter View.</p>";
    d->tags["Image Writer"] = QStringList() << "ITK" << "Filter" << "view";
    d->types["Image Writer"] = "ImageWriter";

   // medComposerNodeFilteringProcess
    d->nodes << "Filtering Process";
    d->descriptions["Filtering Process"] = "<p>testFilter View.</p>";
    d->tags["Filtering Process"] = QStringList() << "ITK" << "Filter" << "view";
    d->types["Filtering Process"] = "FilteringProcess";

}

medComposerFactory::~medComposerFactory(void)
{
    delete d;

    d = NULL;
}

dtkComposerNode *medComposerFactory::create(const QString& type)
{
    if(type == "ImageReader")
        return new ComposerNodeImageReader;

    if(type == "FilteringProcess")
        return new medComposerNodeFilteringProcess;

    return dtkComposerFactory::create(type);
}

QList<QString> medComposerFactory::nodes(void)
{
    return d->nodes;
}

QHash<QString, QString> medComposerFactory::descriptions(void)
{
    return d->descriptions;
}

QHash<QString, QStringList> medComposerFactory::tags(void)
{
    return d->tags;
}

QHash<QString, QString> medComposerFactory::types(void)
{
    return d->types;
}
