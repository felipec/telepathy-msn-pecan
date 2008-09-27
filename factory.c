#include "factory.h"

#include <dbus/dbus-glib.h>

#include <telepathy-glib/base-connection.h>
#include <telepathy-glib/dbus.h>
#include <telepathy-glib/errors.h>
#include <telepathy-glib/interfaces.h>

#if 0
#include "chan.h"
#endif

static gpointer parent_class;

enum
{
    PROP_CONNECTION = 1,
};

struct PecanTpFactoryPrivate
{
    TpBaseConnection *conn;

    GHashTable *channels;
};

static void
instance_init (GTypeInstance *instance,
	       gpointer g_class)
{
    PecanTpFactory *self;

    self = PECAN_TP_FACTORY (instance);

    self->priv = G_TYPE_INSTANCE_GET_PRIVATE (self, PECAN_TP_FACTORY_TYPE, PecanTpFactoryPrivate);

    self->priv->channels = g_hash_table_new_full (g_direct_hash, g_direct_equal, NULL, g_object_unref);
}

static void
dispose (GObject *object)
{
    PecanTpFactory *self = PECAN_TP_FACTORY (object);

    tp_channel_factory_iface_close_all ((TpChannelFactoryIface *) object);
    g_assert (self->priv->channels == NULL);

    ((GObjectClass *) parent_class)->dispose (object);
}

static void
get_property (GObject *object,
	      guint property_id,
	      GValue *value,
	      GParamSpec *pspec)
{
    PecanTpFactory *self = PECAN_TP_FACTORY (object);

    switch (property_id)
    {
	case PROP_CONNECTION:
	    g_value_set_object (value, self->priv->conn);
	    break;
	default:
	    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    }
}

static void
set_property (GObject *object,
	      guint property_id,
	      const GValue *value,
	      GParamSpec *pspec)
{
    PecanTpFactory *self = PECAN_TP_FACTORY (object);

    switch (property_id)
    {
	case PROP_CONNECTION:
	    /* We don't ref the connection, because it owns a reference to the
	     * factory, and it guarantees that the factory's lifetime is
	     * less than its lifetime */
	    self->priv->conn = g_value_get_object (value);
	    break;
	default:
	    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    }
}

static void
close_all (TpChannelFactoryIface *iface)
{
    PecanTpFactory *self = PECAN_TP_FACTORY (iface);

    if (self->priv->channels != NULL)
    {
	GHashTable *tmp = self->priv->channels;

	self->priv->channels = NULL;
	g_hash_table_destroy (tmp);
    }
}

struct _ForeachData
{
    gpointer user_data;
    TpChannelFunc callback;
};

static void
foreach_helper (gpointer key,
		gpointer value,
		gpointer user_data)
{
    struct _ForeachData *data = user_data;
    TpChannelIface *chan = TP_CHANNEL_IFACE (value);

    data->callback (chan, data->user_data);
}

static void
foreach (TpChannelFactoryIface *iface,
	 TpChannelFunc callback,
	 gpointer user_data)
{
    PecanTpFactory *self = PECAN_TP_FACTORY (iface);
    struct _ForeachData data = { user_data, callback };

    g_hash_table_foreach (self->priv->channels, foreach_helper, &data);
}

#if 0
static void
channel_closed_cb (ExampleEchoChannel *chan, PecanTpFactory *self)
{
    TpHandle handle;

    if (self->priv->channels != NULL)
    {
	g_object_get (chan,
		      "handle", &handle,
		      NULL);

	g_hash_table_remove (self->priv->channels, GUINT_TO_POINTER (handle));
    }
}
#endif

#if 0
static ExampleEchoChannel *
new_channel (PecanTpFactory *self, TpHandle handle)
{
    ExampleEchoChannel *chan;
    gchar *object_path;

    object_path = g_strdup_printf ("%s/EchoChannel%u",
				   self->priv->conn->object_path, handle);

    chan = g_object_new (EXAMPLE_TYPE_ECHO_CHANNEL,
			 "connection", self->priv->conn,
			 "object-path", object_path,
			 "handle", handle,
			 NULL);

    g_free (object_path);

    g_signal_connect (chan, "closed", (GCallback) channel_closed_cb, self);

    g_hash_table_insert (self->priv->channels, GUINT_TO_POINTER (handle), chan);

    tp_channel_factory_iface_emit_new_channel (self, (TpChannelIface *) chan, NULL);

    return chan;
}
#endif

static TpChannelFactoryRequestStatus
request (TpChannelFactoryIface *iface,
	 const gchar *chan_type,
	 TpHandleType handle_type,
	 guint handle,
	 gpointer request_id,
	 TpChannelIface **ret,
	 GError **error)
{
    PecanTpFactory *self = PECAN_TP_FACTORY (iface);
    TpChannelFactoryRequestStatus status;
    TpHandleRepoIface *contact_repo = tp_base_connection_get_handles
	(self->priv->conn, TP_HANDLE_TYPE_CONTACT);

#if 0
    if (tp_strdiff (chan_type, TP_IFACE_CHANNEL_TYPE_TEXT))
	return TP_CHANNEL_FACTORY_REQUEST_STATUS_NOT_IMPLEMENTED;
#endif

    if (handle_type != TP_HANDLE_TYPE_CONTACT)
	return TP_CHANNEL_FACTORY_REQUEST_STATUS_NOT_IMPLEMENTED;

    if (!tp_handle_is_valid (contact_repo, handle, error))
	return TP_CHANNEL_FACTORY_REQUEST_STATUS_ERROR;

#if 0
    ExampleEchoChannel *chan;
    chan = g_hash_table_lookup (self->priv->channels, GUINT_TO_POINTER (handle));

    status = TP_CHANNEL_FACTORY_REQUEST_STATUS_EXISTING;
    if (chan == NULL)
    {
	status = TP_CHANNEL_FACTORY_REQUEST_STATUS_CREATED;
	chan = new_channel (self, handle);
    }

    g_assert (chan != NULL);
    *ret = TP_CHANNEL_IFACE (chan);
#endif

    return status;
}

static void
iface_init (gpointer iface,
	    gpointer data)
{
    TpChannelFactoryIfaceClass *iface_class = iface;

    iface_class->close_all = close_all;
    iface_class->foreach = foreach;
    iface_class->request = request;
}

static void
class_init (gpointer g_class,
	    gpointer class_data)
{
    GObjectClass *g_object_class;

    parent_class = g_type_class_peek_parent (g_class);

    g_object_class = (GObjectClass *) g_class;

    g_object_class->dispose = dispose;
    g_object_class->get_property = get_property;
    g_object_class->set_property = set_property;

    {
	GParamSpec *param_spec;
	param_spec = g_param_spec_object ("connection", "Connection object",
					  "The connection that owns this channel factory",
					  TP_TYPE_BASE_CONNECTION,
					  G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE | G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK | G_PARAM_STATIC_BLURB);
	g_object_class_install_property (g_object_class, PROP_CONNECTION, param_spec);
    }

    g_type_class_add_private (g_class, sizeof (PecanTpFactoryPrivate));
}

GType
pecan_tp_factory_get_type (void)
{
    static volatile gsize type_volatile = 0;

    if (g_once_init_enter (&type_volatile))
    {
	GType type;

	type = g_type_register_static_simple (G_TYPE_OBJECT,
					      g_intern_static_string ("PecanTpFactory"),
					      sizeof (PecanTpFactoryClass),
					      class_init,
					      sizeof (PecanTpFactory),
					      instance_init,
					      0);

	{
	    const GInterfaceInfo iface_info = { iface_init, NULL, NULL };
	    g_type_add_interface_static (type, TP_TYPE_CHANNEL_FACTORY_IFACE, &iface_info);
	}

	g_once_init_leave (&type_volatile, type);
    }

    return type_volatile;
}
