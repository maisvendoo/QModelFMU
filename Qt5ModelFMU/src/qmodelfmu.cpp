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

        return load_fmi2(tmp_dir->path());

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
QVariant QModelFMU::getVariable(QString name)
{
    fmi2_variable_t var = vars.value(name, fmi2_variable_t());

    if (var.v == nullptr)
        return 0;

    fmi2_value_reference_t vr = fmi2_import_get_variable_vr(var.v);

    switch (var.type)
    {
    case fmi2_base_type_real:
        {
            fmi2_real_t value;
            fmi2_status_t status = fmi2_import_get_real(fmu, &vr, 1, &value);

            if (status == fmi2_status_error)
                return 0;

            return value;
        }

    case fmi2_base_type_int:
    case fmi2_base_type_enum:
        {
            fmi2_integer_t value;
            fmi2_status_t status = fmi2_import_get_integer(fmu, &vr, 1, &value);

            if (status == fmi2_status_error)
                return 0;

            return value;
        }

    case fmi2_base_type_bool:
        {
            fmi2_boolean_t value;
            fmi2_status_t status = fmi2_import_get_boolean(fmu, &vr, 1, &value);

            if (status == fmi2_status_error)
                return 0;

            return value;
        }

    case fmi2_base_type_str:
        {
            fmi2_string_t value;
            fmi2_status_t status = fmi2_import_get_string(fmu, &vr, 1, &value);

            if (status == fmi2_status_error)
                return 0;

            return value;
        }
    }

    return 0;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
bool QModelFMU::step(double t, double dt)
{
    return fmi2_import_do_step(fmu, t, dt, fmi2_true) == fmi2_status_ok;
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

    read_vars_list(fmu);    

    return true;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void QModelFMU::read_vars_list(fmi2_import_t *fmu)
{
    fmi2_import_variable_list_t *var_list = fmi2_import_get_variable_list(fmu, 1);

    if (var_list == nullptr)
        return;

    size_t vlist_size = fmi2_import_get_variable_list_size(var_list);

    for (size_t i = 0; i < vlist_size; ++i)
    {
        fmi2_variable_t var;

        var.v = fmi2_import_get_variable(var_list, i);
        var.name = fmi2_import_get_variable_name(var.v);
        var.description = fmi2_import_get_variable_description(var.v);
        var.type = fmi2_import_get_variable_base_type(var.v);

        vars.insert(var.name, var);
    }
}
