#ifndef     QMODELFMU_H
#define     QMODELFMU_H

#include    <QObject>

#include    <fmilib.h>
#include    <JM/jm_portability.h>

#include    <qmodelfmu-export.h>

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
class QMODELFMU_EXPORT  QModelFMU : public QObject
{
    Q_OBJECT

public:

    QModelFMU(QObject *parent = Q_NULLPTR);

    ~QModelFMU();

protected:


};

#endif // QMODELFMU_H
