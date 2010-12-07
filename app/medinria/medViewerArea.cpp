/* medViewerArea.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Fri Sep 18 12:43:06 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Wed Nov 10 16:15:44 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 1061
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "medViewerArea.h"
#include "medViewerArea_p.h"
#include "medViewerAreaStack.h"
#include "medViewerConfigurator.h"
#include "medViewerToolBoxConfiguration.h"

#include "medGui/medViewerToolBoxLayout.h"
#include "medViewerToolBoxPatient.h"
#include "medGui/medViewerToolBoxView.h"

#include <dtkCore/dtkAbstractViewFactory.h>
#include <dtkCore/dtkAbstractView.h>
#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataReader.h>
#include <dtkCore/dtkGlobal.h>

#include <dtkVr/dtkVrHeadRecognizer.h>
#include <dtkVr/dtkVrGestureRecognizer.h>

#include <medCore/medDataIndex.h>
#include <medCore/medDataManager.h>
#include <medCore/medViewManager.h>
#include <medCore/medAbstractView.h>

#include <medSql/medDatabaseController.h>
#include <medSql/medDatabaseNonPersitentItem.h>
#include <medSql/medDatabaseNonPersitentController.h>
#include <medSql/medDatabaseNavigator.h>

#include <medGui/medClutEditor.h>
#include <medGui/medToolBox.h>
#include <medGui/medToolBoxContainer.h>
#include <medGui/medToolBoxRegistration.h>
#include <medGui/medToolBoxDiffusion.h>
#include <medGui/medViewContainer.h>
#include <medGui/medViewContainerCustom.h>
#include <medGui/medViewContainerMulti.h>
#include <medGui/medViewContainerSingle.h>
#include <medGui/medViewPool.h>
#include <medGui/medViewerConfiguration.h>
#include <medGui/medViewerConfigurationFactory.h>

#include <QtGui>
#include <QtSql>

// /////////////////////////////////////////////////////////////////
// medViewerArea
// /////////////////////////////////////////////////////////////////

medViewerArea::medViewerArea(QWidget *parent) : QWidget(parent), d(new medViewerAreaPrivate)
{
    // -- Internal logic

    d->current_patient = -1;
    d->configurations = new QHash<QString,medViewerConfiguration *>();
    d->current_configuration = "";

    // -- User interface setup

    d->stack = new QStackedWidget(this);
    d->stack->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);


    // Setting up toolbox container

    d->toolbox_container = new medToolBoxContainer(this);
    d->toolbox_container->setFixedWidth(320);

    // Setting up view container

    QWidget *view_container = new QWidget(this);

    QVBoxLayout *view_container_layout = new QVBoxLayout(view_container);
    view_container_layout->setContentsMargins(0, 10, 0, 10);
    view_container_layout->addWidget(d->stack);

    // Setting up navigator container

    QFrame *navigator_container = new QFrame(this);
    navigator_container->setObjectName("medNavigatorContainer");
    navigator_container->setFixedWidth(186);

    d->navigator = new medDatabaseNavigator(navigator_container);

    QVBoxLayout *navigator_container_layout = new QVBoxLayout(navigator_container);
    navigator_container_layout->setContentsMargins(0, 0, 0, 0);
    navigator_container_layout->setSpacing(0);
    navigator_container_layout->addWidget(d->navigator);
    navigator_container_layout->setAlignment(Qt::AlignHCenter);

    //action for transfer function
    QAction * transFunAction =
      new QAction("Toggle Tranfer Function Widget", this);
    transFunAction->setShortcut(Qt::ControlModifier + Qt::ShiftModifier +
				Qt::Key_L);
    transFunAction->setCheckable( true );
    transFunAction->setChecked( false );
    connect(transFunAction, SIGNAL(toggled(bool)),
	    this, SLOT(bringUpTransferFunction(bool)));

    this->addAction(transFunAction);
}

medViewerArea::~medViewerArea(void)
{
    //TODO: delete
    delete d->configurations;
    delete d;

    d = NULL;
}

//! Status bar setup
/*! 
 *  Called whenever the viewer area is switched to. Add viewer area
 *  specific status widgets on the left of the status bar.
 */

void medViewerArea::setup(QStatusBar *status)
{

}

//! Status bar setdown
/*! 
 *  Called whenever the viewer area is left. Remove viewer area
 *  specific status widgets from the left of the status bar.
 */

void medViewerArea::setdw(QStatusBar *status)
{

}



//! Split the currently displayed custom container.
/*! 
 *  This slots make the connection between the layout toolbox gui and
 *  the actual custom view container.
 */

void medViewerArea::split(int rows, int cols)
{
    if (d->view_stacks.count())
        d->view_stacks.value(d->current_patient)->current()->split(rows, cols);
}

//! Open data corresponding to index \param index.
/*! 
 * 
 */

void medViewerArea::open(const medDataIndex& index)
{
    if(!((medDataIndex)index).isValid())
        return;

    this->switchToPatient(index.patientId());

    if(((medDataIndex)index).isValidForSeries()) {

    dtkAbstractData *data = NULL;
    dtkAbstractView *view = NULL;

    if(!data)
        data = medDataManager::instance()->data(index);

    if(!data)
        data = medDatabaseNonPersitentController::instance()->data(index);

    if(!data)
        data = medDatabaseController::instance()->read(index);

    if(!data)
        return;

    medDataManager::instance()->insert(index, data);

    if(!view)
        view = d->view_stacks.value(d->current_patient)->current()->current()->view();

    if(!view)
        view = dtkAbstractViewFactory::instance()->create("v3dView");
    
    if(!view) {
        qDebug() << "Unable to create a v3dView";
        return;
    }

    medViewManager::instance()->insert(index, view);
    
    view->setData(data);
    view->reset(); // called in view_stacks -> setView but seems necessary with the streaming approach

    QMutexLocker ( &d->mutex );
    d->view_stacks.value(d->current_patient)->current()->current()->setView(view);
    d->view_stacks.value(d->current_patient)->current()->current()->setFocus(Qt::MouseFocusReason);
	
	
    return;

    }

    if(((medDataIndex)index).isValidForPatient()) {

    this->switchToPatient(index.patientId());
    this->switchToContainer(1);

    QSqlQuery stQuery(*(medDatabaseController::instance()->database()));
    stQuery.prepare("SELECT * FROM study WHERE patient = :id");
    stQuery.bindValue(":id", index.patientId());
    if(!stQuery.exec())
	qDebug() << DTK_COLOR_FG_RED << stQuery.lastError() << DTK_NO_COLOR;

    while(stQuery.next()) {
      
        QSqlQuery seQuery(*(medDatabaseController::instance()->database()));
        seQuery.prepare("SELECT * FROM series WHERE study = :id");
        seQuery.bindValue(":id", stQuery.value(0));
        if(!seQuery.exec())
	    qDebug() << DTK_COLOR_FG_RED << seQuery.lastError() << DTK_NO_COLOR;
        
        while(seQuery.next())
	    this->open(medDataIndex(index.patientId(), stQuery.value(0).toInt(), seQuery.value(0).toInt()));
    }

    }
}

//! Open file on the local filesystem.
/*! 
 * 
 */

void medViewerArea::open(const QString& file)
{
    this->open(medDatabaseNonPersitentController::instance()->read(file));
}

//! Switch the view area layout to the one of patient with database index \param index.
/*! 
 * 
 */

void medViewerArea::switchToPatient(int id)
{
    if(id < 0)
        return;

    d->current_patient = id;

    // Setup view container

    medViewerAreaStack *view_stack;

    if(!d->view_stacks.contains(d->current_patient)) {
        view_stack = new medViewerAreaStack(this);
        view_stack->setPatientId(d->current_patient);
        connect(view_stack, SIGNAL(dropped(medDataIndex)), this, SLOT(open(medDataIndex)));
        connect(view_stack, SIGNAL(focused(dtkAbstractView*)), this, SLOT(onViewFocused(dtkAbstractView*)));
        d->view_stacks.insert(d->current_patient, view_stack);
        d->stack->addWidget(view_stack);
    } else {
        view_stack = d->view_stacks.value(d->current_patient);
    }

    d->stack->setCurrentWidget(view_stack);

    // Setup navigator

    d->navigator->onPatientClicked(d->current_patient);

    // Setup patient toolbox
    //TODO emit a signal to the Patient Toolbox
    //emit (setPatientIndex(id));

    // Setup layout toolbox

    
}

//! Set stack index.
/*! 
 *  This method actually allows one to switch between the
 *  single/multi/custom modes for the currently displayed view
 *  stack. A view stack is composed of a single/custom/multi layout.
 */

void medViewerArea::switchToContainer(int index)
{
    if(index < 0)
        return;

    d->current_container = index;

    if (d->view_stacks.count())
        if (d->view_stacks.value(d->current_patient))
	    d->view_stacks.value(d->current_patient)->setCurrentIndex(index);
}

//! Set custom view preset
/*! 
 *  Presets are defined in src/medGui/medViewContainerCustom.
 */

void medViewerArea::switchToContainerPreset(int index)
{
    if(index < 0)
        return;

    if (d->view_stacks.count()) {
        if (d->view_stacks.value(d->current_patient)) {
	    if(medViewContainerCustom *custom = dynamic_cast<medViewContainerCustom *>(d->view_stacks.value(d->current_patient)->custom())) {
                custom->setPreset(index);
            }
        }
    }
}

void medViewerArea::addToolBox(medToolBox *toolbox)
{
    d->toolbox_container->addToolBox(toolbox);
}

void medViewerArea::removeToolBox(medToolBox *toolbox)
{
    d->toolbox_container->removeToolBox(toolbox);
}


#include <dtkVr/dtkVrController.h>

//! View focused callback. 
/*! 
 *  This method updates the toolboxes according to the focused view.
 */

void medViewerArea::onViewFocused(dtkAbstractView *view)
{
    // set head recognizer

    static dtkVrHeadRecognizer *head_recognizer = NULL;

    if(dtkApplicationArgumentsContain(qApp, "--tracker")) {

        if(!head_recognizer) {
            head_recognizer = new dtkVrHeadRecognizer;
            head_recognizer->startConnection(QUrl(dtkApplicationArgumentsValue(qApp, "--tracker")));
        }

        if(view->property("Orientation") == "3D")
            head_recognizer->setView(view);
        else
            head_recognizer->setView(NULL);
    }

    // set gesture recognizer

    static dtkVrGestureRecognizer *gesture_recognizer = NULL;

    if(dtkApplicationArgumentsContain(qApp, "--tracker")) {

        if(!gesture_recognizer) {
            gesture_recognizer = new dtkVrGestureRecognizer;
            gesture_recognizer->startConnection(QUrl(dtkApplicationArgumentsValue(qApp, "--tracker")));
        }

        gesture_recognizer->setView(view);
        gesture_recognizer->setReceiver(static_cast<medAbstractView *>(view)->receiverWidget());
    }

    // Update toolboxes
    //TODO: Send events instead of methods, here
    //d->viewToolBox->update(view);
    //d->diffusionToolBox->update(view);

    this->updateTransferFunction();
}

//! Returns the currently displayed stack. 
/*! 
 *  A stack is a set a view containers for a given patient.
 */

medViewerAreaStack *medViewerArea::currentStack(void)
{
    return d->view_stacks.value(d->current_patient);
}

//! Returns the currently displayed container of the currently displayed stack.
/*! 
 * 
 */

medViewContainer *medViewerArea::currentContainer(void)
{
    return d->view_stacks.value(d->current_patient)->current();
}

//! Returns the currently focused child container.
/*! 
 *  Note that container are hierarchical structures. This methods
 *  returns a container that can contain a view.
 */

medViewContainer *medViewerArea::currentContainerFocused(void)
{
    return d->view_stacks.value(d->current_patient)->current()->current();
}

// view settings

void medViewerArea::setupForegroundLookupTable(QString table)
{
    if(!d->view_stacks.count())
        return;

    if ( medViewPool *pool = this->currentContainer()->pool() )
        pool->setViewProperty("LookupTable", table);

    this->updateTransferFunction();
}
/*
void medViewerArea::setupBackgroundLookupTable(QString table)
{
    if(!d->view_stacks.count())
        return;
    
    if ( medViewPool *pool = this->currentContainer()->pool() ) {
        pool->setViewProperty("BackgroundLookupTable", table);
    }

    this->updateTransferFunction();
}
*/
void medViewerArea::setupAxisVisibility(bool visible)
{
    if(!d->view_stacks.count())
        return;

    if(medViewPool *pool = this->currentContainer()->pool()) {
        pool->setViewProperty("ShowAxis", (visible ? "true" : "false"));
    }
}

void medViewerArea::setupScalarBarVisibility(bool visible)
{
    if(!d->view_stacks.count())
        return;
  
    if(medViewPool *pool = this->currentContainer()->pool()) {
	pool->setViewProperty("ShowScalarBar",
			      (visible ? "true" : "false"));
    }
}

void medViewerArea::setupRulerVisibility(bool visible)
{
    if(!d->view_stacks.count())
        return;
  
    if(medViewPool *pool = this->currentContainer()->pool()) {
	pool->setViewProperty("ShowRuler", (visible ? "true" : "false"));
    }
}

void medViewerArea::setupAnnotationsVisibility(bool visible)
{
    if(!d->view_stacks.count())
        return;
  
    if(medViewPool *pool = this->currentContainer()->pool()) {
	pool->setViewProperty("ShowAnnotations", (visible ? "true" : "false"));
    }
}

void medViewerArea::setup3DMode(QString mode)
{
    if(!d->view_stacks.count())
        return;
  
    if(medViewPool *pool = this->currentContainer()->pool()) {
        pool->setViewProperty("3DMode", mode);
    }
}

void medViewerArea::setup3DVRMode(QString mode)
{
    if(!d->view_stacks.count())
        return;
  
    if(medViewPool *pool = this->currentContainer()->pool()) {
        pool->setViewProperty("Renderer", mode);
    }
}

void medViewerArea::setupLUTPreset(QString table)
{
    if(!d->view_stacks.count())
        return;
  
    if ( medViewPool *pool = this->currentContainer()->pool() ) {
        pool->setViewProperty("Preset", table);
    }

    this->updateTransferFunction();
}

void medViewerArea::setup3DLOD(int value)
{
    if(!d->view_stacks.count())
        return;

    if(dtkAbstractView *view =  this->currentContainer()->current()->view()) {
        view->setMetaData("LOD", QString::number(value));
	view->update();
    }
}

void medViewerArea::setupWindowing(bool checked)
{
    if(!d->view_stacks.count())
        return;

    if(medViewPool *pool = this->currentContainer()->pool()) {
        pool->setViewProperty("MouseInteraction", "Windowing");
    }
}

void medViewerArea::setupZooming(bool checked)
{
    if(!d->view_stacks.count())
        return;
  
    if(medViewPool *pool = this->currentContainer()->pool()) {
        pool->setViewProperty("MouseInteraction", "Zooming");
    }
}

void medViewerArea::setupSlicing(bool checked)
{
    if(!d->view_stacks.count())
        return;
  
    if(medViewPool *pool = this->currentContainer()->pool()) {
        pool->setViewProperty("MouseInteraction", "Slicing");
    }
}

void medViewerArea::setupMeasuring(bool checked)
{
    if(!d->view_stacks.count())
        return;
  
    if(medViewPool *pool = this->currentContainer()->pool()) {
        pool->setViewProperty("MouseInteraction", "Measuring");
    }
}

void medViewerArea::setupCropping(bool checked)
{
    if(!d->view_stacks.count())
        return;
  
    if(medViewPool *pool = this->currentContainer()->pool()) {
        pool->setViewProperty("Cropping", (checked ? "true" : "false"));
    }
}

void medViewerArea::bringUpTransferFunction(bool checked)
{
    if (!checked)
    {
        if (d->transFun !=NULL )
        {
            delete d->transFun ;
            d->transFun=NULL;
        }
	return;
    }
    if(!d->view_stacks.count())
        return;
  
    if ( dtkAbstractView *view = this->currentContainerFocused()->view() ) {

      d->transFun = new medClutEditor(NULL);
      d->transFun->setWindowModality( Qt::WindowModal );
      d->transFun->setWindowFlags(Qt::Tool|Qt::WindowStaysOnTopHint);

      // d->transFun->setData(static_cast<dtkAbstractData *>(view->data()));
      d->transFun->setView(dynamic_cast<medAbstractView*>(view));

      d->transFun->show();
    }
}

void medViewerArea::updateTransferFunction()
{
    dtkAbstractView * view = this->currentContainerFocused()->view();
    if ( d->transFun && view ) {
	// d->transFun->setData( static_cast<dtkAbstractData *>( view->data() ) );
	d->transFun->setView( dynamic_cast<medAbstractView *>( view ), true );
	d->transFun->update();
    }
}
//TODO: don't know why it's been removed from .h file...
//void medViewerArea::setupLayoutCompare(void)
//{
//    if(!d->view_stacks.count())
//        return;

//    d->view_stacks.value(d->current_patient)->setCurrentIndex(3);
//}

//TODO: move this to the configuration
//void medViewerArea::setupLayoutFuse(void)
//{
//    if(!d->view_stacks.count())
//        return;

//    d->view_stacks.value(d->current_patient)->setCurrentIndex(4);
	
//    if (d->registrationToolBox->fuseView()) {
//        this->currentContainer()->setView(d->registrationToolBox->fuseView());
//        this->currentContainer()->setFocus(Qt::MouseFocusReason);
//    }
//}


void medViewerArea::setupConfiguration(const QString& name)
{
    medViewerConfiguration *conf = NULL;

    if (d->current_configuration == name)
    {
        return;
    }

    if (!d->configurations->contains(name))
    {
        if (conf = medViewerConfigurationFactory::instance()->createConfiguration(name))
        {
            d->configurations->insert(name, conf);
        }
        else
        {
            qDebug()<< "Configuration" << name << "couldn't be created";
            return;
        }
    }

    //clean toolboxes
    d->toolbox_container->clear();

    //switch
    switch (conf->layoutType()){
        case medViewerConfiguration::LeftDbRightTb:
            //setup orientation
            d->toolbox_container->setOrientation(medToolBoxContainer::Vertical);
            break;
        case medViewerConfiguration::LeftTbRightDb:
            d->toolbox_container->setOrientation(medToolBoxContainer::Vertical);
            break;
        case medViewerConfiguration::TopDbBottomTb:
            d->toolbox_container->setOrientation(medToolBoxContainer::Horizontal);
            break;
        case medViewerConfiguration::TopTbBottomDb:
            d->toolbox_container->setOrientation(medToolBoxContainer::Horizontal);
            break;
        default:
            qDebug() << "unhandled case in configuration layout switch";

    }



    //setup database visibility
    d->navigator->setVisible(conf->databaseVisibility());

    //setup layout type
    switchToContainer(conf->viewLayoutType());
    if (conf->layoutType() == medViewContainer::Custom)//TODO check index for custom
    {
        switchToContainerPreset(conf->customLayoutType());
    }

    //add new toolboxes
    foreach (medToolBox * toolbox, conf->toolBoxes() )
        this->addToolBox(toolbox);

    d->current_configuration = name;

}