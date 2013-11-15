#include "medComposerSceneNodeEditor.h"
#include "medAbstractProcess.h"

#include "dtkComposer/dtkComposerSceneNode.h"
#include "dtkComposer/dtkComposerSceneNodeEditor_p.h"
#include "dtkComposer/dtkComposerNodeLeafProcess.h"


medComposerSceneNodeEditor::medComposerSceneNodeEditor(QWidget *parent) : dtkComposerSceneNodeEditor(parent)
{
    connect(d->select_implementation, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(showProcessWidget(const QString&)));
}

void medComposerSceneNodeEditor::showProcessWidget(const QString&)
{           
    if (dtkComposerNodeLeafProcess *process_node = dynamic_cast<dtkComposerNodeLeafProcess *>(d->node->wrapee()))
    {
        if( medAbstractProcess* process = dynamic_cast<medAbstractProcess*>(process_node->process()) )
        {
            QWidget * process_widget = process->widget();
            if(process_widget)
              this->layout()->addWidget(process_widget);
        }
    }

}
