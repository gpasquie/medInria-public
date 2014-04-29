#include "medParameterPoolEditor.h"

#include <medAbstractLayeredView.h>
#include <medAbstractParameter.h>
#include <medParameterPoolManager.h>
#include <medParameterPool.h>
#include <medMetaDataKeys.h>
#include <medSettingsManager.h>

#include <QtGui>


class medSavePresetDialog : public QDialog
{
public:
    medSavePresetDialog(QStringList paramNames, QWidget* parent = 0,
                        Qt::WindowFlags flags = 0) : QDialog(parent, flags)
    {
        QString paramText = paramNames.join(", ");
        QHBoxLayout *layout = new QHBoxLayout;
        presetNameEdit = new QLineEdit();
        layout->addWidget(new QLabel("Preset name for " + paramText));
        layout->addWidget(presetNameEdit);

        okButton = new QPushButton("OK");
        cancelButton = new QPushButton("Cancel");
        QHBoxLayout *buttonLayout = new QHBoxLayout;
        buttonLayout->addWidget(okButton);
        buttonLayout->addWidget(cancelButton);

        QVBoxLayout *mainLayout = new QVBoxLayout;
        mainLayout->addLayout(layout);
        mainLayout->addLayout(buttonLayout);

        this->setLayout(mainLayout);

        connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
        connect(cancelButton, SIGNAL(clicked()), this, SLOT(accept()));
    }

    QString presetname()
    {
        return presetNameEdit->text();
    }

private:
    QLineEdit *presetNameEdit;
    QPushButton *okButton;
    QPushButton *cancelButton;
};


class medParameterPoolEditorPrivate
{
public:
    QList<medAbstractView*> views;

    QLineEdit *newGroupNameEdit;
    QPushButton *newGroupButton;
    QListWidget *groupListWidget;
    QPushButton *deleteGroupButton;

    QListWidget *viewListWidget;
    QListWidget *layerListWidget;
    QListWidget *viewParamListWidget;
    QListWidget *layerParamListWidget;
    QListWidget *viewPresetListWidget;
    QListWidget *layerPresetListWidget;

    QPushButton *saveAsViewPresetButton;
    QPushButton *saveAsLayerPresetButton;

    QLineEdit *viewPresetNameEdit;
    QLineEdit *layerPresetNameEdit;

    QPushButton *clearButton;

    QPushButton *okButton;
    QPushButton *cancelButton;

    QHash<QListWidgetItem *, medAbstractView*> itemToViewHash;
    QHash<QListWidgetItem *, medAbstractData*> itemToDataHash;

    QMultiHash<medAbstractView*, medAbstractParameter*> viewToParamHash;
    QMultiHash<medAbstractData*, medAbstractParameter*> dataToParamHash;

    QMultiHash<QListWidgetItem *, QListWidgetItem *> viewPresetItemToParamItemHash;
    QMultiHash<QListWidgetItem *, QListWidgetItem *> layerPresetItemToParamItemHash;

    QString currentGroup;
};


Q_DECLARE_METATYPE( medAbstractView* )
Q_DECLARE_METATYPE( medAbstractParameter* )

medParameterPoolEditor::medParameterPoolEditor(QWidget *parent): QDialog(), d(new medParameterPoolEditorPrivate)
{
    QHBoxLayout *mainLayout = new QHBoxLayout(this);

    d->newGroupButton = new QPushButton(tr("New Group"));
    d->newGroupNameEdit = new QLineEdit;
    d->groupListWidget = new QListWidget;
    d->deleteGroupButton = new QPushButton(tr("Delete Group"));

    QHBoxLayout *newGroupLayout = new QHBoxLayout;
    newGroupLayout->addWidget(d->newGroupNameEdit);
    newGroupLayout->addWidget(d->newGroupButton);

    QHBoxLayout *editGroupLayout = new QHBoxLayout;
    editGroupLayout->addWidget(d->deleteGroupButton, 1);

    QVBoxLayout *groupLayout = new QVBoxLayout;
    groupLayout->addWidget(new QLabel("Define or select a group in the list:"));
    groupLayout->addWidget(d->groupListWidget);
    groupLayout->addLayout(editGroupLayout);
    groupLayout->addLayout(newGroupLayout);

    d->viewListWidget = new QListWidget;
    d->layerListWidget = new QListWidget;
    d->viewParamListWidget = new QListWidget;
    d->layerParamListWidget = new QListWidget;
    d->viewPresetListWidget = new QListWidget;
    d->layerPresetListWidget = new QListWidget;

    d->saveAsViewPresetButton = new QPushButton(tr("Save as preset"));
    d->saveAsLayerPresetButton = new QPushButton(tr("Save as preset"));

    d->viewPresetNameEdit = new QLineEdit;
    d->layerPresetNameEdit = new QLineEdit;

    d->clearButton = new QPushButton(tr("Clear"));
    d->okButton = new QPushButton(tr("OK"));

    QGridLayout *rightLayout = new QGridLayout;

    QLabel *viewLabel = new QLabel("And then associate views and a set of their parameters:");
    QLabel *layerLabel = new QLabel("And / or layers and a set of their parameters:");

    rightLayout->addWidget(viewLabel,                 0, 0, 1, 3);
    rightLayout->addWidget(d->viewParamListWidget,    1, 0, 3, 2);
    rightLayout->addWidget(d->saveAsViewPresetButton, 4, 0, 1, 1);
    rightLayout->addWidget(d->viewPresetNameEdit,     4, 1, 1, 1);
    rightLayout->addWidget(d->viewPresetListWidget,   5, 0, 2, 2);
    rightLayout->addWidget(d->viewListWidget,         1, 2, 6, 1);

    rightLayout->addWidget(layerLabel,                7, 0, 1, 3);
    rightLayout->addWidget(d->layerParamListWidget,   8, 0, 3, 2);
    rightLayout->addWidget(d->saveAsLayerPresetButton,11, 0, 1, 1);
    rightLayout->addWidget(d->layerPresetNameEdit,    11, 1, 1, 1);
    rightLayout->addWidget(d->layerPresetListWidget,  12, 0, 2, 2);
    rightLayout->addWidget(d->layerListWidget,        8, 2, 6, 1);

    rightLayout->addWidget(d->clearButton,            14, 1);
    rightLayout->addWidget(d->okButton,               14, 2);

    mainLayout->addLayout(groupLayout);
    mainLayout->addLayout(rightLayout);

    connect(d->newGroupButton, SIGNAL(clicked()), this, SLOT(createNewGroup()));
    connect(d->groupListWidget, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(loadGroup(QListWidgetItem*)));
    connect(d->okButton, SIGNAL(clicked()), this, SLOT(validate()));
    connect(d->clearButton, SIGNAL(clicked()), this, SLOT(clear()));
    connect(d->deleteGroupButton, SIGNAL(clicked()), this, SLOT(deleteGroup()));
    connect(d->saveAsViewPresetButton, SIGNAL(clicked()), this, SLOT(saveAsViewPresets()));
    connect(d->saveAsLayerPresetButton, SIGNAL(clicked()), this, SLOT(saveAsLayerPresets()));
    connect(d->viewPresetListWidget, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(updateViewParams(QListWidgetItem*)));
    connect(d->layerPresetListWidget, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(updateLayerParams(QListWidgetItem*)));

    connect(d->viewListWidget, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(selectItem(QListWidgetItem*)));
    connect(d->layerListWidget, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(selectItem(QListWidgetItem*)));
    connect(d->viewParamListWidget, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(selectItem(QListWidgetItem*)));
    connect(d->layerParamListWidget, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(selectItem(QListWidgetItem*)));
    connect(d->viewPresetListWidget, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(selectItem(QListWidgetItem*)));
    connect(d->layerPresetListWidget, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(selectItem(QListWidgetItem*)));

    enableLists(false);
}

medParameterPoolEditor::~medParameterPoolEditor()
{

}

void medParameterPoolEditor::setViews(QList<medAbstractView*> views)
{
    d->views = views;

    initLists();

    loadGroups();

    loadPresets();
}


void medParameterPoolEditor::initLists()
{
    QList<QString> viewParamNames;
    QList<QString> layerParamNames;

    foreach(medAbstractView *view, d->views)
    {
        // create an item for the view
        QListWidgetItem *viewItem = new QListWidgetItem(view->identifier());
        viewItem->setCheckState(Qt::Unchecked);
        d->viewListWidget->addItem(viewItem);

        d->itemToViewHash.insert(viewItem, view);

        foreach(medAbstractParameter *param, view->navigatorsParameters())
        {
            d->viewToParamHash.insert(view, param);

            if(!viewParamNames.contains(param->name()))
            {
                // create an item for the view parameter
                QListWidgetItem *paramViewItem = new QListWidgetItem(param->name());
                paramViewItem->setCheckState(Qt::Unchecked);
                d->viewParamListWidget->addItem(paramViewItem);

                viewParamNames.append(param->name());
            }
        }

        medAbstractLayeredView * layeredView = dynamic_cast<medAbstractLayeredView *>(view);
        if(layeredView)
        {
            for(int i=0; i<layeredView->layersCount(); i++)
            {
                QString name = medMetaDataKeys::SeriesDescription.getFirstValue(layeredView->layerData(i),"<i>no name</i>");

                // create an item for the view
                QListWidgetItem *layerItem = new QListWidgetItem(name);
                layerItem->setCheckState(Qt::Unchecked);
                d->layerListWidget->addItem(layerItem);

                d->itemToDataHash.insert(layerItem, layeredView->layerData(i));

                foreach(medAbstractInteractor *interactor, layeredView->interactors(i))
                {
                    foreach(medAbstractParameter *param, interactor->parameters())
                    {
                        d->dataToParamHash.insert(layeredView->layerData(i), param);

                        if(!layerParamNames.contains(param->name()))
                        {
                            // create an item for the layer parameter
                            QListWidgetItem *paramLayerItem = new QListWidgetItem(param->name());
                            paramLayerItem->setCheckState(Qt::Unchecked);
                            d->layerParamListWidget->addItem(paramLayerItem);

                            layerParamNames.append(param->name());
                        }
                    }
                }
            }
        }
    }
}


void medParameterPoolEditor::createNewGroup()
{
    save();

    clear();

    QString groupName = d->newGroupNameEdit->text();
    if(groupName.isEmpty())
        groupName = "Group" + QString::number(d->groupListWidget->count()+1);

    QListWidgetItem *groupItem = new QListWidgetItem(groupName);
    d->groupListWidget->addItem(groupItem);
    groupItem->setSelected(true);

    enableLists(true);

    d->currentGroup = groupName;

    d->newGroupNameEdit->setText("Group" + QString::number(d->groupListWidget->count()+1));
}


void medParameterPoolEditor::loadGroups()
{
    medParameterPoolManager *poolManager = medParameterPoolManager::instance();
    int nbGroups = 0;
    foreach(medParameterPool* group, poolManager->pools())
    {
        QListWidgetItem *groupItem = new QListWidgetItem(group->name());
        d->groupListWidget->addItem(groupItem);
        nbGroups++;
    }

    d->newGroupNameEdit->setText("Group"+QString::number(nbGroups+1));
}

void medParameterPoolEditor::loadGroup(QListWidgetItem* groupItem)
{
    loadGroup(groupItem->text());
}

void medParameterPoolEditor::loadGroup(QString groupName)
{
    enableLists(true);

    // save group edition in progress
    save();

    clear();

    d->currentGroup = groupName;

    medParameterPoolManager *poolManager = medParameterPoolManager::instance();

    medParameterPool* group = poolManager->pool(groupName);

    if(!group)
        return;

    foreach(medAbstractParameter* param, group->parameters())
    {
        medAbstractView *view = d->viewToParamHash.key(param);
        medAbstractData *data = d->dataToParamHash.key(param);
        if(view)
        {
            QListWidgetItem *viewItem = d->itemToViewHash.key(view);
            viewItem->setCheckState(Qt::Checked);

            QList<QListWidgetItem *> paramItems = d->viewParamListWidget->findItems(param->name(), Qt::MatchExactly);
            if(!paramItems.isEmpty())
              paramItems.at(0)->setCheckState(Qt::Checked);
        }
        else if(data)
        {
            QListWidgetItem *dataItem = d->itemToDataHash.key(data);
            dataItem->setCheckState(Qt::Checked);

            QList<QListWidgetItem *> paramItems = d->layerParamListWidget->findItems(param->name(), Qt::MatchExactly);
            if(!paramItems.isEmpty())
              paramItems.at(0)->setCheckState(Qt::Checked);
        }
    }

}

void medParameterPoolEditor::save()
{
    medParameterPoolManager *poolManager = medParameterPoolManager::instance();

    if(d->currentGroup == "")
        return;

    // remove existing group to be sure, group is up to date
    poolManager->removePool(d->currentGroup);

    for(int i=0; i<d->viewListWidget->count(); i++)
    {
        QListWidgetItem *viewItem = d->viewListWidget->item(i);

        if(viewItem->checkState() == Qt::Unchecked)
            continue;

        medAbstractView *view = d->itemToViewHash.value(viewItem);

        for(int j=0; j<d->viewParamListWidget->count(); j++)
        {
            QListWidgetItem *viewParamItem = d->viewParamListWidget->item(j);

            if(viewParamItem->checkState() == Qt::Checked)
            {
                QList<medAbstractParameter*> viewParams = d->viewToParamHash.values(view);
                foreach(medAbstractParameter* viewParam, viewParams)
                {
                    if(viewParam->name() == viewParamItem->text())
                    {
                        poolManager->linkParameter(viewParam, d->currentGroup);
                    }
                }
            }
        }
    }

    for(int i=0; i<d->layerListWidget->count(); i++)
    {
        QListWidgetItem *layerItem = d->layerListWidget->item(i);

        if(layerItem->checkState() == Qt::Unchecked)
            continue;

        medAbstractData *data = d->itemToDataHash.value(layerItem);

        for(int j=0; j<d->layerParamListWidget->count(); j++)
        {
            QListWidgetItem *layerParamItem = d->layerParamListWidget->item(j);

            if(layerParamItem->checkState() == Qt::Checked)
            {
                QList<medAbstractParameter*> layerParams = d->dataToParamHash.values(data);
                foreach(medAbstractParameter* layerParam, layerParams)
                {
                    if(layerParam->name() == layerParamItem->text())
                    {
                        poolManager->linkParameter(layerParam, d->currentGroup);
                    }
                }
            }
        }
    }
}

void medParameterPoolEditor::addlinkAllButton(QListWidget* list)
{
    QListWidgetItem * linkAllItem = new QListWidgetItem("All");
    linkAllItem->setCheckState(Qt::Unchecked);
    list->addItem(linkAllItem);
}

void medParameterPoolEditor::clear()
{
    clear(d->viewListWidget);
    clear(d->layerListWidget);
    clear(d->viewParamListWidget);
    clear(d->layerParamListWidget);
    clear(d->viewPresetListWidget);
    clear(d->layerPresetListWidget);
}

void medParameterPoolEditor::clear(QListWidget *list)
{
    for(int i=0; i<list->count(); i++)
    {
        QListWidgetItem *item = list->item(i);
        item->setCheckState(Qt::Unchecked);
    }
}


void medParameterPoolEditor::validate()
{
    save();

    accept();
}

void medParameterPoolEditor::enableLists(bool enabled)
{
    d->viewListWidget->setEnabled(enabled);
    d->layerListWidget->setEnabled(enabled);
    d->viewParamListWidget->setEnabled(enabled);
    d->layerParamListWidget->setEnabled(enabled);
    d->viewPresetListWidget->setEnabled(enabled);
    d->layerPresetListWidget->setEnabled(enabled);
}

void medParameterPoolEditor::deleteGroup()
{
    if(d->groupListWidget->selectedItems().isEmpty())
    {
        QMessageBox box;
        box.setText(tr("Please select a group first"));
        box.exec();
    }
    medParameterPoolManager *poolManager = medParameterPoolManager::instance();
    poolManager->removePool(d->currentGroup);

    qDeleteAll(d->groupListWidget->selectedItems());

    clear();
}

void medParameterPoolEditor::saveAsViewPresets()
{
    QStringList params;
    QList<QListWidgetItem *> itemsInPreset;

    for(int i=0; i<d->viewParamListWidget->count(); i++)
    {
        QListWidgetItem *item = d->viewParamListWidget->item(i);
        if(item->checkState() == Qt::Checked)
        {
            itemsInPreset.append(item);
            params.append(item->text());
        }
    }

    if(itemsInPreset.isEmpty())
        return;

    QString presetName = d->viewPresetNameEdit->text();

    if(presetName.isEmpty())
    {
        QMessageBox box;
        box.setText(tr("Please enter a preset name first"));
        box.exec();
        return;
    }

    QListWidgetItem *viewPresetItem = new QListWidgetItem(presetName);
    viewPresetItem->setCheckState(Qt::Checked);
    d->viewPresetListWidget->addItem(viewPresetItem);

    foreach(QListWidgetItem * item, itemsInPreset)
        d->viewPresetItemToParamItemHash.insert(viewPresetItem, item);

    d->viewPresetNameEdit->setText("");

    medSettingsManager::instance()->setValue("ViewPresets", presetName, QVariant(params));
}


void medParameterPoolEditor::saveAsLayerPresets()
{
    QStringList params;
    QList<QListWidgetItem *> itemsInPreset;
    for(int i=0; i<d->layerParamListWidget->count(); i++)
    {
        QListWidgetItem *item = d->layerParamListWidget->item(i);
        if(item->checkState() == Qt::Checked)
        {
            itemsInPreset.append(item);
            params.append(item->text());
        }
    }

    if(itemsInPreset.isEmpty())
        return;

    QString presetName = d->layerPresetNameEdit->text();

    if(presetName.isEmpty())
    {
        QMessageBox box;
        box.setText(tr("Please enter a preset name first"));
        box.exec();
    }

    QListWidgetItem *layerPresetItem = new QListWidgetItem(presetName);
    layerPresetItem->setCheckState(Qt::Checked);
    d->layerPresetListWidget->addItem(layerPresetItem);

    foreach(QListWidgetItem * item, itemsInPreset)
        d->layerPresetItemToParamItemHash.insert(layerPresetItem, item);


    d->layerPresetNameEdit->setText("");

    medSettingsManager::instance()->setValue("LayerPresets", presetName, QVariant(params));
}

void medParameterPoolEditor::updateViewParams(QListWidgetItem* presetItem)
{
    QList<QListWidgetItem *> itemsInPreset = d->viewPresetItemToParamItemHash.values(presetItem);

    foreach(QListWidgetItem * item, itemsInPreset)
        item->setCheckState(presetItem->checkState());

}

void medParameterPoolEditor::updateLayerParams(QListWidgetItem* presetItem)
{
    QList<QListWidgetItem *> itemsInPreset = d->layerPresetItemToParamItemHash.values(presetItem);

    foreach(QListWidgetItem * item, itemsInPreset)
        item->setCheckState(presetItem->checkState());

}

void medParameterPoolEditor::selectItem(QListWidgetItem* item)
{
    if(item->checkState() == Qt::Unchecked)
      item->setCheckState(Qt::Checked);
    else
      item->setCheckState(Qt::Unchecked);
}

void medParameterPoolEditor::loadPresets()
{
    QStringList viewsPresets = medSettingsManager::instance()->keys("ViewPresets");

    foreach(QString preset, viewsPresets)
    {
        QStringList params = medSettingsManager::instance()->value("ViewPresets", preset).toStringList();

        if(!params.isEmpty())
        {
            QListWidgetItem *viewPresetItem = new QListWidgetItem(preset);
            viewPresetItem->setCheckState(Qt::Unchecked);
            d->viewPresetListWidget->addItem(viewPresetItem);

            for(int i=0; i<d->viewParamListWidget->count(); i++)
            {
                QListWidgetItem *item = d->viewParamListWidget->item(i);
                if( params.contains(item->text()) )
                  d->viewPresetItemToParamItemHash.insert(viewPresetItem, item);
            }
        }
    }

    QStringList layerPresets = medSettingsManager::instance()->keys("LayerPresets");

    foreach(QString preset, layerPresets)
    {
        QStringList params = medSettingsManager::instance()->value("LayerPresets", preset).toStringList();

        if(!params.isEmpty())
        {
            QListWidgetItem *layerPresetItem = new QListWidgetItem(preset);
            layerPresetItem->setCheckState(Qt::Unchecked);
            d->layerPresetListWidget->addItem(layerPresetItem);

            for(int i=0; i<d->layerParamListWidget->count(); i++)
            {
                QListWidgetItem *item = d->layerParamListWidget->item(i);
                if( params.contains(item->text()) )
                  d->layerPresetItemToParamItemHash.insert(layerPresetItem, item);
            }
        }
    }
}
