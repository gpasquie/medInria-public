/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <QDialog>
#include <medAbstractView.h>

class medParameterPoolEditorPrivate;


class medParameterPoolEditor : public QDialog
{
    Q_OBJECT

public:
    medParameterPoolEditor(QWidget *parent = 0);
    virtual ~medParameterPoolEditor();

    void setViews(QList<medAbstractView*>);

private slots:
    void loadGroups();
    void loadGroup(QString groupName);
    void loadGroup(QListWidgetItem* groupItem);
    void createNewGroup();
    void save();
    void clear();
    void validate();
    void deleteGroup();
    void saveAsViewPresets();
    void saveAsLayerPresets();
    void updateViewParams(QListWidgetItem* item);
    void updateLayerParams(QListWidgetItem* item);
    void selectItem(QListWidgetItem* item);

private:
    void initLists();
    void enableLists(bool enabled);
    void loadPresets();
    void addlinkAllButton(QListWidget* list);
    void clear(QListWidget *list);


private:
    medParameterPoolEditorPrivate *d;

};


