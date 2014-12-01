/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medComposerArea.h>
#include <dtkGui/dtkSplitter.h>
#include <dtkGui/dtkToolBox.h>
#include <dtkComposer/dtkComposerCompass.h>
#include <dtkComposer/dtkComposerControls.h>
#include <dtkComposer/dtkComposerFactoryView.h>
#include <dtkComposer/dtkComposerScene.h>
#include <dtkComposer/dtkComposerGraph.h>
#include <dtkComposer/dtkComposerSceneModel.h>
#include <dtkComposer/dtkComposerSceneNodeEditor.h>
#include <dtkComposer/dtkComposerSceneView.h>
#include <dtkComposer/dtkComposerStackView.h>
#include <dtkComposer/dtkComposerGraphView.h>
#include <dtkComposer/dtkComposerView.h>
#include <dtkDistributed/dtkDistributor.h>
#include <medComposer/medComposer.h>
#include <medComposer/medComposerFactory.h>
#include <medDataSourceManager.h>
#include <medDatabaseDataSource.h>

class medComposerAreaPrivate
{
public:
    medComposer *composer;
    dtkComposerFactoryView *nodes;
    dtkComposerSceneModel *model;
    dtkComposerSceneNodeEditor *editor;
    dtkComposerSceneView *scene;
    dtkComposerStackView *stack;
    dtkComposerGraphView *graphView;

public:
    QString current_composition;
public:
    QPushButton *button_open;
    QPushButton *button_save;
    QPushButton *button_save_as;
    QPushButton *button_insert;
public:
    QPushButton *button_flag_blue;
    QPushButton *button_flag_gray;
    QPushButton *button_flag_green;
    QPushButton *button_flag_orange;
    QPushButton *button_flag_pink;
    QPushButton *button_flag_red;
    QPushButton *button_flag_yellow;
public:
    QPushButton *button_start;
    QPushButton *button_stop;
    QPushButton *button_cont;
};

// ///////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////
medComposerArea::medComposerArea(QWidget *parent) : QFrame(parent)
{
    d = new medComposerAreaPrivate;
    d->button_open = new QPushButton("Open");
    d->button_save = new QPushButton("Save");
    d->button_save_as = new QPushButton("Save As");
    d->button_insert = new QPushButton("Insert");
    QHBoxLayout *f_layout = new QHBoxLayout;
    f_layout->addWidget(d->button_open);
    f_layout->addWidget(d->button_save);
    f_layout->addWidget(d->button_save_as);
    f_layout->addWidget(d->button_insert);
    QFrame *f_menu = new QFrame(this);
    f_menu->setLayout(f_layout);
    connect(d->button_open, SIGNAL(clicked()), this, SLOT(compositionOpen()));
    connect(d->button_save, SIGNAL(clicked()), this, SLOT(compositionSave()));
    connect(d->button_save_as, SIGNAL(clicked()), this, SLOT(compositionSaveAs()));
    connect(d->button_insert, SIGNAL(clicked()), this, SLOT(compositionInsert()));
    d->composer = new medComposer;
    medComposerFactory *factory = new medComposerFactory;
    d->composer->setFactory(factory);
    d->composer->view()->setCacheMode(QGraphicsView::CacheBackground);
    d->composer->compass()->setBackgroundBrush(QColor("#222222"));
    d->editor = new dtkComposerSceneNodeEditor(this);
    d->editor->setScene(d->composer->scene());
    d->editor->setStack(d->composer->stack());
    d->editor->setGraph(d->composer->graph());
    d->model = new dtkComposerSceneModel(this);
    d->model->setScene(d->composer->scene());

    // Not really useful for now
    d->scene = new dtkComposerSceneView(this);
    d->scene->setScene(d->composer->scene());
    d->scene->setModel(d->model);
    d->stack = new dtkComposerStackView(this);
    d->stack->setStack(d->composer->stack());

    d->nodes = new dtkComposerFactoryView(this);
    d->nodes->setFactory(d->composer->factory());
    d->nodes->setDark();

    d->graphView = new dtkComposerGraphView(this);
    d->graphView->setScene(d->composer->graph());
    // ///////////////////////////////////////////////////////////////////
    //
    // ///////////////////////////////////////////////////////////////////
    d->button_flag_blue = new QPushButton(this);
    d->button_flag_blue->setIcon(d->composer->scene()->flagAsBlueAction()->icon());
    d->button_flag_gray = new QPushButton(this);
    d->button_flag_gray->setIcon(d->composer->scene()->flagAsGrayAction()->icon());
    d->button_flag_green = new QPushButton(this);
    d->button_flag_green->setIcon(d->composer->scene()->flagAsGreenAction()->icon());
    d->button_flag_orange = new QPushButton(this);
    d->button_flag_orange->setIcon(d->composer->scene()->flagAsOrangeAction()->icon());
    d->button_flag_pink = new QPushButton(this);
    d->button_flag_pink->setIcon(d->composer->scene()->flagAsPinkAction()->icon());
    d->button_flag_red = new QPushButton(this);
    d->button_flag_red->setIcon(d->composer->scene()->flagAsRedAction()->icon());
    d->button_flag_yellow = new QPushButton(this);
    d->button_flag_yellow->setIcon(d->composer->scene()->flagAsYellowAction()->icon());

    // Not really useful for now
//    QHBoxLayout *n_layout = new QHBoxLayout;
//    n_layout->addWidget(d->button_flag_blue);
//    n_layout->addWidget(d->button_flag_gray);
//    n_layout->addWidget(d->button_flag_green);
//    n_layout->addWidget(d->button_flag_orange);
//    n_layout->addWidget(d->button_flag_pink);
//    n_layout->addWidget(d->button_flag_red);
//    n_layout->addWidget(d->button_flag_yellow);
//    QFrame *n_menu = new QFrame(this);
//    n_menu->setLayout(n_layout);

    connect(d->button_flag_blue, SIGNAL(clicked()), d->composer->scene()->flagAsBlueAction(), SLOT(trigger()));
    connect(d->button_flag_gray, SIGNAL(clicked()), d->composer->scene()->flagAsGrayAction(), SLOT(trigger()));
    connect(d->button_flag_green, SIGNAL(clicked()), d->composer->scene()->flagAsGreenAction(), SLOT(trigger()));
    connect(d->button_flag_orange, SIGNAL(clicked()), d->composer->scene()->flagAsOrangeAction(), SLOT(trigger()));
    connect(d->button_flag_pink, SIGNAL(clicked()), d->composer->scene()->flagAsPinkAction(), SLOT(trigger()));
    connect(d->button_flag_red, SIGNAL(clicked()), d->composer->scene()->flagAsRedAction(), SLOT(trigger()));
    connect(d->button_flag_yellow, SIGNAL(clicked()), d->composer->scene()->flagAsYellowAction(), SLOT(trigger()));
    // ///////////////////////////////////////////////////////////////////
    d->button_start = new QPushButton("Start");
    d->button_start->setObjectName("left");
    d->button_stop = new QPushButton("Stop");
    d->button_stop->setObjectName("right");
    d->button_cont = new QPushButton("Continue");
    d->button_cont->setObjectName("Continue");
    QHBoxLayout *c_layout = new QHBoxLayout;
    c_layout->addWidget(d->button_start);
    c_layout->addWidget(d->button_stop);
    c_layout->addWidget(d->button_cont);
    QFrame *c_menu = new QFrame(this);
    c_menu->setLayout(c_layout);
    connect(d->button_start, SIGNAL(clicked()), d->composer, SLOT(run()));
    connect(d->button_stop, SIGNAL(clicked()), d->composer, SLOT(stop()));
    connect(d->button_cont, SIGNAL(clicked()), d->composer, SLOT(cont()));
    dtkDistributor *distributor = new dtkDistributor(this);
    distributor->setApplication("medSlave");
    // -- Layout
    dtkSplitter *left = new dtkSplitter(this);
    left->setOrientation(Qt::Vertical);
    left->addWidget(f_menu);
    left->addWidget(c_menu);

    //left->addWidget(n_menu);


//    QWidget *navWidget = medDataSourceManager::instance()->databaseDataSource()->compactViewWidget();
//    //navWidget->resize( navWidget->width(), navWidget->height() );
//    //little tricks to force to recompute the stylesheet.
//    navWidget->setStyleSheet("/* */");

    foreach (medAbstractDataSource* dataSource, medDataSourceManager::instance()->dataSources())
    {
        //TODO - Fix this, it's ugly - RDE

        if(medDatabaseDataSource* dbDataSource = qobject_cast<medDatabaseDataSource*>(dataSource))
        {
            QVBoxLayout *databaseViewLayout = new QVBoxLayout;
            databaseViewLayout->setSpacing(0);
            databaseViewLayout->setContentsMargins(0,0,0,0);

            QWidget *compactViewWidget = dbDataSource->compactViewWidget();
            databaseViewLayout->addWidget(compactViewWidget);
            //d->navigatorContainer->setLayout(databaseViewLayout);

            compactViewWidget->resize(compactViewWidget->width(), compactViewWidget->height());
            //little tricks to force to recompute the stylesheet.
            compactViewWidget->setStyleSheet("/* */");

            left->addWidget(compactViewWidget);

            //    connect(compactViewWidget, SIGNAL(open(const medDataIndex&)),
            //            this, SIGNAL(open(const medDataIndex&)),
            //            Qt::UniqueConnection);
        }
    }

//    left->addWidget(navWidget);
    left->addWidget(d->nodes);

    //Moved to right panel instead
    //left->addWidget(distributor);

    // Not really useful for now
    //dtkComposerControls *controls = new dtkComposerControls(this);
    //controls->setScene(d->composer->scene());

    dtkSplitter *right = new dtkSplitter(this);
    right->setOrientation(Qt::Vertical);

    // Not really useful for now
    //right->addWidget(d->scene);

    right->addWidget(d->editor);

    // Not really useful for now
    //right->addWidget(d->stack);

    right->addWidget(d->composer->compass());
    right->addWidget(distributor);

    //right->addWidget(d->graphView);

    // Not really useful for now
    //right->addWidget(controls);
    QWidget *middle = new QWidget(this);
    QVBoxLayout *m_layout = new QVBoxLayout(middle);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setSpacing(0);
    m_layout->addWidget(d->composer);
    dtkSplitter *inner = new dtkSplitter(this);
    inner->setOrientation(Qt::Horizontal);
    inner->addWidget(left);
    inner->addWidget(middle);
    inner->addWidget(right);
    inner->setSizes(QList<int>()
                    << 300
                    << 1000
                    << 300);
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(inner);
}

medComposerArea::~medComposerArea(void)
{
    delete d;
}

void medComposerArea::setup(QStatusBar *status)
{
    Q_UNUSED(status);
}

void medComposerArea::setdw(QStatusBar *status)
{
    Q_UNUSED(status);
}

bool medComposerArea::compositionOpen(void)
{
    QFileDialog *dialog = new QFileDialog(this, tr("Open composition"), QDir::homePath(), QString("medinria composition (*.med)"));
    dialog->setStyleSheet("background-color: none ; color: none;");
    dialog->setAcceptMode(QFileDialog::AcceptOpen);
    dialog->setFileMode(QFileDialog::AnyFile);
    dialog->open(this, SLOT(compositionOpen(const QString&)));
    return true;
}

bool medComposerArea::compositionOpen(const QString& file)
{
    bool status = d->composer->open(file);
    return status;
}

bool medComposerArea::compositionSave(void)
{
    bool status;
    if(d->current_composition.isEmpty() || d->current_composition == "untitled.med")
        status = this->compositionSaveAs();
    else
        status = d->composer->save();
    if(status)
        this->setWindowModified(false);
    return status;
}

bool medComposerArea::compositionSaveAs(void)
{
    bool status = false;
    QStringList nameFilters;
    nameFilters << "Ascii composition (*.med)";
    nameFilters << "Binary composition (*.med)";
    QFileDialog dialog(this, "Save composition as ...", QDir::homePath(), QString("medinria composition (*.med)"));
    dialog.setStyleSheet("background-color: none ; color: none;");
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setConfirmOverwrite(true);
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setNameFilters(nameFilters);
    dialog.setDefaultSuffix("med");
    if(dialog.exec()) {
        if(dialog.selectedNameFilter() == nameFilters.at(0))
            status = this->compositionSaveAs(dialog.selectedFiles().first(), dtkComposerWriter::Ascii);
        else
            status = this->compositionSaveAs(dialog.selectedFiles().first(), dtkComposerWriter::Binary);
    }
    return status;
}

bool medComposerArea::compositionSaveAs(const QString& file, dtkComposerWriter::Type type)
{
    bool status = false;
    if(file.isEmpty())
        return status;
    status = d->composer->save(file, type);
    if(status) {
        d->current_composition = file;
        this->setWindowModified(false);
    }
    return status;
}

bool medComposerArea::compositionInsert(void)
{
    QFileDialog *dialog = new QFileDialog(this, tr("Insert composition"), QDir::homePath(), QString("medinria composition (*.med)"));
    dialog->setStyleSheet("background-color: none ; color: none;");
    dialog->setAcceptMode(QFileDialog::AcceptOpen);
    dialog->setFileMode(QFileDialog::AnyFile);
    dialog->open(this, SLOT(compositionInsert(const QString&)));
    return true;
}

bool medComposerArea::compositionInsert(const QString& file)
{
    bool status = d->composer->insert(file);
    if(status)
        this->setWindowModified(true);
    return status;
}
