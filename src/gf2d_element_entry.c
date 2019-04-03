#include "gf2d_element_entry.h"
#include "gf2d_element_label.h"
#include "gf2d_mouse.h"
#include "simple_logger.h"
#include "gf2d_input.h"

void gf2d_element_entry_draw(Element *element,Vector2D offset)
{
    EntryElement *entry;
    Vector2D position;
    if (!element)return;
    entry = (EntryElement*)element->data;
    if (!entry)return;
    vector2d_add(position,offset,element->bounds);
    gf2d_element_draw(entry->label,position);
    gf2d_rect_draw(
        gf2d_rect(
            element->bounds.x + position.x,
            element->bounds.y + position.y,
            element->bounds.w,
            element->bounds.h),
        gf2d_color8(200,200,200,255));
}

List *gf2d_element_entry_update(Element *element,Vector2D offset)
{
    Rect bounds;
    List *list;
    TextLine input = {0};
    char newLet = 0;
    SDL_Keymod mods;
    EntryElement *entry;
    if (!element)return NULL;
    entry = (EntryElement*)element->data;
    if (!entry)return NULL;
    bounds = gf2d_element_get_absolute_bounds(element,offset);
    if(gf2d_mouse_in_rect(bounds))
    {
        if (gf2d_mouse_button_pressed(0))
        {
            entry->has_focus = 1;
        }
    }
    else
    {
        if (gf2d_mouse_button_pressed(0))
        {
            entry->has_focus = 0;
        }
    }
    // check for keyboard input
    if (entry->text == NULL)
    {
        slog("no text buffer provided for entry");
        return NULL;
    }
    if (entry->has_focus)
    {
        mods = SDL_GetModState();
        if (entry->cursor_pos < (GF2DTEXTLEN -1))
        {
            for (input[0] = 'a';input[0] <= 'z';input[0]++)
            {
                if (gf2d_input_key_pressed(input))
                {
                    if (mods & KMOD_SHIFT)
                    {
                        newLet = input[0] - 'a' + 'A';
                    }
                    else
                    {
                        newLet = input[0];
                    }
                }
            }
            for (input[0] = '0';input[0] <= '9';input[0]++)
            {
                if (gf2d_input_key_pressed(input))
                {
                    if (mods & KMOD_SHIFT)
                    {
                        switch(input[0])
                        {
                            case '0':
                                newLet = ')';
                                break;
                            case '1':
                                newLet = '!';
                                break;
                            case '2':
                                newLet = '@';
                                break;
                            case '3':
                                newLet = '#';
                                break;
                            case '4':
                                newLet = '$';
                                break;
                            case '5':
                                newLet = '%';
                                break;
                            case '6':
                                newLet = '^';
                                break;
                            case '7':
                                newLet = '&';
                                break;
                            case '8':
                                newLet = '*';
                                break;
                            case '9':
                                newLet = '(';
                                break;
                        }
                    }
                    else
                    {
                        newLet = input[0];
                    }
                }
            }
            if (gf2d_input_key_pressed("`"))
            {
                if (mods & KMOD_SHIFT)
                {
                    newLet = '~';
                }
                else newLet = '`';
            }
            if (gf2d_input_key_pressed(";"))
            {
                if (mods & KMOD_SHIFT)
                {
                    newLet = ':';
                }
                else newLet = ';';
            }
            if (gf2d_input_key_pressed("="))
            {
                if (mods & KMOD_SHIFT)
                {
                    newLet = '+';
                }
                else newLet = '=';
            }
            if (gf2d_input_key_pressed("-"))
            {
                if (mods & KMOD_SHIFT)
                {
                    newLet = '_';
                }
                else newLet = '-';
            }
            if (gf2d_input_key_pressed("["))
            {
                if (mods & KMOD_SHIFT)
                {
                    newLet = '{';
                }
                else newLet = '[';
            }
            if (gf2d_input_key_pressed("]"))
            {
                if (mods & KMOD_SHIFT)
                {
                    newLet = '}';
                }
                else newLet = ']';
            }
            if (gf2d_input_key_pressed("."))
            {
                if (mods & KMOD_SHIFT)
                {
                    newLet = '>';
                }
                else newLet = '.';
            }
            if (gf2d_input_key_pressed(","))
            {
                if (mods & KMOD_SHIFT)
                {
                    newLet = '<';
                }
                else newLet = ',';
            }
            if (gf2d_input_key_pressed(" "))
            {
                newLet = ' ';
            }
            if (gf2d_input_key_pressed("\'"))
            {
                if (mods & KMOD_SHIFT)
                {
                    newLet = '"';
                }
                else newLet = '\'';
            }
            if (gf2d_input_key_pressed("\\"))
            {
                if (mods & KMOD_SHIFT)
                {
                    newLet = '|';
                }
                else newLet = '\\';
            }
            if (gf2d_input_key_pressed("/"))
            {
                if (mods & KMOD_SHIFT)
                {
                    newLet = '?';
                }
                else newLet = '/';
            }
            if (newLet != 0)
            {
                entry->text[entry->cursor_pos++] = newLet;
                entry->text[entry->cursor_pos] = '\0';
                gf2d_element_label_set_text(entry->label,entry->text);
                list = gf2d_list_new();
                gf2d_list_append(list,element);
                return list;
            }
        }
        if (entry->cursor_pos > 0)
        {
            if (gf2d_input_key_pressed("BACKSPACE"))
            {
                entry->text[--entry->cursor_pos] = '\0';
                gf2d_element_label_set_text(entry->label,entry->text);
                list = gf2d_list_new();
                gf2d_list_append(list,element);
                return list;
            }
        }
    }
    return NULL;
}

Element *entry_get_by_name(Element *e,char *name)
{
    EntryElement *entry;
    Element *r;
    if (!e)return NULL;
    entry = (EntryElement*)e->data;
    r = gf2d_get_element_by_name(entry->label,name);
    if (r)return r;
    return NULL;
}

void gf2d_element_entry_free(Element *element)
{
    EntryElement *entry;
    if (!element)return;
    entry = (EntryElement*)element->data;
    if (entry != NULL)
    {
        gf2d_element_free(entry->label);
        free(entry);
    }
}

void gf2d_element_make_entry(Element *e,EntryElement *entry)
{
    if (!e)return;
    e->data = entry;
    e->type = ET_Entry;
    e->draw = gf2d_element_entry_draw;
    e->update = gf2d_element_entry_update;
    e->free_data = gf2d_element_entry_free;
    e->get_by_name = entry_get_by_name;
}

EntryElement *gf2d_element_entry_new()
{
    EntryElement *entry;
    entry = (EntryElement *)malloc(sizeof(EntryElement));
    if (!entry)
    {
        slog("failed to allocate memory for entry");
        return NULL;
    }
    memset(entry,0,sizeof(EntryElement));
    return entry;
}

EntryElement *gf2d_element_entry_new_full(Element *label)
{
    EntryElement *entry;
    entry = gf2d_element_entry_new();
    if (!entry)return NULL;
    entry->label = label;
    entry->has_focus = 1;
    return entry;
}

void gf2d_element_entry_set_text_pointer(Element *e,char *text,size_t len)
{
    EntryElement *entry;
    if (!e)return;
    entry = (EntryElement*)e->data;
    if (entry == NULL)
    {
        return;
    }
    entry->text = text;
    entry->max_len = len;
    gf2d_element_label_set_text(entry->label,text);
}

void gf2d_element_load_entry_from_config(Element *e,SJson *json,Window *win)
{
    Element *label = NULL;
    SJson *value;
    
    if ((!e) || (!json))
    {
        slog("call missing parameters");
        return;
    }
    
    value = sj_object_get_value(json,"label");
    if (value)
    {
        label = gf2d_element_load_from_config(value,e,win);
    }
    gf2d_element_make_entry(e,gf2d_element_entry_new_full(label));
}

/*eol@eof*/
