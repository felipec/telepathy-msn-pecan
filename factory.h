#ifndef PECAN_TP_FACTORY_H
#define PECAN_TP_FACTORY_H

#include <glib-object.h>
#include <telepathy-glib/channel-factory-iface.h>

G_BEGIN_DECLS

typedef struct PecanTpFactory PecanTpFactory;
typedef struct PecanTpFactoryClass PecanTpFactoryClass;
typedef struct PecanTpFactoryPrivate PecanTpFactoryPrivate;

struct PecanTpFactoryClass
{
    GObjectClass parent_class;
};

struct PecanTpFactory
{
    GObject parent;

    PecanTpFactoryPrivate *priv;
};

GType pecan_tp_factory_get_type (void);

#define PECAN_TP_FACTORY_TYPE (pecan_tp_factory_get_type ())
#define PECAN_TP_FACTORY(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), PECAN_TP_FACTORY_TYPE, PecanTpFactory))

G_END_DECLS

#endif /* PECAN_TP_FACTORY_H */
