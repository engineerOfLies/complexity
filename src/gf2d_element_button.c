#include "gf2d_element_button.h"
#include "gf2d_element_actor.h"
#include "gf2d_mouse.h"
#include "simple_logger.h"
#include "gf2d_input.h"

void gf2d_element_button_draw(Element *element,Vector2D offset)
{
    ButtonElement *button;
    Vector2D position;
    Actor *actor;
    if (!element)return;
    button = (ButtonElement*)element->data;
    if (!button)return;
    vector2d_add(position,offset,element->bounds);
    actor = gf2d_element_actor_get_actor(button->actor);
    switch(element->state)
    {
        case ES_hidden:
        case ES_disable:
            return;
        case ES_idle:
            gf2d_actor_set_action(actor,"idle");
            break;
        case ES_highlight:
            gf2d_actor_set_action(actor,"high");
            break;
        case ES_active:
            gf2d_actor_set_action(actor,"press");
            break;
    }
    gf2d_element_draw(button->actor,position);
    gf2d_element_draw(button->label,position);
}

List *gf2d_element_button_update(Element *element,Vector2D offset)
{
    Actor *actor;
    Rect bounds;
    List *list;
    ButtonElement *button;
    if (!element)return NULL;
    button = (ButtonElement*)element->data;
    if (!button)return NULL;
    actor = gf2d_element_actor_get_actor(button->actor);
    gf2d_actor_next_frame(actor);
    bounds = gf2d_element_get_absolute_bounds(element,offset);
    if(gf2d_mouse_in_rect(bounds))
    {
        element->state = ES_highlight;
        if (gf2d_mouse_button_state(0))
        {
            element->state = ES_active;
        }
        else if (gf2d_mouse_button_released(0))
        {
            list = gf2d_list_new();
            list = gf2d_list_append(list,element);
            return list;
        }
    }
    else
    {
        element->state = ES_idle;
    }
    if (gf2d_input_command_pressed(button->hotkey))
    {
        element->state = ES_active;
        list = gf2d_list_new();
        gf2d_list_append(list,element);
        return list;
    }

    return NULL;
}

const char *gf2d_element_button_get_input(Element *e)
{
    ButtonElement *button;
    if (!e)return NULL;
    button = (ButtonElement*)e->data;
    if (!button)return NULL;
    return button->hotkey;
}

Element *button_get_by_name(Element *e,char *name)
{
    ButtonElement *button;
    Element *r;
    if (!e)return NULL;
    button = (ButtonElement*)e->data;
    r = gf2d_get_element_by_name(button->label,name);
    if (r)return r;
    return gf2d_get_element_by_name(button->actor,name);
}

void gf2d_element_button_free(Element *element)
{
    ButtonElement *button;
    if (!element)return;
    button = (ButtonElement*)element->data;
    if (button != NULL)
    {
        gf2d_element_free(button->label);
        gf2d_element_free(button->actor);
        free(button);
    }
}

ButtonElement *gf2d_element_button_new()
{
    ButtonElement *button;
    button = (ButtonElement *)malloc(sizeof(ButtonElement));
    if (!button)
    {
        slog("failed to allocate memory for button");
        return NULL;
    }
    memset(button,0,sizeof(ButtonElement));
    return button;
}


void gf2d_element_make_button(Element *e,ButtonElement *button)
{
    if (!e)return;
    e->data = button;
    e->type = ET_Button;
    e->draw = gf2d_element_button_draw;
    e->update = gf2d_element_button_update;
    e->free_data = gf2d_element_button_free;
    e->get_by_name = button_get_by_name;
}

ButtonElement *gf2d_element_button_new_full(Element *label,Element *actor,Color highColor,Color pressColor)
{
    ButtonElement *button;
    button = gf2d_element_button_new();
    if (!button)return NULL;
    button->label = label;
    button->actor = actor;
    button->highColor = highColor;
    button->pressColor = pressColor;
    return button;
}

void gf2d_element_load_button_from_config(Element *e,SJson *json,Window *win)
{
    Vector4D highColor = {255,255,255,255},pressColor = {255,255,255,255};
    Element *label = NULL;
    Element *actor = NULL;
    SJson *value;
    ButtonElement *button;
    
    if ((!e) || (!json))
    {
        slog("call missing parameters");
        return;
    }
    
    value = sj_object_get_value(json,"highColor");
    if (!sj_value_as_vector4d(value,&highColor))
    {
        slog("highColor not provided");
    }

    value = sj_object_get_value(json,"pressColor");
    if (!sj_value_as_vector4d(value,&pressColor))
    {
        slog("pressColor not provided");
    }


    value = sj_object_get_value(json,"label");
    if (value)
    {
        label = gf2d_element_load_from_config(value,e,win);
    }
    value = sj_object_get_value(json,"actor");
    if (value)
    {
        actor = gf2d_element_load_from_config(value,e,win);
    }
    gf2d_element_make_button(e,gf2d_element_button_new_full(label,actor,gf2d_color_from_vector4(highColor),gf2d_color_from_vector4(pressColor)));
 
    button = (ButtonElement*)e->data;

    value = sj_object_get_value(json,"hotkey");
    if (value)
    {
        gf2d_line_cpy(button->hotkey,sj_get_string_value(value));
    }
}


/*eol@eof*/
