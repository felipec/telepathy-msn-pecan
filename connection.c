#include "connection.h"
#include "factory.h"

#include <dbus/dbus-glib.h>

#include <telepathy-glib/dbus.h>
#include <telepathy-glib/errors.h>
#include <telepathy-glib/handle-repo-dynamic.h>

static gpointer parent_class;

enum
{
    PROP_ACCOUNT = 1,
    PROP_PASSWORD,
};

struct PecanTpConnectionPrivate
{
    gchar *account;
    gchar *password;
};

static void
instance_init (GTypeInstance *instance,
	       gpointer g_class)
{
    PecanTpConnection *self;
    self = PECAN_TP_CONNECTION (instance);
    self->priv = G_TYPE_INSTANCE_GET_PRIVATE (instance, PECAN_TP_CONNECTION_TYPE, PecanTpConnectionPrivate);
}

static void
get_property (GObject *object,
	      guint property_id,
	      GValue *value,
	      GParamSpec *spec)
{
    PecanTpConnection *self = PECAN_TP_CONNECTION (object);

    switch (property_id)
    {
	case PROP_ACCOUNT:
	    g_value_set_string (value, self->priv->account);
	    break;
	case PROP_PASSWORD:
	    g_value_set_string (value, self->priv->password);
	    break;
	default:
	    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, spec);
    }
}

static void
set_property (GObject *object,
	      guint property_id,
	      const GValue *value,
	      GParamSpec *spec)
{
    PecanTpConnection *self = PECAN_TP_CONNECTION (object);

    switch (property_id)
    {
	case PROP_ACCOUNT:
	    g_free (self->priv->account);
	    self->priv->account = g_utf8_strdown (g_value_get_string (value), -1);
	    break;
	case PROP_PASSWORD:
	    g_free (self->priv->password);
	    self->priv->password = g_strdup (g_value_get_string (value));
	    break;
	default:
	    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, spec);
    }
}

static void
finalize (GObject *object)
{
    PecanTpConnection *self = PECAN_TP_CONNECTION (object);

    g_free (self->priv->account);
    g_free (self->priv->password);

    G_OBJECT_CLASS (parent_class)->finalize (object);
}

static gchar *
get_unique_connection_name (TpBaseConnection *conn)
{
    PecanTpConnection *self = PECAN_TP_CONNECTION (conn);

    return g_strdup (self->priv->account);
}

static gchar *
normalize_contact (TpHandleRepoIface *repo,
		   const gchar *id,
		   gpointer context,
		   GError **error)
{
    if (id[0] == '\0')
    {
	g_set_error (error, TP_ERRORS, TP_ERROR_INVALID_ARGUMENT,
		     "ID must not be empty");
	return NULL;
    }

    return g_utf8_strdown (id, -1);
}

static void
create_handle_repos (TpBaseConnection *conn,
		     TpHandleRepoIface *repos[NUM_TP_HANDLE_TYPES])
{
    repos[TP_HANDLE_TYPE_CONTACT] = tp_dynamic_handle_repo_new (TP_HANDLE_TYPE_CONTACT, normalize_contact, NULL);
}

static GPtrArray *
create_channel_factories (TpBaseConnection *conn)
{
    GPtrArray *array;
   
    array = g_ptr_array_sized_new (1);

    g_ptr_array_add (array, g_object_new (PECAN_TP_FACTORY_TYPE, "connection", conn, NULL));

    return array;
}

static gboolean
start_connecting (TpBaseConnection *conn,
		  GError **error)
{
    PecanTpConnection *self = PECAN_TP_CONNECTION (conn);
    TpHandleRepoIface *contact_repo = tp_base_connection_get_handles (conn, TP_HANDLE_TYPE_CONTACT);

    conn->self_handle = tp_handle_ensure (contact_repo, self->priv->account, NULL, NULL);

    tp_base_connection_change_status (conn, TP_CONNECTION_STATUS_CONNECTED, TP_CONNECTION_STATUS_REASON_REQUESTED);

    return TRUE;
}

static void
shut_down (TpBaseConnection *conn)
{
    tp_base_connection_finish_shutdown (conn);
}

static void
class_init (gpointer g_class,
	    gpointer class_data)
{
    TpBaseConnectionClass *base_class;
    GObjectClass *gobject_class;

    base_class = (TpBaseConnectionClass *) g_class;
    gobject_class = (GObjectClass *) g_class;

    parent_class = g_type_class_peek_parent (g_class);
    g_type_class_add_private (g_class, sizeof (PecanTpConnectionPrivate));

    gobject_class->get_property = get_property;
    gobject_class->set_property = set_property;
    gobject_class->finalize = finalize;

    base_class->create_handle_repos = create_handle_repos;
    base_class->get_unique_connection_name = get_unique_connection_name;
    base_class->create_channel_factories = create_channel_factories;
    base_class->start_connecting = start_connecting;
    base_class->shut_down = shut_down;

    {
	GParamSpec *param_spec;

	param_spec = g_param_spec_string ("account", "Account",
					  "The username", NULL,
					  G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE | G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK | G_PARAM_STATIC_BLURB);
	g_object_class_install_property (gobject_class, PROP_ACCOUNT, param_spec);

	param_spec = g_param_spec_string ("password", "Password",
					  "The password", NULL,
					  G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE | G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK | G_PARAM_STATIC_BLURB);
	g_object_class_install_property (gobject_class, PROP_PASSWORD, param_spec);
    }
}

GType
pecan_tp_connection_get_type (void)
{
    static volatile gsize type = 0;

    if (g_once_init_enter (&type))
    {
	GType g_define_type_id;

	g_define_type_id = g_type_register_static_simple (TP_TYPE_BASE_CONNECTION,
							  g_intern_static_string ("PecanTpConnection"),
							  sizeof (PecanTpConnectionClass),
							  class_init,
							  sizeof (PecanTpConnection),
							  instance_init,
							  0);

	g_once_init_leave (&type, g_define_type_id);
    }

    return type;
}
