#include "manager.h"
#include "connection.h"

#include <dbus/dbus-glib.h>
#include <dbus/dbus-protocol.h>

#include <telepathy-glib/dbus.h>
#include <telepathy-glib/errors.h>

static gpointer parent_class;

static void
instance_init (GTypeInstance *instance,
	       gpointer g_class)
{
}

typedef struct {
    gchar *account;
    gchar *password;
} Params;

static const TpCMParamSpec params[] = {
    { "account", DBUS_TYPE_STRING_AS_STRING, G_TYPE_STRING,
	TP_CONN_MGR_PARAM_FLAG_REQUIRED, NULL,
	G_STRUCT_OFFSET (Params, account),
	tp_cm_param_filter_string_nonempty, NULL },
    { "password", DBUS_TYPE_STRING_AS_STRING, G_TYPE_STRING,
	TP_CONN_MGR_PARAM_FLAG_REQUIRED, NULL,
	G_STRUCT_OFFSET (Params, password),
	tp_cm_param_filter_string_nonempty, NULL },
    { NULL }
};

static gpointer
alloc_params (void)
{
    return g_slice_new0 (Params);
}

static void
free_params (gpointer p)
{
    Params *params = p;

    g_free (params->account);

    g_slice_free (Params, params);
}

static const TpCMProtocolSpec stub_protocols[] = {
    { "msn", params, alloc_params, free_params },
    { NULL, NULL }
};

static TpBaseConnection *
new_connection (TpBaseConnectionManager *self,
		const gchar *proto,
		TpIntSet *params_present,
		void *parsed_params,
		GError **error)
{
    Params *params;
    PecanTpConnection *conn;

    g_debug ("new connection");

    params = parsed_params;
    conn = PECAN_TP_CONNECTION (g_object_new (PECAN_TP_CONNECTION_TYPE,
					      "account", params->account,
					      "password", params->password,
					      "protocol", proto,
					      NULL));
    return (TpBaseConnection *) conn;
}

static void
class_init (gpointer g_class,
	    gpointer class_data)
{
    TpBaseConnectionManagerClass *base_class;

    base_class = (TpBaseConnectionManagerClass *) g_class;
    parent_class = g_type_class_peek_parent (g_class);

    base_class->new_connection = new_connection;
    base_class->cm_dbus_name = "msn_pecan";
    base_class->protocol_params = stub_protocols;
}

GType
pecan_cm_get_type (void)
{
    static volatile gsize type_volatile = 0;

    if (g_once_init_enter (&type_volatile))
    {
	GType type;

	type = g_type_register_static_simple (TP_TYPE_BASE_CONNECTION_MANAGER,
					      g_intern_static_string ("PecanCm"),
					      sizeof (PecanCmClass),
					      class_init,
					      sizeof (PecanCm),
					      instance_init,
					      0);

	g_once_init_leave (&type_volatile, type);
    }

    return type_volatile;
}
