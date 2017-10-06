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

#endif /* _TCL_TP_H */

#include <lttng/tracepoint-event.h>
