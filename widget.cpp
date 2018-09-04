#include "widget.h"
#include "ui_widget.h"

#include "libraw/libraw.h"

#include <QFileDialog>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    setWindowTitle("CR2 Viewer");

    _stashpath = QDir::homePath() + "/cr2viewer/stash";

    _stashlabel = new QLabel(ui->thumbnailLabel);
    QPixmap pix;
    pix.load(":/icons/heart");
    _stashlabel->setPixmap(pix);
    _stashlabel->setScaledContents(true);
    _stashlabel->setStyleSheet("background-color: transparent");
    _stashlabel->setFixedSize(32, 32);
    _stashlabel->hide();

    _canmoveback = false;
    _canmovenext = false;

    changeDirectory(QDir::current().path());
}

Widget::~Widget()
{
    delete _stashlabel;
    delete ui;
}

void Widget::on_currentDirectoryButton_clicked()
{
    QString path = QFileDialog::getExistingDirectory(this, "Change current directory", ui->currentDirectoryLineEdit->text());
    if(!path.isEmpty())
    {
        changeDirectory(path);
    }
}

void Widget::changeDirectory(QString path)
{
    ui->currentDirectoryLineEdit->setText(path);
    ui->filesListWidget->clear();
    ui->filesListWidget->addItems(QDir(path).entryList(QStringList() << "*.CR2"));

    ui->thumbnailLabel->clear();
    _stashlabel->hide();
    ui->stashButton->setEnabled(false);
    ui->removeButton->setEnabled(false);
    ui->homeButton->setEnabled(ui->filesListWidget->count() > 0);
    ui->backButton->setEnabled(false);
    ui->nextButton->setEnabled(false);
    ui->rotateButton->setEnabled(false);
    _currentpath = path;
    _currentfile.clear();
    _canmoveback = false;
    _canmovenext = false;
}

void Widget::resizeLabel(const QSize &imagesize)
{
    QSize widgetsize = ui->thumbnailWidget->size();
    qreal ratio = static_cast<qreal>(imagesize.width()) / static_cast<qreal>(imagesize.height());
    QSize labelsize;
    labelsize.setWidth(widgetsize.width());
    qreal height = labelsize.width() / ratio;
    if(height > widgetsize.height())
    {
        height = widgetsize.height();
        labelsize.setWidth(static_cast<int>(height * ratio));
    }

    labelsize.setHeight(static_cast<int>(height));
    QRect widgetrect(QPoint(0, 0), widgetsize);
    QRect labelrect(QPoint(0, 0), labelsize);
    labelrect.moveCenter(widgetrect.center());
    ui->thumbnailLabel->setGeometry(labelrect);
    _stashlabel->move(12, 12);
}

bool Widget::moveBack()
{
    if(_canmoveback)
    {
        ui->filesListWidget->setCurrentRow(ui->filesListWidget->row(ui->filesListWidget->currentItem()) - 1);
        return true;
    }
    return false;
}

bool Widget::moveNext()
{
    if(_canmovenext)
    {
        ui->filesListWidget->setCurrentRow(ui->filesListWidget->row(ui->filesListWidget->currentItem()) + 1);
        return true;
    }
    return false;
}

void Widget::rotate()
{
    const QPixmap * pix = ui->thumbnailLabel->pixmap();
    if(pix != nullptr)
    {
        QImage image = pix->toImage().transformed(QMatrix().rotate(-90.0));
        ui->thumbnailLabel->setPixmap(QPixmap::fromImage(image));
        resizeLabel(image.size());
    }
}

void Widget::resizeEvent(QResizeEvent *)
{
    if(ui->thumbnailLabel->pixmap() != nullptr)
    {
        resizeLabel(ui->thumbnailLabel->pixmap()->size());
    }
}

void Widget::on_stashButton_clicked()
{
    if(QDir().mkpath(_stashpath))
    {
        QString source = _currentpath + "/" + _currentfile;
        QString target = _stashpath + "/" + _currentfile;
        QFile::copy(source, target);
        if(!moveNext())
        {
            _stashlabel->show();
            ui->stashButton->setEnabled(false);
            ui->removeButton->setEnabled(true);
        }
    }
}
void Widget::on_removeButton_clicked()
{
    if(QFile().remove(_stashpath + "/" + _currentfile))
    {
        _stashlabel->hide();
        ui->stashButton->setEnabled(true);
        ui->removeButton->setEnabled(false);
    }
}

void Widget::on_fullScreenButton_toggled(bool checked)
{
    ui->currentDirectoryWidget->setVisible(!checked);
    ui->filesListWidget->setVisible(!checked);
    if(checked)
    {
        showFullScreen();
    }
    else
    {
        showNormal();
    }
}
void Widget::on_rotateButton_clicked()
{
    rotate();
}

void Widget::on_homeButton_clicked()
{
    if(ui->filesListWidget->count() > 0)
    {
        ui->filesListWidget->setCurrentRow(0);
    }
}
void Widget::on_backButton_clicked()
{
    moveBack();
}
void Widget::on_nextButton_clicked()
{
    moveNext();
}

void Widget::on_filesListWidget_currentItemChanged(QListWidgetItem *current, QListWidgetItem *)
{
    if(current != nullptr)
    {
        _currentfile = current->text();
        QString path = _currentpath + "/" + _currentfile;

        LibRaw iProcessor;
        if(iProcessor.open_file(path.toLatin1().data()) == LIBRAW_SUCCESS)
        {
            if(iProcessor.unpack_thumb() == LIBRAW_SUCCESS)
            {
                libraw_thumbnail_t tdata = iProcessor.imgdata.thumbnail;
                QByteArray data(tdata.thumb, static_cast<int>(tdata.tlength));
                QSize imagesize(tdata.twidth, tdata.theight);
                QImage image(imagesize, QImage::Format_RGB32);
                image.loadFromData(data, "JPG");
                resizeLabel(imagesize);
                ui->thumbnailLabel->setPixmap(QPixmap::fromImage(image));

                bool stashed = QDir(_stashpath).exists(_currentfile);
                _stashlabel->setVisible(stashed);
                ui->stashButton->setEnabled(!stashed);
                ui->removeButton->setEnabled(stashed);

                int row = ui->filesListWidget->row(current);
                int rows =  ui->filesListWidget->count();
                _canmoveback = (row > 0);
                _canmovenext = (row < (rows-1));
                ui->backButton->setEnabled(_canmoveback);
                ui->nextButton->setEnabled(_canmovenext);

                ui->rotateButton->setEnabled(true);
            }
        }
    }
}

