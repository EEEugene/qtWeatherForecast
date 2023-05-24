#ifndef CITYWITHTEMP_H
#define CITYWITHTEMP_H

#include <QWidget>

namespace Ui {
class cityWithTemp;
}

class cityWithTemp : public QWidget
{
    Q_OBJECT

public:
    explicit cityWithTemp(QWidget *parent = nullptr);
    ~cityWithTemp();
    void SetData(QString str1,QString str2,const QString& pic);

protected:
    void mousePressEvent(QMouseEvent* event);

private:
    Ui::cityWithTemp *ui;
};

#endif // CITYWITHTEMP_H
