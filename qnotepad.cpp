#include "qnotepad.h"
#include "ui_qnotepad.h"
#include <QMessageBox>
#include <QCloseEvent>
#include <QFontDialog>
#include <QSettings>
#include <QFile>
#include <QTextStream>
#include <QFileInfo>
#include <QFileDialog>

QNotePad::QNotePad(const QString &fileName, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::QNotePad),
    m_fileName (fileName)
{
    ui->setupUi(this);
    setTitle(fileName);
    if (!fileName.isEmpty())
        loadFile (m_fileName);
    QSettings s;
    ui->textEdit->setFont(s.value("fonts", QApplication::font()).value<QFont>());
    connect (ui->actionClose, SIGNAL(triggered()),
             this, SLOT(close()));

    connect (ui->actionUndo, SIGNAL(triggered()),
             ui->textEdit, SLOT(undo()));
    connect (ui->textEdit, SIGNAL(undoAvailable(bool)),
             ui->actionUndo, SLOT(setEnabled(bool)));
    ui->actionUndo->setEnabled(false);

    connect (ui->actionRedo, SIGNAL(triggered()),
             ui->textEdit, SLOT(redo()));
    connect (ui->textEdit, SIGNAL(redoAvailable(bool)),
             ui->actionRedo, SLOT(setEnabled(bool)));
    ui->actionRedo->setEnabled(false);

    connect (ui->actionCut, SIGNAL(triggered()),
             ui->textEdit, SLOT(cut()));
    connect (ui->textEdit, SIGNAL(copyAvailable(bool)),
             ui->actionCut, SLOT(setEnabled(bool)));
    ui->actionCut->setEnabled(false);

    connect (ui->actionCopy, SIGNAL(triggered()),
             ui->textEdit, SLOT(copy()));
    connect (ui->textEdit, SIGNAL(copyAvailable(bool)),
             ui->actionCopy, SLOT(setEnabled(bool)));
    ui->actionCopy->setEnabled(false);

    connect (ui->actionPaste, SIGNAL(triggered()),
             ui->textEdit, SLOT(paste()));
}

QNotePad::~QNotePad()
{
    delete ui;
}

QString QNotePad::getFileName() const
{
    return m_fileName;
}

void QNotePad::on_actionNew_triggered()
{
    QNotePad *np = new QNotePad();
    np->setAttribute(Qt::WA_DeleteOnClose);
    np->show();
}

void QNotePad::on_textEdit_textChanged()
{
    setWindowModified(true);
}

void QNotePad::loadFile(const QString &fileName)
{
    QFile f(fileName);

    if (!f.open(QIODevice::ReadOnly|QIODevice::Text)) {
        QMessageBox::critical(this, "Error",
                              "There was an error reading "+fileName+". Ensure you have read permissions on the file.");
        setTitle(QString());
        return;
    }

    QTextStream s(&f);
    ui->textEdit->setPlainText(s.readAll());
    f.close();
    setWindowModified(false);
    m_fileName = fileName;
    setTitle(fileName);
}

bool QNotePad::saveFile(const QString &fileName)
{
    QFile f(fileName);

    if (!f.open(QIODevice::WriteOnly|QIODevice::Text)) {
        QMessageBox::critical(this, "Error",
                              "There was an error saving to:\n"+fileName+"\nEnsure you have proper permissions.");
        return false;
    }

    QTextStream s(&f);
    s << ui->textEdit->toPlainText();
    f.close();
    setWindowModified(false);
    m_fileName = fileName;
    setTitle (fileName);
    return true;
}

void QNotePad::setTitle (const QString &fileName)
{
    setWindowTitle(QString("%1[*] - %2")
                   .arg(fileName.isEmpty() ? "Untitled":QFileInfo(fileName).fileName())
                   .arg(QApplication::applicationName()));
}

void QNotePad::closeEvent(QCloseEvent *e)
{
    if (isWindowModified()) {
        QMessageBox::StandardButton btn = QMessageBox::warning(this, "Are you sure?",
                         "You are about to close an unsaved document. All changes will be lost if you continue.",
                         QMessageBox::Save|QMessageBox::Discard|QMessageBox::Cancel,
                         QMessageBox::Cancel);
        switch (btn) {
        case QMessageBox::Save:
            if (on_actionSave_triggered())
                e->accept();
            else
                e->ignore();
            break;
        case QMessageBox::Discard:
            e->accept(); break;
        case QMessageBox::Cancel:
            e->ignore(); break;
        default:
            e->ignore();
        }
    }
    else {
        e->accept();
    }
}

void QNotePad::on_actionSelect_Font_triggered()
{
    bool ok;
    QFont newFont = QFontDialog::getFont(&ok, ui->textEdit->font(), this);
    if (ok) {
        QSettings s;
        s.setValue("fonts", newFont);
        ui->textEdit->setFont(newFont);
    }
}

void QNotePad::on_actionOpen_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this);
    if (isWindowModified() || !getFileName().isEmpty()) {
        QNotePad *np = new QNotePad(fileName);
        np->setAttribute(Qt::WA_DeleteOnClose);
        if (!np->getFileName().isEmpty()) {
            np->show();
        }
        else {
            np->close();
        }
    }
    else {
        loadFile (fileName);
    }
}

bool QNotePad::on_actionSave_triggered()
{
    if (!isWindowModified()) {
        return true;
    }

    if (m_fileName.isEmpty())
        return on_actionSave_As_triggered();
    else
        return saveFile (m_fileName);
}

bool QNotePad::on_actionSave_As_triggered()
{
    QString newFileName = QFileDialog::getSaveFileName(this);
    if (!newFileName.isEmpty())
        return saveFile (newFileName);
    else
        return false;
}
