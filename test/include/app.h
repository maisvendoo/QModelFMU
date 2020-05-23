#ifndef     APP_H
#define     APP_H

#include    <QCoreApplication>

class QModelFMU;

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
};

#endif // APP_H
