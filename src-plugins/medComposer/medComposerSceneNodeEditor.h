/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <dtkComposer/dtkComposerSceneNodeEditor.h>

class medComposerSceneNodeEditor : public dtkComposerSceneNodeEditor
{
    Q_OBJECT

public:
    medComposerSceneNodeEditor(QWidget *parent);

public slots:
    void showProcessWidget(const QString&);

};


