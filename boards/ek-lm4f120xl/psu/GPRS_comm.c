#include "GPRS_comm.h"
#include "inc/utils.h"

volatile bool GPRSModuleInitialisationState = false;

void checkModemData(void)
{

    int c=0;

}

void sendGPRSCommand(const char* command)
{
    UARTGPRSSendBlocking((const unsigned char*) command, strlen(command));
    SysCtlDelay(SysCtlClockGet() / (5 * 5));
}

void sendVoltageGPRS(void)
{
    char com_array[100];
    char str_payload[5];
    unsigned int command_size;
    
    voltage = GetVoltage();
    strcpy(com_array, http_get_prefix);
    strcat(com_array, http_url_payload);
    snprintf(str_payload, sizeof(str_payload), "%d", GetVoltage());
    strcat(com_array, str_payload);
    strcat(com_array, http_url_terminator);

    command_size = strlen(http_get_prefix) + strlen(http_url_payload) + strlen(http_url_terminator) + 5;
    UARTGPRSSendBlocking((const unsigned char*) com_array, command_size);
    SysCtlDelay(SysCtlClockGet() / (5 * 3));

}

void InitGPRSModule(void)
{    
    sendGPRSCommand(comm_check_at);
    sendGPRSCommand(comm_check_current_operator);
    sendGPRSCommand(comm_check_signal_quality);
    sendGPRSCommand(comm_enable_get_time);
    sendGPRSCommand(comm_get_time);
    sendGPRSCommand(comm_set_all_functionality);
    sendGPRSCommand(comm_check_sim_unlock_status);
    sendGPRSCommand(comm_configure_bearer_apn);
    sendGPRSCommand(comm_configure_bearer_mode);
    sendGPRSCommand(comm_init_HTTP);
    sendVoltageGPRS();
    sendGPRSCommand(comm_do_HTTP_GET);

}

