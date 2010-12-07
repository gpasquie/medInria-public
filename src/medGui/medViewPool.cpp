/* medDropSite.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Fri Feb 19 17:41:43 2010 (+0100)
 * Version: $Id$
 * Last-Updated: Mon Feb 22 21:15:03 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 18
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "medViewPool.h"
#include <medCore/medMessageController.h>
#include <medCore/medAbstractView.h>

#include <dtkCore/dtkAbstractProcess.h>
#include <dtkCore/dtkAbstractProcessFactory.h>

#include "medCore/medMessageController.h"

class medViewPoolPrivate
{
public:
    QList<medAbstractView *>                 views;
    QMap<medAbstractView*, dtkAbstractData*> viewData;
    QHash<QString, QString>                  propertySet;
};


medViewPool::medViewPool(void) : d (new medViewPoolPrivate)
{
    connect(this,SIGNAL(showInfo(QObject*,const QString&,unsigned int)),
            medMessageController::instance(),SLOT(showInfo(QObject*,const QString&,unsigned int)));
    connect(this,SIGNAL(showError(QObject*,const QString&,unsigned int)),
            medMessageController::instance(),SLOT(showError(QObject*,const QString&,unsigned int)));
}

medViewPool::~medViewPool(void)
{
	if (medAbstractView *refView=this->daddy()) {
        foreach (medAbstractView *view, d->views) {
            refView->unlink(view);
            refView->linkPosition  (view, false);
            refView->linkCamera    (view, false);
            refView->linkWindowing (view, false);
            
            disconnect (view, SIGNAL (becomeDaddy(bool)),             this, SLOT (onViewDaddy(bool)));
            disconnect (view, SIGNAL (syncPosition(bool)),            this, SLOT (onViewSyncPosition(bool)));
            disconnect (view, SIGNAL (syncCamera(bool)),              this, SLOT (onViewSyncCamera(bool)));
            disconnect (view, SIGNAL (syncWindowing(bool)),           this, SLOT (onViewSyncWindowing(bool)));
            disconnect (view, SIGNAL (reg(bool)),                     this, SLOT (onViewReg(bool)));		
        }
	}
    delete d;
    d = NULL;
}

void medViewPool::appendView (medAbstractView *view)
{
    if (!view || d->views.contains (view))
        return;
    
    if (d->views.count()==0) { // view will become daddy      
        view->setProperty ("Daddy", "true");
    }
    
    d->views.append (view);
    
    connect (view, SIGNAL (propertySet(const QString&, const QString&)), this, SLOT (onViewPropertySet(const QString&, const QString &)));
    connect (view, SIGNAL (becomeDaddy(bool)),             this, SLOT (onViewDaddy(bool)));
    connect (view, SIGNAL (syncPosition(bool)),            this, SLOT (onViewSyncPosition(bool)));
    connect (view, SIGNAL (syncCamera(bool)),              this, SLOT (onViewSyncCamera(bool)));
    connect (view, SIGNAL (syncWindowing(bool)),           this, SLOT (onViewSyncWindowing(bool)));
    connect (view, SIGNAL (reg(bool)),                     this, SLOT (onViewReg(bool)));
    
    // set properties
    QHashIterator<QString, QString> it(d->propertySet);
    while (it.hasNext()) {
        it.next();
        view->setProperty (it.key(), it.value());
    }
    
    if (medAbstractView *refView = this->daddy())
        if (refView != view)
            refView->link (view);
}

void medViewPool::removeView (medAbstractView *view)
{
    if (!view || !d->views.contains (view))
        return;
    
    // look if a view is a daddy
    medAbstractView *refView = this->daddy();
    
    if (refView) {
        if (refView==view) { // we are daddy, we need to unlink, find a new daddy, and link again (if needed)
            
            // unlink
            QList<medAbstractView *>::iterator it = d->views.begin();
            for( ; it!=d->views.end(); it++) {
                refView->unlink ((*it));
                refView->linkPosition  ((*it), false);
                refView->linkCamera    ((*it), false);
                refView->linkWindowing ((*it), false);
            }
            
            // change daddy
            it = d->views.begin();
            for( ; it!=d->views.end(); it++)
                if ((*it)!=refView && (*it)->property ("Daddy")=="false") {
                    (*it)->setProperty ("Daddy", "true");
                    break;
                }
            
            medAbstractView *oldDaddy = refView;
            oldDaddy->setProperty ("Daddy", "false");
        }
        else { // we are not daddy, just unlink
            refView->unlink (view);
            refView->linkPosition  (view, false);
            refView->linkCamera    (view, false);
            refView->linkWindowing (view, false);	    
        }
    }
    
    disconnect (view, SIGNAL (propertySet(const QString&, const QString&)), this, SLOT (onViewPropertySet(const QString&, const QString &)));
    disconnect (view, SIGNAL (becomeDaddy(bool)),             this, SLOT (onViewDaddy(bool)));
    disconnect (view, SIGNAL (syncPosition(bool)),            this, SLOT (onViewSyncPosition(bool)));
    disconnect (view, SIGNAL (syncCamera(bool)),              this, SLOT (onViewSyncCamera(bool)));
    disconnect (view, SIGNAL (syncWindowing(bool)),           this, SLOT (onViewSyncWindowing(bool)));
    disconnect (view, SIGNAL (reg(bool)),                     this, SLOT (onViewReg(bool)));
    
    d->views.removeOne (view);
}

medAbstractView *medViewPool::daddy (void)
{
    QList<medAbstractView *>::iterator it = d->views.begin();
    for( ; it!=d->views.end(); it++) {
        if ((*it)->property ("Daddy")=="true")
            return (*it);
    }
    return NULL;
}

void medViewPool::onViewDaddy (bool daddy)
{
    if (medAbstractView *view = dynamic_cast<medAbstractView *>(this->sender())) {
        
        if (daddy) { // view wants to become daddy
            
            medAbstractView *refView = this->daddy();
            
            if (refView) { // if daddy, then unlink first
                QList<medAbstractView *>::iterator it = d->views.begin();
                for( ; it!=d->views.end(); it++) {
                    refView->unlink((*it));
                    refView->linkPosition  ((*it), false);
                    refView->linkCamera    ((*it), false);
                    refView->linkWindowing ((*it), false);
                }
            }
            
            // tell all views that they are not daddys
            QList<medAbstractView *>::iterator it = d->views.begin();
            for( ; it!=d->views.end(); it++)
                (*it)->setProperty ("Daddy", "false");
            
            // tell the sender it is now daddy
            view->setProperty ("Daddy", "true");
            
            // restore the previous data (if any)
            if ( d->viewData[view] ) {
                view->setData (d->viewData[view]);
                d->viewData[view] = NULL;
                view->update();
            }
			
            // link with new daddy
            it = d->views.begin();
            for( ; it!=d->views.end(); it++)
                view->link((*it));
            
        }
        else { // view does not want to become daddy
            if (view==this->daddy()) {
                QList<medAbstractView *>::iterator it = d->views.begin();
                for( ; it!=d->views.end(); it++) {
                    view->unlink((*it));
                    view->linkPosition  ((*it), false);
                    view->linkCamera    ((*it), false);
                    view->linkWindowing ((*it), false);			
                }
            }
            
            view->setProperty ("Daddy", "false");
        }
    }
}

void medViewPool::onViewSyncPosition (bool value)
{
    if (medAbstractView *view = dynamic_cast<medAbstractView *>(this->sender())) {
        
        if (view->property ("Daddy")=="false") {
			if (medAbstractView *refView = this->daddy() ) {
                refView->linkPosition (view, value);
                view->update();
			}
        }
        
    }
}

void medViewPool::onViewSyncCamera (bool value)
{
    if (medAbstractView *view = dynamic_cast<medAbstractView *>(this->sender())) {
        
        if (view->property ("Daddy")=="false") {
			if (medAbstractView *refView = this->daddy() ) {
                refView->linkCamera (view, value);
                view->update();
			}
        }
        
    }
}

void medViewPool::onViewSyncWindowing (bool value)
{
    if (medAbstractView *view = dynamic_cast<medAbstractView *>(this->sender())) {
        
        if (view->property ("Daddy")=="false") {
			if (medAbstractView *refView = this->daddy() ) {
                refView->linkWindowing (view, value);
                view->update();
			}
        }
        
    }
}

void medViewPool::onViewReg(bool value)
{
	if (medAbstractView *view = dynamic_cast<medAbstractView *>(this->sender())) {
        
        if (value) {
            medAbstractView *refView = this->daddy();
            
            if (refView==view) // do not register the view with itself
                return;
            
            if (refView) {
                
                dtkAbstractData *data1 = static_cast<dtkAbstractData *>(refView->data());
                dtkAbstractData *data2 = static_cast<dtkAbstractData *>(view->data());
                
                if (data1!=data2) {// do not register the same data
                    
                    dtkAbstractProcess *process = dtkAbstractProcessFactory::instance()->create("itkProcessRegistration");
                    if (!process)
                        return;
                    process->setInput (data1, 0);
                    process->setInput (data2, 1);
                    if (process->run()==0) {
                        dtkAbstractData *output = process->output();
                        d->viewData[view] = data2;
                        view->setData (output);
                        view->update();
                        emit showInfo (this, tr ("Automatic registration successful"),3000);
                    }
                    else {
                        emit showError(this, tr  ("Automatic registration failed"),3000);
                    }
                }
                
            }
        }
        else {
            
            // restore the previous data (if any)
            if ( d->viewData[view] ) {
                view->setData (d->viewData[view]);
                d->viewData[view] = NULL;
                view->update();
            }
        }
    }
}

void medViewPool::onViewPropertySet (const QString &key, const QString &value)
{
    // property that we do not want to synchronize
    if (key=="Daddy" ||
        key=="PositionLinked" ||
        key=="CameraLinked" ||
        key=="WindowingLinked" ||
        key=="Orientation")
        return;
    
    d->propertySet[key] = value;
    
    foreach (medAbstractView *lview, d->views) {
        if (lview!=this->sender()) {
            lview->blockSignals (true);
            lview->setProperty (key, value);
            lview->update();
            lview->blockSignals (false);
        }
    }
}

int medViewPool::count (void)
{
    return d->views.count();
}