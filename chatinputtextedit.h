#ifndef CHATINPUTTEXTEDIT_H
#define CHATINPUTTEXTEDIT_H

#include <QPlainTextEdit>

class ChatInputTextEdit : public QPlainTextEdit
{
    Q_OBJECT
public:
    //ChatInputTextEdit(QWidget *parent =0);
    ChatInputTextEdit(QWidget *parent =0);
    //virtual ~ChatInputTextEdit ();
signals:
    void sigEnterKeyed();
protected:
    virtual void keyPressEvent(QKeyEvent *e);

};



#endif // CHATINPUTTEXTEDIT_H
