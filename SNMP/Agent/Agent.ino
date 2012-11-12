/**
 * @file Agent.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) Mikael Patel, 2012
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA  02111-1307  USA
 *
 * @section Description
 * Demonstration of an SNMP Agent and MIB for Arduino.
 * Requires Agentuino and Ethernet Library.
 * http://code.google.com/p/agentuino/
 * You will also need a MIB Browser. For instance; 
 * iReasoning MIB Browser. 
 * Please not that the MIB parsing is not complete and not
 * very efficient. This is a demo.
 */

#include <Ethernet.h>
#include <SPI.h>
#include <MemoryFree.h>
#include <Agentuino.h> 

static byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
static byte ip[] = { 192, 168, 0, 64 };

// RFC1213-MIB OIDs
// .iso (.1)
// .iso.org (.1.3)
// .iso.org.dod (.1.3.6)
// .iso.org.dod.internet (.1.3.6.1)
// .iso.org.dod.internet.mgmt (.1.3.6.1.2)
// .iso.org.dod.internet.mgmt.mib-2 (.1.3.6.1.2.1)
// .iso.org.dod.internet.mgmt.mib-2.system (.1.3.6.1.2.1.1)
// .iso.org.dod.internet.mgmt.mib-2.system.sysDescr (.1.3.6.1.2.1.1.1)
static char sysDescr[] PROGMEM      = "1.3.6.1.2.1.1.1.0";  // read-only  (DisplayString)

// .iso.org.dod.internet.mgmt.mib-2.system.sysObjectID (.1.3.6.1.2.1.1.2)
static char sysObjectID[] PROGMEM   = "1.3.6.1.2.1.1.2.0";  // read-only  (ObjectIdentifier)

// .iso.org.dod.internet.mgmt.mib-2.system.sysUpTime (.1.3.6.1.2.1.1.3)
static char sysUpTime[] PROGMEM     = "1.3.6.1.2.1.1.3.0";  // read-only  (TimeTicks)

// .iso.org.dod.internet.mgmt.mib-2.system.sysContact (.1.3.6.1.2.1.1.4)
static char sysContact[] PROGMEM    = "1.3.6.1.2.1.1.4.0";  // read-write (DisplayString)

// .iso.org.dod.internet.mgmt.mib-2.system.sysName (.1.3.6.1.2.1.1.5)
static char sysName[] PROGMEM       = "1.3.6.1.2.1.1.5.0";  // read-write (DisplayString)

// .iso.org.dod.internet.mgmt.mib-2.system.sysLocation (.1.3.6.1.2.1.1.6)
static char sysLocation[] PROGMEM   = "1.3.6.1.2.1.1.6.0";  // read-write (DisplayString)

// .iso.org.dod.internet.mgmt.mib-2.system.sysServices (.1.3.6.1.2.1.1.7)
static char sysServices[] PROGMEM   = "1.3.6.1.2.1.1.7.0";  // read-only  (Integer)

// Arduino defined OIDs
// .iso.org.dod.internet.private (.1.3.6.1.4)
// .iso.org.dod.internet.private.enterprises (.1.3.6.1.4.1)
// .iso.org.dod.internet.private.enterprises.arduino (.1.3.6.1.4.1.36582)
// .iso.org.dod.internet.private.enterprises.arduino.value.valA0-A5 (.1.3.6.1.4.1.36582.3.1-6)
static char valA0[] PROGMEM   = "1.3.6.1.4.1.36582.3.1.0";  // read-only  (Integer)
static char valA1[] PROGMEM   = "1.3.6.1.4.1.36582.3.2.0";  // read-only  (Integer)

// .iso.org.dod.internet.private.enterprises.arduino.value.valD0-D13 (.1.3.6.1.4.1.36582.3.7-20)
static char valD0[] PROGMEM   = "1.3.6.1.4.1.36582.3.7.0";  // read-only  (Integer)
static char valD1[] PROGMEM   = "1.3.6.1.4.1.36582.3.8.0";  // read-only  (Integer)

static char locDescr[] = "Agentuino, a light-weight SNMP Agent.";
static char locObjectID[] = "1.3.6.1.4.1.36582";
static uint32_t locUpTime = 0;
static char locContact[20] = "Eric Gionet";
static char locName[20] = "Agentuino";
static char locLocation[20] = "Nova Scotia, CA";
static int32_t locServices = 7;

uint32_t prevMillis = millis();
char oid[SNMP_MAX_OID_LEN];
SNMP_API_STAT_CODES api_status;
SNMP_ERR_CODES status;

void pduReceived()
{
  SNMP_PDU pdu;
  Serial.println("UDP Packet Received...");
  api_status = Agentuino.requestPdu(&pdu);
  if ( pdu.type == SNMP_PDU_GET || pdu.type == SNMP_PDU_GET_NEXT || pdu.type == SNMP_PDU_SET
    && pdu.error == SNMP_ERR_NO_ERROR && api_status == SNMP_API_STAT_SUCCESS ) {
    pdu.OID.toString(oid);
    Serial.print("OID = ");
    Serial.println(oid);
    if ( pdu.type == SNMP_PDU_SET ) {
      status = SNMP_ERR_READ_ONLY;
    } else if ( strcmp_P(oid, sysDescr ) == 0 ) {
      status = pdu.VALUE.encode(SNMP_SYNTAX_OCTETS, locDescr);
    } else if ( strcmp_P(oid, sysUpTime ) == 0 ) {
      status = pdu.VALUE.encode(SNMP_SYNTAX_TIME_TICKS, locUpTime);
    } else if ( strcmp_P(oid, sysName ) == 0 ) {
      status = pdu.VALUE.encode(SNMP_SYNTAX_OCTETS, locName);
    } else if ( strcmp_P(oid, sysContact ) == 0 ) {
      status = pdu.VALUE.encode(SNMP_SYNTAX_OCTETS, locContact);
    } else if ( strcmp_P(oid, sysLocation ) == 0 ) {
      status = pdu.VALUE.encode(SNMP_SYNTAX_OCTETS, locLocation);
    } else if ( strcmp_P(oid, sysServices) == 0 ) {
      status = pdu.VALUE.encode(SNMP_SYNTAX_INT, locServices);
     } else if ( strcmp_P(oid, sysObjectID) == 0 ) {
      status = pdu.VALUE.encode(SNMP_SYNTAX_OCTETS, locObjectID);
    } else if ( strcmp_P(oid, valA0) == 0 ) {
      status = pdu.VALUE.encode(SNMP_SYNTAX_INT, analogRead(0));
    } else if ( strcmp_P(oid, valA1) == 0 ) {
      status = pdu.VALUE.encode(SNMP_SYNTAX_INT, analogRead(1));
    } else if ( strcmp_P(oid, valD0) == 0 ) {
      status = pdu.VALUE.encode(SNMP_SYNTAX_INT, digitalRead(0));
    } else if ( strcmp_P(oid, valD1) == 0 ) {
      status = pdu.VALUE.encode(SNMP_SYNTAX_INT, digitalRead(1));
    } else {
      status = SNMP_ERR_NO_SUCH_NAME;
    }
    pdu.type = SNMP_PDU_RESPONSE;
    pdu.error = status;
    Agentuino.responsePdu(&pdu);
  }
  Agentuino.freePdu(&pdu);
}

void setup()
{
  Serial.begin(9600);
  while (!Serial);
  Ethernet.begin(mac, ip);
  api_status = Agentuino.begin();
  if ( api_status == SNMP_API_STAT_SUCCESS ) {
    Agentuino.onPduReceive(pduReceived);
    delay(10);
    Serial.println("SNMP Agent Initiated...");
    return;
  }
  delay(10);
  Serial.println("SNMP Agent failed!");
}

void loop()
{
  Agentuino.listen();
  if ( millis() - prevMillis > 1000 ) {
    prevMillis += 1000;
    locUpTime += 100;
  }
}

