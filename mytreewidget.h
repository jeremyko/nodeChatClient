#ifndef MYTREEWIDGET_H
#define MYTREEWIDGET_H

#include <QTreeWidget>
#include <QTreeWidgetItem>

class MyTreeWidget: public QTreeWidget
{
    Q_OBJECT

protected:
    QTreeWidgetItem *m_pRootItem;

public:
    explicit MyTreeWidget(QWidget *parent);
    QTreeWidgetItem *AddItem( QTreeWidgetItem *pParentItem, QString strText );
    QTreeWidgetItem *GetRootItem();
    void Clear( QTreeWidgetItem *pParent );

protected:
    virtual void contextMenuEvent( QContextMenuEvent *event );
signals:
    // R-button click시에 발생하는 signal.
    void pressedRButton(MyTreeWidget *, QTreeWidgetItem *pCurItem);

public slots:
};

#endif // MYTREEWIDGET_H
