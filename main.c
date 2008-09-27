#include <telepathy-glib/run.h>
#include "manager.h"

#include <telepathy-glib/debug.h>

static TpBaseConnectionManager *
construct_cm (void)
{
    return (TpBaseConnectionManager *) g_object_new (PECAN_CM_TYPE, NULL);
}

int
main (int argc,
      char **argv)
{
    tp_debug_set_persistent (TRUE);

    return tp_run_connection_manager ("telepathy-msn-pecan", VERSION, construct_cm, argc, argv);
}
