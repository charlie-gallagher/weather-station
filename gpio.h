typedef enum{
    GPIO_DIR_INPUT=0,
    GPIO_DIR_OUTPUT
} GPIO_DIRECTION;

typedef enum{
    GPIO_LOW=0,
    GPIO_HIGH
} GPIO_VALUE;

typedef enum{
    GPIO_EDGE_NONE,GPIO_EDGE_RISING,GPIO_EDGE_FALLING,GPIO_EDGE_BOTH
} GPIO_EDGE;

typedef enum{
    GPIO_PROP_DIRECTION,GPIO_PROP_EDGE,GPIO_PROP_LABEL,GPIO_PROP_VALUE,GPIO_PROP_ACTIVE_LOW
} GPIO_PROPERTIES;

typedef struct{
    int number;
    GPIO_DIRECTION dir;
    int debounce_time;
} gpio_pin;

// Private functions
int _gpio_get_filename(gpio_pin* gpio,char* fname,GPIO_PROPERTIES prop);
int _gpio_write(gpio_pin* gpio,GPIO_PROPERTIES prop,char* value);
int _gpio_read(gpio_pin* gpio,GPIO_PROPERTIES prop,char* value);

// Public functions
int gpio_set_direction(gpio_pin* gpio,GPIO_DIRECTION dir);
GPIO_DIRECTION gpio_get_direction(gpio_pin* gpio);
int gpio_set_value(gpio_pin* gpio,GPIO_VALUE val);
int gpio_toggle_output(gpio_pin* gpio);
GPIO_VALUE gpio_get_value(gpio_pin* gpio);
int gpio_set_active_low(gpio_pin* gpio);
int gpio_set_active_high(gpio_pin* gpio);
// Software debounce only
void gpio_set_debounce_time(gpio_pin* gpio,int time);
