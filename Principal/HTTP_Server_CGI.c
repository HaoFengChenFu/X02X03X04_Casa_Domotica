/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network:Service
 * Copyright (c) 2004-2018 ARM Germany GmbH. All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    HTTP_Server_CGI.c
 * Purpose: HTTP Server CGI Module
 * Rev.:    V6.0.0
 *----------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include "cmsis_os2.h"                  // ::CMSIS:RTOS2
#include "rl_net.h"                     // Keil.MDK-Pro::Network:CORE
#include "stdlib.h"				// Para usar el atoi

#include "Principal.h"

char Time_Date[60], prueba[30];
uint8_t umbralTemp = 25;

extern Mensaje_Temp_Hum datos_SHT30;
extern Tiempo_Fecha datos_horarios;
extern Mensaje_Iluminacion datos_luz;

extern uint8_t vent_forzado, luz_forzada;



//#include "Board_LED.h"                  // ::Board Support:LED

#if      defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
#pragma  clang diagnostic push
#pragma  clang diagnostic ignored "-Wformat-nonliteral"
#endif

// http_server.c
extern uint16_t AD_in (uint32_t ch);
extern uint8_t  get_button (void);

extern bool LEDrun;
extern char lcd_text[2][20+1];
extern osThreadId_t TID_Display;

// Local variables.
static uint8_t P2;
static uint8_t ip_addr[NET_ADDR_IP6_LEN];
static char    ip_string[40];

// My structure of CGI status variable.
typedef struct {
  uint8_t idx;
  uint8_t unused[3];
} MY_BUF;
#define MYBUF(p)        ((MY_BUF *)p)

// Process query string received by GET request.
void netCGI_ProcessQuery (const char *qstr) {
  netIF_Option opt = netIF_OptionMAC_Address;
  int16_t      typ = 0;
  char var[40];

  do {
    // Loop through all the parameters
    qstr = netCGI_GetEnvVar (qstr, var, sizeof (var));
    // Check return string, 'qstr' now points to the next parameter

    switch (var[0]) {
      case 'i': // Local IP address
        if (var[1] == '4') { opt = netIF_OptionIP4_Address;       }
        else               { opt = netIF_OptionIP6_StaticAddress; }
        break;

      case 'm': // Local network mask
        if (var[1] == '4') { opt = netIF_OptionIP4_SubnetMask; }
        break;

      case 'g': // Default gateway IP address
        if (var[1] == '4') { opt = netIF_OptionIP6_DefaultGateway; }
        else               { opt = netIF_OptionIP6_DefaultGateway; }
        break;

      case 'p': // Primary DNS server IP address
        if (var[1] == '4') { opt = netIF_OptionIP4_PrimaryDNS; }
        else               { opt = netIF_OptionIP6_PrimaryDNS; }
        break;

      case 's': // Secondary DNS server IP address
        if (var[1] == '4') { opt = netIF_OptionIP4_SecondaryDNS; }
        else               { opt = netIF_OptionIP6_SecondaryDNS; }
        break;
      
      default: var[0] = '\0'; break;
    }

    switch (var[1]) {
      case '4': typ = NET_ADDR_IP4; break;
      case '6': typ = NET_ADDR_IP6; break;

      default: var[0] = '\0'; break;
    }

    if ((var[0] != '\0') && (var[2] == '=')) {
      netIP_aton (&var[3], typ, ip_addr);
      // Set required option
      netIF_SetOption (NET_IF_CLASS_ETH, opt, ip_addr, sizeof(ip_addr));
    }
  } while (qstr);
}

// Process data received by POST request.
// Type code: - 0 = www-url-encoded form data.
//            - 1 = filename for file upload (null-terminated string).
//            - 2 = file upload raw data.
//            - 3 = end of file upload (file close requested).
//            - 4 = any XML encoded POST data (single or last stream).
//            - 5 = the same as 4, but with more XML data to follow.
void netCGI_ProcessData (uint8_t code, const char *data, uint32_t len) {
  char var[40],passw[12];

  if (code != 0) {
    // Ignore all other codes
    return;
  }
	
  P2 = 0;
  if (len == 0) {
    return;
  }
  passw[0] = 1;
  do {
    // Parse all parameters
    data = netCGI_GetEnvVar (data, var, sizeof (var));
    if (var[0] != 0) {

      if (strcmp (var, "LumHab=1") == 0) {
        // Encender el ventilador
				printf("Encendido forzado de las luces\n");
				luz_forzada = 1;
      }
			else if (strcmp (var, "LumHab=0") == 0){
				// Apagar el ventilador
				printf("Apagado forzado de las luces\n");
				luz_forzada = 0;
			}
      else if (strcmp (var, "VentHab=1") == 0) {
        // Encender el ventilador
				printf("Encendido forzado del ventilador\n");
				vent_forzado = 1;
      }
			else if (strcmp (var, "VentHab=0") == 0){
				// Apagar el ventilador
				printf("Apagado forzado del ventilador\n");
				vent_forzado = 0;
			}
      else if (strncmp (var, "UmbralTemp=", strlen("UmbralTemp=")) == 0) {
				
				umbralTemp = atoi(&var[11]);
				printf("Valor del pulso: %d �C\n", umbralTemp);
				printf("Falta saber por donde se va a enviar\n");
      }
    }
  } while (data);

}

// Generate dynamic web data from a script line.
uint32_t netCGI_Script (const char *env, char *buf, uint32_t buflen, uint32_t *pcgi) {
  int32_t socket;
  netTCP_State state;
  NET_ADDR r_client;
  const char *lang;
  uint32_t len = 0U;
  uint8_t id;
  netIF_Option opt = netIF_OptionMAC_Address;
  int16_t      typ = 0;

  switch (env[0]) {
    // Analyze a 'c' script line starting position 2
    case 'a' :
      // Network parameters from 'network.cgi'
      switch (env[3]) {
        case '4': typ = NET_ADDR_IP4; break;
        case '6': typ = NET_ADDR_IP6; break;

        default: return (0);
      }
      
      switch (env[2]) {
        case 'l':
          // Link-local address
          if (env[3] == '4') { return (0);                             }
          else               { opt = netIF_OptionIP6_LinkLocalAddress; }
          break;

        case 'i':
          // Write local IP address (IPv4 or IPv6)
          if (env[3] == '4') { opt = netIF_OptionIP4_Address;       }
          else               { opt = netIF_OptionIP6_StaticAddress; }
          break;

        case 'm':
          // Write local network mask
          if (env[3] == '4') { opt = netIF_OptionIP4_SubnetMask; }
          else               { return (0);                       }
          break;

        case 'g':
          // Write default gateway IP address
          if (env[3] == '4') { opt = netIF_OptionIP4_DefaultGateway; }
          else               { opt = netIF_OptionIP6_DefaultGateway; }
          break;

        case 'p':
          // Write primary DNS server IP address
          if (env[3] == '4') { opt = netIF_OptionIP4_PrimaryDNS; }
          else               { opt = netIF_OptionIP6_PrimaryDNS; }
          break;

        case 's':
          // Write secondary DNS server IP address
          if (env[3] == '4') { opt = netIF_OptionIP4_SecondaryDNS; }
          else               { opt = netIF_OptionIP6_SecondaryDNS; }
          break;
      }

      netIF_GetOption (NET_IF_CLASS_ETH, opt, ip_addr, sizeof(ip_addr));
      netIP_ntoa (typ, ip_addr, ip_string, sizeof(ip_string));
      len = (uint32_t)sprintf (buf, &env[5], ip_string);
      break;

    case 'b':
      // LED control from 'led.cgi'
      if (env[2] == 'c') {
        // Select Control
        len = (uint32_t)sprintf (buf, &env[4], LEDrun ?     ""     : "selected",
                                               LEDrun ? "selected" :    ""     );
        break;
      }
      // LED CheckBoxes
      id = env[2] - '0';
      if (id > 7) {
        id = 0;
      }
      id = (uint8_t)(1U << id);
      len = (uint32_t)sprintf (buf, &env[4], (P2 & id) ? "checked" : "");
      break;

    case 'c':
      // TCP status from 'tcp.cgi'
      while ((uint32_t)(len + 150) < buflen) {
        socket = ++MYBUF(pcgi)->idx;
        state  = netTCP_GetState (socket);

        if (state == netTCP_StateINVALID) {
          /* Invalid socket, we are done */
          return ((uint32_t)len);
        }

        // 'sprintf' format string is defined here
        len += (uint32_t)sprintf (buf+len,   "<tr align=\"center\">");
        if (state <= netTCP_StateCLOSED) {
          len += (uint32_t)sprintf (buf+len, "<td>%d</td><td>%d</td><td>-</td><td>-</td>"
                                             "<td>-</td><td>-</td></tr>\r\n",
                                             socket,
                                             netTCP_StateCLOSED);
        }
        else if (state == netTCP_StateLISTEN) {
          len += (uint32_t)sprintf (buf+len, "<td>%d</td><td>%d</td><td>%d</td><td>-</td>"
                                             "<td>-</td><td>-</td></tr>\r\n",
                                             socket,
                                             netTCP_StateLISTEN,
                                             netTCP_GetLocalPort(socket));
        }
        else {
          netTCP_GetPeer (socket, &r_client, sizeof(r_client));

          netIP_ntoa (r_client.addr_type, r_client.addr, ip_string, sizeof (ip_string));
          
          len += (uint32_t)sprintf (buf+len, "<td>%d</td><td>%d</td><td>%d</td>"
                                             "<td>%d</td><td>%s</td><td>%d</td></tr>\r\n",
                                             socket, netTCP_StateLISTEN, netTCP_GetLocalPort(socket),
                                             netTCP_GetTimer(socket), ip_string, r_client.port);
        }
      }
      /* More sockets to go, set a repeat flag */
      len |= (1u << 31);
      break;
		
		case 'h':				// Refresco de la Humedad Relativa
					sprintf(prueba, "%.2f %%", datos_SHT30.humedad);
          len = (uint32_t)sprintf (buf, &env[4], prueba);
			break;
		
		case 'g':				// Refresco Humedad "Humedad.cgx"
					sprintf(prueba, "%.2f %%", datos_SHT30.humedad);
          len = (uint32_t)sprintf (buf, &env[1], prueba);
			break;

    case 'l':				// Luminosidad
					sprintf(prueba, "%d %%", datos_luz.porcentaje_pulso);
          len = (uint32_t)sprintf (buf, &env[4], prueba);
      break;
		
    case 'm':				// Refresco de la Luminosidad
					sprintf(prueba, "%d %%", datos_luz.porcentaje_pulso);
          len = (uint32_t)sprintf (buf, &env[1], prueba);
      break;
		
    case 'r':				//RTC
					sprintf(Time_Date, "Hora : %.2d:%.2d:%.2d | Fecha: %.2d-%.2d-%.4d ",
					datos_horarios.horas, datos_horarios.minutos, datos_horarios.segundos, datos_horarios.dia, datos_horarios.mes, datos_horarios.anno+2000);
		
          len = (uint32_t)sprintf (buf, &env[4], Time_Date);
      break;
		
    case 's':				// Refresco del RTC
					sprintf(Time_Date, "Hora : %.2d:%.2d:%.2d | Fecha: %.2d-%.2d-%.4d ", 
					datos_horarios.horas, datos_horarios.minutos, datos_horarios.segundos, datos_horarios.dia, datos_horarios.mes, datos_horarios.anno+2000);
		
          len = (uint32_t)sprintf (buf, &env[1], Time_Date);
      break;
		
    case 't':				// Temperatura
					sprintf(prueba, "%.2f �C", datos_SHT30.temperatura);
          len = (uint32_t)sprintf (buf, &env[4], prueba);
      break;

		case 'u':				// Refresco Temperatura "Temp.cgx"
					sprintf(prueba, "%.2f �C", datos_SHT30.temperatura);
          len = (uint32_t)sprintf (buf, &env[1], prueba);
			break;

  }
  return (len);
}

#if      defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
#pragma  clang diagnostic pop
#endif
