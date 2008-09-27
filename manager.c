#include "manager.h"

#include <dbus/dbus-glib.h>

#include <telepathy-glib/dbus.h>
#include <telepathy-glib/errors.h>

static gpointer parent_class;

static void
instance_init (GTypeInstance *instance,
	       gpointer g_class)
{
}

static const TpCMProtocolSpec stub_protocols[] = {
    { NULL, NULL }
};

static TpBaseConnection *
new_connection (TpBaseConnectionManager *self,
		const gchar *proto,
		TpIntSet *params_present,
		void *parsed_params,
		GError **error)
{
    g_assert_not_reached ();

    return NULL;
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
    static volatile gsize type = 0;

    if (g_once_init_enter (&type))
    {
	GType g_define_type_id;

	g_define_type_id = g_type_register_static_simple ((tp_base_connection_manager_get_type ()),
							  g_intern_static_string ("PecanCm"),
							  sizeof (PecanCmClass),
							  class_init,
							  sizeof (PecanCm),
							  instance_init,
							  0);

	g_once_init_leave (&type, g_define_type_id);
    }

    return type;
}
