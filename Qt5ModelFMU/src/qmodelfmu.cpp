#include    "qmodelfmu.h"

#include    <QDir>
#include    <QFileInfo>

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
QModelFMU::QModelFMU(QObject *parent) : QObject(parent)
  , tmpPath(QDir::tempPath())
  , callbacks(jm_callbacks())
  , context(nullptr)
  , version(fmi_version_unknown_enu)
  , fmu(nullptr)
  , callback_functions(fmi2_callback_functions_t())
  , instance_name("")
  , tmp_dir(nullptr)
  , var_list(nullptr)
{

}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
QModelFMU::~QModelFMU()
{
    close();
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void import_logger(jm_callbacks *,
                   jm_string,
                   jm_log_level_enu_t,
                   jm_string)
{

}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
bool QModelFMU::load(QString path)
{
    callbacks.malloc = malloc;
    callbacks.calloc = calloc;
    callbacks.realloc = realloc;
    callbacks.free = free;
    callbacks.logger = import_logger;
    callbacks.log_level = jm_log_level_debug;
    callbacks.context = nullptr;

    context = fmi_import_allocate_context(&callbacks);

    if (context == nullptr)
    {
        return false;
    }

    path = QDir::toNativeSeparators(path);

    QFileInfo file_info(path);
    instance_name = file_info.baseName();

    QString tmpDir = QDir::toNativeSeparators(tmpPath) + QDir::separator() + instance_name;

    tmp_dir = new QDir(tmpDir);

    if (!tmp_dir->exists())
    {
        tmp_dir->mkdir(tmpDir);
    }

    version = fmi_import_get_fmi_version(context,
                                         path.toStdString().c_str(),
                                         tmp_dir->path().toStdString().c_str());

    switch (version)
    {
    case fmi_version_1_enu:

        break;

    case fmi_version_2_0_enu:

        return load_fmi2(tmpPath);

    case fmi_version_unsupported_enu:

        return false;

    case fmi_version_unknown_enu:

        return false;
    }

    return true;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void QModelFMU::close()
{
    switch (version)
    {
    case fmi_version_1_enu:

        break;

    case fmi_version_2_0_enu:

        close_fmi2();
        break;

    case fmi_version_unsupported_enu:

        break;

    case fmi_version_unknown_enu:

        break;
    }

    fmi_import_free_context(context);

    if (tmp_dir->exists())
    {
        tmp_dir->removeRecursively();
        delete tmp_dir;
    }
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
bool QModelFMU::load_fmi2(QString tmpPath)
{
    fmu = fmi2_import_parse_xml(context,
                                tmpPath.toStdString().c_str(),
                                nullptr);

    if (!fmu)
    {
        return false;
    }

    if (fmi2_import_get_fmu_kind(fmu) == fmi2_fmu_kind_me)
    {
        return false;
    }

    callback_functions.logger = fmi2_log_forwarding;
    callback_functions.allocateMemory = calloc;
    callback_functions.freeMemory = free;
    callback_functions.componentEnvironment = fmu;

    jm_status_enu_t status = fmi2_import_create_dllfmu(fmu,
                                                       fmi2_fmu_kind_cs,
                                                       &callback_functions);

    if (status == jm_status_error)
    {
        return false;
    }

    return init_fmi2(fmu);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void QModelFMU::close_fmi2()
{
    fmi2_import_destroy_dllfmu(fmu);
    fmi2_import_free(fmu);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
bool QModelFMU::init_fmi2(fmi2_import_t *fmu)
{
    jm_status_enu_t status = fmi2_import_instantiate(fmu,
                                                     instance_name.toStdString().c_str(),
                                                     fmi2_cosimulation,
                                                     nullptr,
                                                     fmi2_false);

    if (status == jm_status_error)
    {
        return false;
    }

    fmi2_status_t fmi_status = fmi2_import_setup_experiment(fmu,
                                                            fmi2_true,
                                                            1e-6,
                                                            0.0,
                                                            false,
                                                            1.0);

    if (fmi_status != fmi2_status_ok)
    {
        return false;
    }

    fmi_status = fmi2_import_enter_initialization_mode(fmu);

    if (fmi_status != fmi2_status_ok)
    {
        return false;
    }

    fmi_status = fmi2_import_exit_initialization_mode(fmu);

    if (fmi_status != fmi2_status_ok)
    {
        return false;
    }

    var_list = fmi2_import_get_variable_list(fmu, 1);

    fmi2_import_variable_t *v = fmi2_import_get_variable_by_name(fmu, "revolute.phi");

    return true;
}
