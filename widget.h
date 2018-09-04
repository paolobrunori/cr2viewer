#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QLabel>
#include <QListWidgetItem>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();

protected:
    void resizeEvent(QResizeEvent *event);

private slots:
    void on_currentDirectoryButton_clicked();

    void on_stashButton_clicked();
    void on_removeButton_clicked();
    void on_fullScreenButton_toggled(bool checked);
    void on_rotateButton_clicked();
    void on_homeButton_clicked();
    void on_backButton_clicked();
    void on_nextButton_clicked();

    void on_filesListWidget_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

private:
    Ui::Widget *ui;
    QLabel * _stashlabel;

    QString _currentpath;
    QString _currentfile;
    QString _stashpath;

    bool _canmoveback;
    bool _canmovenext;

    void changeDirectory(QString path);
    void resizeLabel(const QSize & imagesize);
    bool moveBack();
    bool moveNext();

    void rotate();
};

#endif // WIDGET_H
