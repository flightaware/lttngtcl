#undef TRACEPOINT_PROVIDER
#define TRACEPOINT_PROVIDER tcl

#undef TRACEPOINT_INCLUDE
#define TRACEPOINT_INCLUDE "tcl-tp.h"

#if !defined(_TCL_TP_H) || defined(TRACEPOINT_HEADER_MULTI_READ)
#define _TCL_TP_H

#include <lttng/tracepoint.h>

TRACEPOINT_EVENT(
    tcl,
    tcl_cmdtrace,
    TP_ARGS(
        int, level_arg,
        char*, cmd_arg
    ),
    TP_FIELDS(
        ctf_string(name_field, cmd_arg)
        ctf_integer(int, level_field, level_arg)
    )
)

TRACEPOINT_EVENT(
    tcl,
    tcl_linetrace,
    TP_ARGS(
        int, level_arg,
        int, line_arg,
        char*, file_arg
    ),
    TP_FIELDS(
        ctf_integer(int, level_field, level_arg)
        ctf_integer(int, line_field, line_arg)
        ctf_string(file_field, file_arg)
    )
)

TRACEPOINT_EVENT(
    tcl,
    tcl_infotrace,
    TP_ARGS(
        int, level_arg,
        int, line_arg,
        char*, file_arg,
	char*, cmd_arg,
	char*, type_arg,
	char*, proc_or_lambda_arg,
	char*, method_arg,
	char*, class_or_object_arg
    ),
    TP_FIELDS(
        ctf_integer(int, level_field, level_arg)
        ctf_integer(int, line_field, line_arg)
        ctf_string(file_field, file_arg)
        ctf_string(cmd_field, cmd_arg)
        ctf_string(type_field, type_arg)
        ctf_string(proc_or_lambda_field, proc_or_lambda_arg)
        ctf_string(method_field, method_arg)
        ctf_string(class_or_object_field, class_or_object_arg)
    )
)

#endif /* _TCL_TP_H */

#include <lttng/tracepoint-event.h>
