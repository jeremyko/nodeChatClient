#include "mytreewidget.h"
#include <QMouseEvent>


MyTreeWidget::MyTreeWidget(QWidget *parent) :
    QTreeWidget(parent)
{
    m_pRootItem = NULL;

    //while ( parent && !parent->inherits("CServerDemoView" ) )
    //    parent = parent->parentWidget();

    connect(this, SIGNAL(pressedRButton(MyTreeWidget*, QTreeWidgetItem*)), parent,
            SLOT(onTreeRButtonPressed(MyTreeWidget*, QTreeWidgetItem*)));
}

void MyTreeWidget::contextMenuEvent( QContextMenuEvent *event )
{
    QTreeWidget::contextMenuEvent( event );
    QTreeWidgetItem *pCurItem = currentItem();
    emit pressedRButton( this, pCurItem );
}

QTreeWidgetItem *MyTreeWidget::AddItem( QTreeWidgetItem *pParentItem, QString strText )
{
    QTreeWidgetItem *pNewItem = NULL;
    if ( m_pRootItem == NULL )
    {
        pNewItem = new QTreeWidgetItem(this);
        m_pRootItem = pNewItem;
        m_pRootItem->setText( 0, strText );
    }
    else
    {
        // addChild()를 별도로 할 필요 없음. pParentItem->childCount() 해 보면 child로 들어가 있음.
        pNewItem = new QTreeWidgetItem(pParentItem);
        pNewItem->setText( 0, strText );
    }
    return pNewItem;
}

QTreeWidgetItem *MyTreeWidget::GetRootItem()
{
    return m_pRootItem;
}
/**
  headerItem() 의 모든 child들을 recursive하게 삭제해 준다.
  */
void MyTreeWidget::Clear( QTreeWidgetItem *pParent )
{
    if ( pParent == NULL )
        return;
    int cntChild = pParent->childCount();
    int i;
    for (i = cntChild-1;i >= 0;i--)
    {
        QTreeWidgetItem *pChild = pParent->child(i);
        pParent->removeChild(pChild);
        if ( pChild->childCount() > 0 )
            Clear( pChild );
        else
            delete pChild;
    }
    if ( pParent == m_pRootItem )
        m_pRootItem = NULL;
    delete pParent;
}
