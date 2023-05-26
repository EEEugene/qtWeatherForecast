#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QLabel>
#include <QMap>
#include <QMainWindow>
#include "weatherData.h"
#include "worldlistwea.h"
#include "asiawea.h"
#include<QNetworkAccessManager>
#include<QNetworkReply>
#include <citywithtemp.h>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();
   //跳转五大洲的按钮
   WorldListWea* List = new WorldListWea;
   AsiaWea* World = new AsiaWea;
   cityWithTemp* city = new cityWithTemp;

protected:
    void contextMenuEvent(QContextMenuEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    //获取天气数据、解析Json、更新UI
    void getWeatherInfo(QString cityName);
    void parseJson(QByteArray& byteArray);
    void updateUI();
    //重写父类的eventFilter方法
    bool eventFilter(QObject* watched,QEvent* event);
    void paintHighCurve();
    void paintLowCurve();
    void weaType();

signals:
    void sendTemp(QString city,QString temp,QString type);

private slots:
    void on_btnSearch_clicked();
    void receiveData(QString str);

private:
    void onReplied(QNetworkReply* reply);

private:
    Ui::MainWindow* ui;
    QMenu* mExitMenu;   // 退出菜单
    QAction* mExitAct;  // 菜单项（退出）
    QPoint mOffset;     //窗口移动时，鼠标与左上角的偏移
    QNetworkAccessManager* mNetAccessManager;

    Today mToday;
    Day mDay[6];

    //星期和日期
    QList<QLabel*> mWeekList;
    QList<QLabel*> mDateList;

    //天气和图标
    QList<QLabel*> mTypeList;
    QList<QLabel*> mTypeIconList;

    // 质量指数
    QList<QLabel*> mAqiList;

    //风力风向
    QList<QLabel*> mFxList;
    QList<QLabel*> mFlList;

    QMap<QString,QString>mTypeMap;

};
#endif  // MAINWINDOW_H
