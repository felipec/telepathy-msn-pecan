#ifndef PECAN_TP_CONNECTION_H
#define PECAN_TP_CONNECTION_H

#include <glib-object.h>
#include <telepathy-glib/base-connection.h>

G_BEGIN_DECLS

typedef struct PecanTpConnection PecanTpConnection;
typedef struct PecanTpConnectionClass PecanTpConnectionClass;
typedef struct PecanTpConnectionPrivate PecanTpConnectionPrivate;

struct PecanTpConnectionClass
{
    TpBaseConnectionClass parent_class;
};

struct PecanTpConnection
{
    TpBaseConnection parent;

    PecanTpConnectionPrivate *priv;
};

GType pecan_tp_connection_get_type (void);

#define PECAN_TP_CONNECTION_TYPE (pecan_tp_connection_get_type ())
#define PECAN_TP_CONNECTION(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), PECAN_TP_CONNECTION_TYPE, PecanTpConnection))

G_END_DECLS

#endif /* PECAN_TP_CONNECTION_H */
