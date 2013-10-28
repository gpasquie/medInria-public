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

#include <dtkComposer/dtkComposerFactory.h>

#include <QtCore>

class medComposerFactoryPrivate;

class MEDCOMPOSERPLUGIN_EXPORT medComposerFactory : public dtkComposerFactory
{
    Q_OBJECT

public:
     medComposerFactory(void);
    ~medComposerFactory(void);

public slots:
    dtkComposerNode *create(const QString& type);

public:
    QList<QString> nodes(void);
    QHash<QString, QString> descriptions(void);
    QHash<QString, QStringList> tags(void);
    QHash<QString, QString> types(void);

private:
    medComposerFactoryPrivate *d;
};
