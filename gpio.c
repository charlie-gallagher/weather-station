// Printing and file handling
#include <stdio.h>
#include <string.h>
#include <stdlib.h>  // atoi
#include "gpio.h"

void gpio_set_debounce_time(gpio_pin* gpio,int time){gpio->debounce_time = time;}

int _gpio_get_filename(gpio_pin* gpio,char* fname,GPIO_PROPERTIES prop)
{
    /**
     * @brief Generates a filename based on gpio number and property.
     * 
     * This helper function does not check if the file actually exists.
     *
     * @param gpio gpio_pin struct pointer.
     * @param fname filename buffer to be written to.
     * @param prop GPIO property enum.
     * @return 1 on success, -1 if property is not recognized.
     */
    if(prop == GPIO_PROP_DIRECTION)
        sprintf(fname,  "/sys/class/gpio/gpio%d/%s",gpio->number,"direction");
    else if(prop == GPIO_PROP_EDGE)
        sprintf(fname,  "/sys/class/gpio/gpio%d/%s",gpio->number,"edge");
    else if(prop == GPIO_PROP_LABEL)
        sprintf(fname,  "/sys/class/gpio/gpio%d/%s",gpio->number,"label");
    else if(prop == GPIO_PROP_VALUE)
        sprintf(fname,  "/sys/class/gpio/gpio%d/%s",gpio->number,"value");
    else if(prop == GPIO_PROP_ACTIVE_LOW)
        sprintf(fname,  "/sys/class/gpio/gpio%d/%s",gpio->number,"active_low");
    else
        return -1;
    return 1;
}

int _gpio_write(gpio_pin* gpio,GPIO_PROPERTIES prop,char* value)
{
    /**
     * @brief Writes a value to a GPIO pin property file.
     *
     * Uses the given gpio data and the desired property `prop`, builds the
     * filename, and attempts to write `value` to that file.
     *
     * @param gpio gpio_pin struct pointer.
     * @param prop GPIO property enum.
     * @param value C string with value to be written to file
     * @return 1 on success, -1 on failure.
     */
    // Generate filename.
    char filename[FILENAME_MAX];
    int stat = _gpio_get_filename(gpio,filename,prop);
    if(stat != 1)
    {
        printf("Unrecognized GPIO_PROPERTIES value %d\n",prop);
        return -1;
    }

    // Open file.
    FILE* fp = fopen(filename,"w");
    if(!fp){
	    perror("File opening failed.");
        printf("Check that the path is correct and that you have privileges for writing to it. The GPIO pin might not exist.\n");
	    return -1;
    }

    // Write to file.
    int write_status = fputs(value,fp);
    if(write_status == EOF)
    {
        printf("File writing failed with fputs(%s,%s)\n",value,filename);
        printf("Check that you have privileges for writing.\n");
        return -1;
    }
    // Close file.
    fclose(fp);
    // Return success.
    return 1;
}

int _gpio_read(gpio_pin* gpio,GPIO_PROPERTIES prop,char* value)
{
    /**
     * @brief Reads the GPIO property into a buffer.
     *
     * Uses the given gpio data and the desired property `prop`, builds the
     * filename, and attempts to read the contents into `value`.
     *
     * @param gpio gpio_pin struct pointer.
     * @param prop GPIO property enum.
     * @return Return 1 on success, -1 on failure.
     */
    char filename[FILENAME_MAX];
    int stat = _gpio_get_filename(gpio,filename,prop);
    if(stat != 1)
    {
        printf("Unrecognized GPIO_PROPERTIES value %d\n",prop);
        return -1;
    }

    // Open file.
    FILE* fp = fopen(filename,"r");
    if(!fp){
	    perror("File opening failed, check that the path is correct and that you have privileges for reading it. The GPIO pin might not exist. Error");
	    return -1;
    }

    // Read from file.
    fgets(value,100,fp);
    value[strcspn(value, "\n") ] = '\0'; // Remove line ending

    // Close file.
    fclose(fp);
    // Return success.
    return 1;
}

int gpio_set_direction(gpio_pin* gpio,GPIO_DIRECTION dir)
{
    /**
     * @brief Set GPIO pin direction.
     *
     * @param gpio gpio_pin struct pointer.
     * @param dir GPIO_DIRECTION enum value.
     * @return 1 on success, -1 on failure.
     */
    int write_status = -1;
    if(dir == GPIO_DIR_INPUT)
    {
        write_status = _gpio_write(gpio,GPIO_PROP_DIRECTION,"in");
    }
    else if(dir == GPIO_DIR_OUTPUT)
    {
        write_status = _gpio_write(gpio,GPIO_PROP_DIRECTION,"out");
    }
    else write_status = -1;
    return write_status;
}

GPIO_DIRECTION gpio_get_direction(gpio_pin* gpio)
{
    /**
     * @brief Read GPIO pin direction and return it as GPIO_DIRECTION enum.
     *
     * @param gpio gpio_pin struct pointer.
     * @return GPIO_DIRECTION value.
     */
    char dir[10];
    _gpio_read(gpio,GPIO_PROP_DIRECTION,dir);
    if(strcmp(dir,"in") == 0)
    {
        return GPIO_DIR_INPUT;
    }
    else if(strcmp(dir,"out")==0)
    {
        return GPIO_DIR_OUTPUT;
    }
    return -1;
}

int gpio_set_value(gpio_pin* gpio,GPIO_VALUE val)
{
    /**
     * @brief Write GPIO pin value through sysfs.
     *
     * @param gpio gpio_pin struct pointer.
     * @return Return description.
     */
    int write_status = -1;
    
    char str[2];
	sprintf(str, "%d", val);

    write_status = _gpio_write(gpio,GPIO_PROP_VALUE,str);
    return write_status;
}

GPIO_VALUE gpio_get_value(gpio_pin* gpio)
{
    /**
     * @brief Read and return GPIO pin value.
     *
     * @param gpio gpio_pin struct pointer.
     * @return GPIO_VALUE value, or -1 on failure.
     */
    int read_status = -1;
    char val_str[2];
    read_status = _gpio_read(gpio,GPIO_PROP_VALUE,val_str);
    if(read_status == -1) return -1;
    GPIO_VALUE val_int = atoi(val_str);

    return val_int;
}

int gpio_toggle_output(gpio_pin* gpio)
{
    /**
     * @brief Toggle GPIO pin value.
     *
     * @param gpio gpio_pin struct pointer.
     * @return New value on success, -1 on failure.
     */
    GPIO_VALUE current_val = gpio_get_value(gpio);
    if(current_val == -1) return -1;
    GPIO_VALUE new_val = current_val ^ 1;  // XOR
    return new_val;
}

int gpio_set_active_low(gpio_pin* gpio)
{
    /**
     * @brief Summary.
     *
     * Long description.
     *
     * @param gpio gpio_pin struct pointer.
     * @return Return description.
     */
    int write_status = _gpio_write(gpio,GPIO_PROP_ACTIVE_LOW,"1");
    return write_status;
}

int gpio_set_active_high(gpio_pin* gpio)
{
    /**
     * @brief Summary.
     *
     * Long description.
     *
     * @param gpio gpio_pin struct pointer.
     * @return Return description.
     */
    int write_status = _gpio_write(gpio,GPIO_PROP_ACTIVE_LOW,"0");
    return write_status;
}







