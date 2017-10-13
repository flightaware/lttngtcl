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

#ifdef HAVE_TCL_INFOFRAME
extern Tcl_Obj * Tcl_InfoFrame(Tcl_Interp *);
#endif

static void GetFrameInfoFromDict(
		Tcl_Interp *interp,
		Tcl_Obj *info,
		char **args,
		int *argsi)
{
	static Tcl_Obj *keys[10] = { NULL };
	Tcl_Obj **k = keys, *val;
	int i = 0;

	if (!*k) {
#define kini(s) keys[i] = Tcl_NewStringObj(s, strlen(s)); i++
		kini("cmd");    kini("type");   kini("proc");   kini("file");
		kini("method"); kini("class");  kini("lambda"); kini("object");
		kini("line");   kini("level");
#undef kini
	}
	for (i = 0; i < 6; i++) {
		Tcl_DictObjGet(interp, info, *k++, &val);
		args[i] = val ? Tcl_GetString(val) : NULL;
	}
	/* no "proc" -> use "lambda" */
	if (!args[2]) {
		Tcl_DictObjGet(interp, info, *k, &val);
		args[2] = val ? Tcl_GetString(val) : NULL;
	}
	k++;
	/* no "class" -> use "object" */
	if (!args[5]) {
		Tcl_DictObjGet(interp, info, *k, &val);
		args[5] = val ? Tcl_GetString(val) : NULL;
	}
	k++;
	for (i = 0; i < 2; i++) {
		Tcl_DictObjGet(interp, info, *k++, &val);
		if (val) {
			Tcl_GetIntFromObj(interp, val, &argsi[i]);
		} else {
			argsi[i] = 0;
		}
	}
}

/*
 * Callback for Tcl's cmdtrace.
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
	char short_cmd[64];
	strncpy(short_cmd, command, sizeof(short_cmd) - 2);
	short_cmd[sizeof(short_cmd) - 1] = 0;
	tracepoint(tcl, tcl_cmdtrace, level, short_cmd);

#ifdef HAVE_TCL_INFOFRAME
	Tcl_Obj *info = Tcl_InfoFrame(interp);
	if (info) {
		char no_string = 0;
		char *a[6]; int i[2];
		GetFrameInfoFromDict(interp, info, a, i);

		char *infoCmd = (a[0] ? a[0] : &no_string);
		char *infoType = (a[1] ? a[1] : &no_string);
		char *infoProcOrLambda = (a[2] ? a[2] : &no_string);
		char *infoFile = (a[3] ? a[3] : &no_string);
		char *infoMethod = (a[4] ? a[4] : &no_string);
		char *infoClassOrObject = (a[5] ? a[5] : &no_string);
		int infoLine = i[0];
		int infoLevel = i[1];

		tracepoint(tcl, tcl_linetrace, infoLevel, infoLine, infoFile);
		strncpy(short_cmd, infoCmd, sizeof(short_cmd) - 2);
		short_cmd[sizeof(short_cmd) - 1] = 0;
		tracepoint(tcl, tcl_infotrace, infoLevel, infoLine, infoFile,
				short_cmd, infoType, infoProcOrLambda, infoMethod, infoClassOrObject);

		Tcl_DecrRefCount(info);
	}
#endif
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
