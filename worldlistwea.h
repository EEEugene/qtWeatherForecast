#ifndef WORLDLISTWEA_H
#define WORLDLISTWEA_H

#include <QMainWindow>
#include <QLabel>
#include <QMap>
#include <QMainWindow>
#include "weatherData.h"
#include<QNetworkAccessManager>
#include<QNetworkReply>
#include<QListWidget>
//#include "dialog.h"
#include "citywithtemp.h"
//#include "mainwindow.h"

namespace Ui {
class WorldListWea;
}

class WorldListWea : public QMainWindow
{
    Q_OBJECT

public:
    explicit WorldListWea(QWidget *parent = nullptr);
    ~WorldListWea();
//    Dialog* dlg = new Dialog;
    QVector<QString> cityList;
protected:
    void contextMenuEvent(QContextMenuEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void getWeatherInfo(QString cityName);
    void getWorldWeather(QString wcity);
    void parseJson(QByteArray& byteArray);
    void updateUI();
    void appearButton();
//    void initWidget();
//    void appearList();

signals:
    void BackButton();
    void setData(QString text);

private slots:
//    void on_Refresh_clicked();
    void on_Back_clicked();
    void on_EuBTn_clicked();
    void on_AfBtn_clicked();
    void on_NaBTn_clicked();
    void on_OaBTn_clicked();
//    void on_pushButton_clicked();
    void on_pushButton_2_clicked();

    void on_AsiaBtn_clicked();
    void on_listWidget_itemDoubleClicked();
    void on_listWidget_itemClicked();
    void on_hide_clicked();
    void getCity(QString);
    void GetData(QString);

private:
    void onReplied(QNetworkReply* reply);
    Ui::WorldListWea *ui;
    QMenu* mExitMenu;   // 退出菜单
    QAction* mExitAct;  // 菜单项（退出）
    QPoint mOffset;     //窗口移动时，鼠标与左上角的偏移
    QNetworkAccessManager* mNetAccessManager;
    void AddItem(QString str1,QString str2,const QString& pic);
    Today mToday;
    Day mDay[6];
};

#endif // WORLDLISTWEA_H
