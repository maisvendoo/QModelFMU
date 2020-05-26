#ifndef     QMODELFMU_H
#define     QMODELFMU_H

#include    <QObject>
#include    <QMap>
#include    <QVariant>

#include    <fmilib.h>
#include    <JM/jm_portability.h>

#include    <qmodelfmu-export.h>
#include    <fmi2-variable.h>

class       QDir;

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

    QVariant getVariable(QString name);

    bool step(double t, double dt);

private:

    /// Temporary path for unzip of *.fmu
    QString                                 tmpPath;

    jm_callbacks                            callbacks;

    /// Current context
    fmi_import_context_t                    *context;

    fmi_version_enu_t                       version;

    fmi2_import_t                           *fmu;

    fmi2_callback_functions_t               callback_functions;

    QString                                 instance_name;

    QDir                                    *tmp_dir;

    QMap<QString, fmi2_variable_t>          vars;

    bool load_fmi2(QString tmpPath);

    void close_fmi2();

    bool init_fmi2(fmi2_import_t *fmu);

    void read_vars_list(fmi2_import_t *fmu);
};

#endif // QMODELFMU_H
