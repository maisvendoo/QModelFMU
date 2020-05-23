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

    bool load(QString path);

    void close();

private:

    /// Temporary path for unzip of *.fmu
    QString                                 tmpPath;

    jm_callbacks                            callbacks;

    /// Current context
    fmi_import_context_t                    *context;

    fmi_version_enu_t                       version;

    fmi2_import_t                           *fmu;

    fmi2_callback_functions_t               callback_functions;

    bool load_fmi2(QString tmpPath);

    void close_fmi2();
};

#endif // QMODELFMU_H
