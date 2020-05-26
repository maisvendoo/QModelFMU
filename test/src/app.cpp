#include    "app.h"

#include    "qmodelfmu.h"

#include    <QTimer>

#include    <iostream>

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
TestApp::TestApp(int &argc, char **argv) : QCoreApplication(argc, argv)
  , model(new QModelFMU)
  , timer(new QTimer)
  , interval(100)
  , t_start(0.0)
  , t_stop(100.0)
  , dt(1e-3)
  , t(t_start)
{

}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
TestApp::~TestApp()
{

}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
int TestApp::exec(QString path)
{
    if (model == nullptr)
        return -1;

    if (!model->load(path))
        return -1;

    timer->setInterval(interval);
    connect(timer, &QTimer::timeout, this, &TestApp::process);
    timer->start();

    return QCoreApplication::exec();
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void TestApp::process()
{
    if (t >= t_stop)
    {
        timer->stop();
        return;
    }

    double tau = 0.0;

    QString debug = QString("t: %1 phi: %2 omega: %3")
            .arg(t, 10, 'f', 2)
            .arg(model->getVariable("revolute.phi").toDouble(), 6, 'f', 2)
            .arg(model->getVariable("revolute.w").toDouble(), 6, 'f', 2);

    std::cout << debug.toStdString() << std::endl;

    while (tau <= static_cast<double>(interval) / 1000.0)
    {
        if (!model->step(t, dt))
        {
            timer->stop();
            return;
        }

        tau += dt;
        t += dt;
    }
}
