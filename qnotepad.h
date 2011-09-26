#ifndef QNOTEPAD_H
#define QNOTEPAD_H

#include <QMainWindow>

namespace Ui {
    class QNotePad;
}

class QNotePad : public QMainWindow
{
    Q_OBJECT

public:
    explicit QNotePad(const QString &fileName=QString(), QWidget *parent = 0);
    ~QNotePad();

    QString getFileName() const;

private slots:
    void on_actionNew_triggered();
    void on_textEdit_textChanged();
    void on_actionSelect_Font_triggered();
    void on_actionOpen_triggered();
    bool on_actionSave_triggered();
    bool on_actionSave_As_triggered();

protected:
    void closeEvent (QCloseEvent *e);

private:
    Ui::QNotePad *ui;
    QString m_fileName;

    void loadFile(const QString &fileName);
    bool saveFile(const QString &fileName);
    void setTitle(const QString &fileName);
};

#endif // QNOTEPAD_H
