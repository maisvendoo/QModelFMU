#include    "app.h"

#include    "qmodelfmu.h"

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
TestApp::TestApp(int &argc, char **argv) : QCoreApplication(argc, argv)
  , model(new QModelFMU)
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

    return QCoreApplication::exec();
}
