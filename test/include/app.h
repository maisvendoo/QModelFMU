#ifndef     APP_H
#define     APP_H

#include    <QCoreApplication>

class       QModelFMU;
class       QTimer;

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
class TestApp : public QCoreApplication
{
    Q_OBJECT

public:

    explicit TestApp(int &argc, char **argv);

    virtual ~TestApp();

    int exec(QString path);

private:

    QModelFMU   *model;

    QTimer      *timer;

    int         interval;

    double      t_start;

    double      t_stop;

    double      dt;

    double      t;

private slots:

    void process();
};

#endif // APP_H
