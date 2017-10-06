lttngtcl - a Tcl_CreateTrace API connector sending trace data to LTTng

This TCL extension provides a cmdtrace command which sends the trace data to LTTng.

```
package require lttngtcl

::lttng::cmdtrace
```

LTTng is an open source tracing framework for Linux.  http://lttng.org

TCL provides the C API hook for command tracing callbacks.  This extension sends the
trace data into a LTTng trace provider.

Use LTTng you can turn on and off trace tracing on the fly.  You can also send trace data
over the network.

Therefore, there is overhead on the TCL side once ::lttng::cmdtrace is invoked.  
The C trace callback will call the LTTng implemention.

https://www.tcl.tk/man/tcl8.6/TclLib/CrtTrace.htm
