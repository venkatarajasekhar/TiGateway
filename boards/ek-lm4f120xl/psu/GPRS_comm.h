#ifndef __GPRS_COMM_H__
#define __GPRS_COMM_H__

#include "inc/uart_comm.h"
#include "stdbool.h"
#include "string.h"

#define COMMAND_SIZE(x) ( sizeof(x) / sizeof((x)[0]))
    

static const char* comm_check_at = "AT\r\n";
static const char* comm_check_current_operator = "AT+COPS?\r\n";
static const char* comm_check_signal_quality = "AT+CSQ\r\n";
static const char* comm_set_all_functionality = "AT+CFUN=1\r\n";
static const char* comm_check_sim_unlock_status = "AT+CPIN?\r\n";
static const char* comm_configure_bearer_apn = "AT+SAPBR=3,1,\"APN\",\"net\"\r\n";
static const char* comm_configure_bearer_mode = "AT+SAPBR=1,1\r\n";
static const char* comm_init_HTTP = "AT+HTTPINIT\r\n";
static const char* comm_set_HTTP_params = "AT+HTTPPARA=\"URL\",\"http://omv.tremend.ro//scrie.php?id=10&data=testare_multipla_keil\"\r\n";
static const char* comm_do_HTTP_GET = "AT+HTTPACTION=0\r\n";
static const char* comm_enable_get_time = "AT+CLTS=1\r\n"; 
static const char* comm_get_time = "AT+CCLK?\r\n";

static const char * http_get_prefix = "AT+HTTPPARA=\"URL\",\"http://omv.tremend.ro//scrie.php?id=PowerTrainVoltage";
static const char * http_url_payload = "&data=";
static const char * http_url_terminator = " miliVolts\"\r\n";

void checkModemData(void);
void InitGPRSModule(void);
void sendGPRSCommand(const char*);


extern volatile bool GPRSModuleInitialisation;

#endif
