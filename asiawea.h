#ifndef ASIAWEA_H
#define ASIAWEA_H

#include <QMainWindow>
#include <QMap>
#include <QLabel>
#include "weatherData.h"
#include "worldlistwea.h"
#include<QNetworkAccessManager>
#include<QNetworkReply>

QT_BEGIN_NAMESPACE
namespace Ui {
class AsiaWea;
}
QT_END_NAMESPACE

class AsiaWea : public QMainWindow
{
    Q_OBJECT

public:
    explicit AsiaWea(QWidget *parent = nullptr);
    ~AsiaWea();
    //得到返回值按钮
    WorldListWea* ListBtn = new WorldListWea;
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
private slots:
    void GetData(QString);
signals:
    void FirstBtn();
private:
    void onReplied(QNetworkReply* reply);
    private:
    Ui::AsiaWea *ui;

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
#endif // ASIAWEA_H
