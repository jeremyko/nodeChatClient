#include "chatinputtextedit.h"



ChatInputTextEdit::ChatInputTextEdit(QWidget *parent):
    QPlainTextEdit(parent)
{
    //
}

//ChatInputTextEdit::~ChatInputTextEdit () {}

void ChatInputTextEdit::keyPressEvent ( QKeyEvent * event )
{
    if( event->key() == Qt::Key_Return )
    {
        //QPlainTextEdit::keyPressEvent( event );

        emit sigEnterKeyed();
    }
    else
    {
        QPlainTextEdit::keyPressEvent( event );
    }
}

