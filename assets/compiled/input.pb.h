/* Automatically generated nanopb header */
/* Generated by nanopb-0.4.5 */

#ifndef PB_PB_INPUT_INPUT_PB_H_INCLUDED
#define PB_PB_INPUT_INPUT_PB_H_INCLUDED
#include <pb.h>

#if PB_PROTO_HEADER_VERSION != 40
#error Regenerate this file with the current version of nanopb generator.
#endif

/* Enum definitions */
typedef enum _PB_Input_Key { 
    PB_Input_Key_UP = 0, 
    PB_Input_Key_DOWN = 1, 
    PB_Input_Key_RIGHT = 2, 
    PB_Input_Key_LEFT = 3, 
    PB_Input_Key_OK = 4, 
    PB_Input_Key_BACK = 5 
} PB_Input_Key;

typedef enum _PB_Input_Type { 
    PB_Input_Type_PRESS = 0, /* *< Press event, emitted after debounce */
    PB_Input_Type_RELEASE = 1, /* *< Release event, emitted after debounce */
    PB_Input_Type_SHORT = 2, /* *< Short event, emitted after InputTypeRelease done withing INPUT_LONG_PRESS interval */
    PB_Input_Type_LONG = 3, /* *< Long event, emmited after INPUT_LONG_PRESS interval, asynchronouse to InputTypeRelease */
    PB_Input_Type_REPEAT = 4 /* *< Repeat event, emmited with INPUT_REPEATE_PRESS period after InputTypeLong event */
} PB_Input_Type;

/* Struct definitions */
typedef struct _PB_Input_SendEventRequest { 
    PB_Input_Key key; 
    PB_Input_Type type; 
} PB_Input_SendEventRequest;


/* Helper constants for enums */
#define _PB_Input_Key_MIN PB_Input_Key_UP
#define _PB_Input_Key_MAX PB_Input_Key_BACK
#define _PB_Input_Key_ARRAYSIZE ((PB_Input_Key)(PB_Input_Key_BACK+1))

#define _PB_Input_Type_MIN PB_Input_Type_PRESS
#define _PB_Input_Type_MAX PB_Input_Type_REPEAT
#define _PB_Input_Type_ARRAYSIZE ((PB_Input_Type)(PB_Input_Type_REPEAT+1))


#ifdef __cplusplus
extern "C" {
#endif

/* Initializer values for message structs */
#define PB_Input_SendEventRequest_init_default   {_PB_Input_Key_MIN, _PB_Input_Type_MIN}
#define PB_Input_SendEventRequest_init_zero      {_PB_Input_Key_MIN, _PB_Input_Type_MIN}

/* Field tags (for use in manual encoding/decoding) */
#define PB_Input_SendEventRequest_key_tag        1
#define PB_Input_SendEventRequest_type_tag       2

/* Struct field encoding specification for nanopb */
#define PB_Input_SendEventRequest_FIELDLIST(X, a) \
X(a, STATIC,   SINGULAR, UENUM,    key,               1) \
X(a, STATIC,   SINGULAR, UENUM,    type,              2)
#define PB_Input_SendEventRequest_CALLBACK NULL
#define PB_Input_SendEventRequest_DEFAULT NULL

extern const pb_msgdesc_t PB_Input_SendEventRequest_msg;

/* Defines for backwards compatibility with code written before nanopb-0.4.0 */
#define PB_Input_SendEventRequest_fields &PB_Input_SendEventRequest_msg

/* Maximum encoded size of messages (where known) */
#define PB_Input_SendEventRequest_size           4

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
