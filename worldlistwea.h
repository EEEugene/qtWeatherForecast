#ifndef WORLDLISTWEA_H
#define WORLDLISTWEA_H

#include <QMainWindow>
#include <QLabel>
#include <QMap>
#include <QMainWindow>
#include<QNetworkAccessManager>
#include<QNetworkReply>
#include<QListWidget>
#include "citywithtemp.h"

namespace Ui {
class WorldListWea;
}

class WorldListWea : public QMainWindow
{
    Q_OBJECT

public:
    explicit WorldListWea(QWidget *parent = nullptr);
    ~WorldListWea();
    QString updateCity;
    QString updateTemp;
    QString updateType;
    QList<QString> cityList;
    QList<QString> tempList;
    QList<QString> typeList;
    QList<QString> clickedCity;

protected:
    void contextMenuEvent(QContextMenuEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void setState();
    void asiaCity();
    void euCity();
    void afCity();
    void naCity();
    void oaCity();

signals:
    void BackButton();
    void setData(QString text);
    void sendIsland(QString);

private slots:
    void on_pushButton_2_clicked();
    void on_listWidget_itemDoubleClicked(QListWidgetItem *item);
    void on_stateWidget_itemDoubleClicked();
    void on_listBtn_clicked();
    void getTemp(QString temp,QString city,QString type);

private:
    Ui::WorldListWea *ui;
    QMenu* mExitMenu;   // 退出菜单
    QAction* mExitAct;  // 菜单项（退出）
    QPoint mOffset;     //窗口移动时，鼠标与左上角的偏移
    QNetworkAccessManager* mNetAccessManager;
    void AddItem(QString city, QString temp, QString pic);
};

#endif // WORLDLISTWEA_H
