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

The name of the LTTng provider is `tcl:tcl_cmdtrace`

Therefore, there is overhead on the TCL side once ::lttng::cmdtrace is invoked.  
The C trace callback will call the LTTng implemention.

https://www.tcl.tk/man/tcl8.6/TclLib/CrtTrace.htm

## Installing

Installs with a standard `autoconf && make install`

## H2 Hacking TclInfoFrame into the TCL C API

If we hack the TCL implementation to export `TclInfoFrame(Tcl_Interp *, CmdFramePtr *)` as `Tcl_InfoFrame(Tcl_Interp *)`, then we
can provide a better set of traces.

```
/* add to generic/tclCmdIL.c */
Tcl_Obj *
Tcl_InfoFrame(Tcl_Interp *interp)
{
    Interp *iPtr = (Interp *) interp;
    if (iPtr && iPtr->cmdFramePtr) {
	return TclInfoFrame(interp, iPtr->cmdFramePtr);
    }
    return NULL;
}

```

Configure and build TCL with this patch. Configure this extension with `CFLAGS=-DHAVE_TCL_INFOFRAME ./configure --with-tcl=<path to patched TCL>`.

This will implement two additional providers for the data from info.  A file line trace info and the contents of "info frame".
```
tcl:tcl_cmdtrace   TCL level and cmd passed to the Tcl_CreateTrace callback
tcl:tcl_linetrace  TCL level, file and line number if available
tcl:tcl_infotrace  TCL "info frame" information
```

## Using barectf

In platforms where LTTng does not work, you can generate traces with barectf.

https://github.com/efficios/barectf

The barectf implementation in TCL lttng uses the linux-fs demo platform implementation.  You will need to install barectf.
You will also need the source code from git to inline compile the provider.
The makefile named Makefile.barectf provides an outline of how to build using barectf.
Remember that the C code from barectf is portable. You can copy the generated source files from one platform to another and compile.  Just copy the git barectf files also.
