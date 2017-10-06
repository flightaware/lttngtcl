/*
 * Copyright (C) 2017 FlightAware
 *
 * Freely redistributable under the Berkeley copyright.  See license.terms
 * for details.
 */

#include <tcl.h>
#include <string.h>

#include "tcl-tp.h"

#undef TCL_STORAGE_CLASS
#define TCL_STORAGE_CLASS DLLEXPORT

/*
 * Callback for Tcl's cmdtrace.
 *
 */
void tcllttng_callback(ClientData clientData,
		Tcl_Interp *interp,     /* Current interpreter. */
		int level,              /* Current trace level. */
		char *command,          /* The command being traced (after
								 * substitutions). */
		Tcl_CmdProc *cmdProc,   /* Points to command's command procedure. */
		ClientData cmdClientData,  /* Client data associated with command procedure. */
		int argc,               /* Number of arguments. */
		const char *argv[])     /* Argument strings. */
{
	tracepoint(tcl, tcl_cmdtrace, level, command);
}

typedef struct tcllttng_objectClientData {
	int object_magic;
} tcllttng_objectClientData;

#define LTTNG_OBJECT_MAGIC 14302957

static void tcllttng_CmdDeleteProc(ClientData clientData)
{
	if (clientData != NULL) {
		ckfree(clientData);
	}
}

static int tcllttngObjCmd(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objvp[])
{
	Tcl_CreateTrace(interp, 50000, tcllttng_callback, NULL);

	return TCL_OK;
}


/*
 *----------------------------------------------------------------------
 *
 * Lttngtcl_Init --
 *
 *	Initialize the LTTng extension.  The string "lttngtcl"
 *      in the function name must match the PACKAGE declaration at the top of
 *	configure.in.
 *
 * Results:
 *	A standard Tcl result
 *
 * Side effects:
 *	One new command "::lttng::cmdtrace" is added to the Tcl interpreter.
 *
 *----------------------------------------------------------------------
 */

	EXTERN int
Lttngtcl_Init(Tcl_Interp *interp)
{
	Tcl_Namespace *namespace;
	/*
	 * This may work with 8.0, but we are using strictly stubs here,
	 * which requires 8.1.
	 */
	if (Tcl_InitStubs(interp, "8.1", 0) == NULL) {
		return TCL_ERROR;
	}

	if (Tcl_PkgRequire(interp, "Tcl", "8.1", 0) == NULL) {
		return TCL_ERROR;
	}

	if (Tcl_PkgProvide(interp, PACKAGE_NAME, PACKAGE_VERSION) != TCL_OK) {
		return TCL_ERROR;
	}

	namespace = Tcl_CreateNamespace (interp, "::lttng", NULL, NULL);
	tcllttng_objectClientData *data = (tcllttng_objectClientData *)ckalloc(sizeof(tcllttng_objectClientData));

	if (data == NULL) {
		return TCL_ERROR;
	}

	data->object_magic = LTTNG_OBJECT_MAGIC;

	/* Create the create command  */
	Tcl_CreateObjCommand(interp, "::lttng::cmdtrace", (Tcl_ObjCmdProc *) tcllttngObjCmd, 
			(ClientData)data, (Tcl_CmdDeleteProc *)tcllttng_CmdDeleteProc);

	Tcl_Export (interp, namespace, "*", 0);

	return TCL_OK;
}


/*
 *----------------------------------------------------------------------
 *
 * tcllttng_SafeInit --
 *
 *	Initialize tcllttngtcl in a safe interpreter.
 *
 * Results:
 *	A standard Tcl result
 *
 *----------------------------------------------------------------------
 */

	EXTERN int
Tcllttng_SafeInit(Tcl_Interp *interp)
{
	/*
	 * can this work safely?  I don't know...
	 */
	return TCL_ERROR;
}

/* vim: set ts=4 sw=4 sts=4 noet : */
