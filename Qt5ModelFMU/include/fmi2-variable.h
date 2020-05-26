#ifndef     FMI2_VARIABLE_H
#define     FMI2_VARIABLE_H

#include    <QString>

#include    <fmilib.h>

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
struct fmi2_variable_t
{
    fmi2_import_variable_t  *v;
    QString                 name;
    QString                 description;
    fmi2_base_type_enu_t    type;

    fmi2_variable_t()
        : v(nullptr)
        , name("")
        , description("")
        , type(fmi2_base_type_real)
    {

    }
};

#endif // FMI2_VARIABLE_H
