/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medComposer.h>
#include <medComposerScene.h>
#include <medComposerStack.h>
#include <medComposerGraph.h>
#include <medComposerEvaluator.h>

#include <dtkComposer/dtkComposerCompass.h>
#include <dtkComposer/dtkComposerFactory.h>
#include <dtkComposer/dtkComposerGraph.h>
#include <dtkComposer/dtkComposerMachine.h>
#include <dtkComposer/dtkComposerPath.h>
#include <dtkComposer/dtkComposerScene.h>
#include <dtkComposer/dtkComposerStack.h>
#include <dtkComposer/dtkComposerView.h>
#include <dtkComposer/dtkComposerEvaluator.h>

class medComposerPrivate
{
public:
    medComposerScene *scene;
    medComposerStack *stack;
    medComposerGraph *graph;
    medComposerEvaluator *evaluator;
};

medComposer::medComposer(QWidget *parent): dtkComposer(parent), d(new medComposerPrivate)
{
    d->scene = new medComposerScene(this);
    d->stack = new medComposerStack;
    d->graph = new medComposerGraph;

    d->evaluator = new medComposerEvaluator;
    d->evaluator->setGraph(d->graph);

    this->view()->setScene(d->scene);
    this->setEvaluator(d->evaluator);

    d->scene->setFactory(this->factory());
    d->scene->setMachine(this->machine());
    d->scene->setStack(d->stack);
    d->scene->setGraph(d->graph);
    d->scene->setContext(this->view()->context());

    this->evaluator()->setGraph(d->graph);

    this->path()->setScene(d->scene);

    this->compass()->setScene(d->scene);

    this->setScene(d->scene);
    this->setGraph(d->graph);
}

void medComposer::setFactory(dtkComposerFactory *factory)
{
    dtkComposer::setFactory(factory);

    d->scene->setFactory(factory);
}

void medComposer::run(void)
{
    this->updateRemotes(d->scene->root());

    QtConcurrent::run(d->evaluator, &medComposerEvaluator::run, false);

    d->graph->update();
}
