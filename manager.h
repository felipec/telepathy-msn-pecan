#ifndef PECAN_CM_H
#define PECAN_CM_H

#include <glib-object.h>
#include <telepathy-glib/base-connection-manager.h>

G_BEGIN_DECLS

typedef struct PecanCm PecanCm;
typedef struct PecanCmClass PecanCmClass;

struct PecanCmClass
{
    TpBaseConnectionManagerClass parent_class;
};

struct PecanCm
{
    TpBaseConnectionManager parent;

    gpointer priv;
};

GType pecan_cm_get_type (void);

#define PECAN_CM_TYPE (pecan_cm_get_type ())
#define PECAN_CM(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), PECAN_CM_TYPE, PecanCm))

G_END_DECLS

#endif /* PECAN_CM_H */
