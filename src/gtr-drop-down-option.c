/*
 * Copyright (C) 2022  Daniel Garcia Moreno <danigm@gnome.org>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <gtr-drop-down-option.h>

enum {
  PROP_0,
  PROP_NAME,
  PROP_DESCRIPTION,
  N_PROPERTIES
};

static GParamSpec *option_properties[N_PROPERTIES] = { NULL, };

struct _GtrDropDownOption {
  GObject parent_instance;
  char *name;
  char *description;
};

G_DEFINE_TYPE (GtrDropDownOption, gtr_drop_down_option, G_TYPE_OBJECT);

static void
gtr_drop_down_option_init (GtrDropDownOption *option)
{
  option->name = NULL;
  option->description = NULL;
}

static void
gtr_drop_down_option_finalize (GObject *object)
{
  GtrDropDownOption *option = GTR_DROP_DOWN_OPTION (object);

  g_free (option->name);
  g_free (option->description);

  G_OBJECT_CLASS (gtr_drop_down_option_parent_class)->finalize (object);
}

static void
gtr_drop_down_set_property (GObject      *object,
                            guint         property_id,
                            const GValue *value,
                            GParamSpec   *pspec)
{
  GtrDropDownOption *self = GTR_DROP_DOWN_OPTION (object);

  switch (property_id)
    {
    case PROP_NAME:
      if (self->name)
        g_free (self->name);
      self->name = g_value_dup_string (value);
      break;
    case PROP_DESCRIPTION:
      if (self->description)
        g_free (self->description);
      self->description = g_value_dup_string (value);
      break;
    default:
      /* We don't have any other property... */
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
    }
}

static void
gtr_drop_down_get_property (GObject    *object,
                            guint       property_id,
                            GValue     *value,
                            GParamSpec *pspec)
{
  GtrDropDownOption *self = GTR_DROP_DOWN_OPTION (object);

  switch (property_id)
    {
    case PROP_NAME:
      g_value_set_string (value, self->name);
      break;
    case PROP_DESCRIPTION:
      g_value_set_string (value, self->description);
      break;
    default:
      /* We don't have any other property... */
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
    }
}

static void
gtr_drop_down_option_class_init (GtrDropDownOptionClass *class)
{
  GObjectClass *object_class = G_OBJECT_CLASS (class);

  object_class->finalize = gtr_drop_down_option_finalize;
  object_class->set_property = gtr_drop_down_set_property;
  object_class->get_property = gtr_drop_down_get_property;

  option_properties[PROP_NAME] =
    g_param_spec_string ("name", "Name", "Name",
                      NULL,
                      G_PARAM_CONSTRUCT | G_PARAM_READWRITE);

  option_properties[PROP_DESCRIPTION] =
    g_param_spec_string ("description", "Description", "Description",
                      NULL,
                      G_PARAM_CONSTRUCT | G_PARAM_READWRITE);

  g_object_class_install_properties (object_class,
                                     N_PROPERTIES,
                                     option_properties);
}

GtrDropDownOption *
gtr_drop_down_option_new (const char *name, const char *description)
{
  GtrDropDownOption *option = g_object_new (GTR_TYPE_DROP_DOWN_OPTION,
                                            "name", name,
                                            "description", description,
                                            NULL);
  return option;
}

gboolean
gtr_drop_down_option_equal (GtrDropDownOption *opt1, GtrDropDownOption *opt2)
{
    if (strcmp (opt1->name, opt2->name))
        return FALSE;
    return TRUE;
}

const char *
gtr_drop_down_option_get_name (GtrDropDownOption *opt)
{
  return opt->name;
}

const char *
gtr_drop_down_option_get_description (GtrDropDownOption *opt)
{
  return opt->description;
}
