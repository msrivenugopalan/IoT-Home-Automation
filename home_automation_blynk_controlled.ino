/*************************************************************
Title         :   Home automation using blynk
Description   :   To control light's brigntness with brightness,monitor temperature , monitor water level in the tank through blynk app
Pheripherals  :   Arduino UNO , Temperature system, LED, LDR module, Serial Tank, Blynk cloud, Blynk App.
 *************************************************************/

// Template ID, Device Name and Auth Token are provided by the Blynk.Cloud
// See the Device Info tab, or Template settings

#define BLYNK_TEMPLATE_ID "TMPL3xRSX1TwN"
#define BLYNK_TEMPLATE_NAME "Home Automation"
#define BLYNK_AUTH_TOKEN "rzLPb7OpjzWGw3nXoMCcIST8QxtawQ7C"

#include <SPI.h>
#include <Ethernet.h>
#include <BlynkSimpleEthernet.h>

#include "ldr.h"
#include "main.h"
#include "temperature_system.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,16,2);




#include "serial_tank.h"  


bool heater_sw,inlet_sw,outlet_sw,cooler_sw;
unsigned int tank_volume;

BlynkTimer timer;


// This function is called every time the Virtual Pin 0 state changes
/*To turn ON and OFF cooler based virtual PIN value*/

BLYNK_WRITE(COOLER_V_PIN)
{
   cooler_sw = param.asInt();
   cooler_control(cooler_sw);
   lcd.setCursor(8,0);
   if(cooler_sw)
   {
      lcd.print("CLR_ON ");
   }
   else
   {
      lcd.print("CLR_OFF");
   }
  
}

/*To turn ON and OFF heater based virtual PIN value*/

BLYNK_WRITE(HEATER_V_PIN )
{
    heater_sw = param.asInt();
    heater_control(heater_sw);
    lcd.setCursor(8,0);
    if(heater_sw)
    {
      lcd.print("HTR_ON ");
    }
    else
    {
      lcd.print("HTR_OFF");
    }

}

/*To turn ON and OFF inlet vale based virtual PIN value*/

BLYNK_WRITE(INLET_V_PIN)
{
   inlet_sw = param.asInt();
   lcd.setCursor(7, 1);
   if(inlet_sw)
   {
      enable_inlet();
      lcd.print("IN_FL_ON ");
   }
   else
   {
      disable_inlet();
      lcd.print("IN_FL_OFF");
   }
}

/*To turn ON and OFF outlet value based virtual switch value*/

BLYNK_WRITE(OUTLET_V_PIN)
{
  outlet_sw = param.asInt();
  lcd.setCursor(7, 1);
  if(outlet_sw)
  {
      enable_outlet();
      lcd.print("OT_FL_ON ");
  }
  else
  {
      disable_outlet();
      lcd.print("OT_FL_OFF");
  }
}

/* To display temperature and water volume as gauge on the Blynk App*/  

void update_temperature_reading()
{
  Blynk.virtualWrite(TEMPERATURE_GAUGE, read_temperature());
  Blynk.virtualWrite(WATER_VOL_GAUGE,volume());
  
}

/*To turn off the heater if the temperature raises above 35 deg C*/

void handle_temp(void)
{
   if(read_temperature() > (float) 35 && heater_sw)
   {
     heater_sw = 0;
     heater_control(0);
     Blynk.virtualWrite(HEATER_V_PIN, 0);

     lcd.setCursor(8,0);
     lcd.print("HTR_OFF");
   }
}

/*To control water volume above 2000ltrs*/

void handle_tank(void)
{
  if((volume() < 2000) && (inlet_sw == 0))
  {
     inlet_sw = 1;
     enable_inlet();
     Blynk.virtualWrite(INLET_V_PIN, 1);

     lcd.setCursor(7, 1);
     lcd.print("IN_FL_ON ");
  }
}



void setup(void)
{
    //initialise LDR
    init_ldr();
    //initialise temperature system
    init_temperature_system();
    //connect arduino to blynk cloud
    Blynk.begin(BLYNK_AUTH_TOKEN);

    //initialise CLCD
    lcd.init();
    lcd.backlight();
    lcd.home();
    
    lcd.setCursor(0,0);
    lcd.print("Home Automation");
    delay(1000);
    lcd.clear();
    
    init_serial_tank();

    timer.setInterval(1000, update_temperature_reading);

}
float temp;
unsigned int volume_water;
void loop(void) 
{
    //control the heater and cooler from the blynk app
    Blynk.run();  
    timer.run();
    brightness_control(); 

    temp = read_temperature();
    handle_temp();

    handle_tank();
    volume_water = volume();

    lcd.setCursor(0,0);
    lcd.print(temp, 2);     // print 2 decimal places
    lcd.print("C ");

    lcd.setCursor(0,1);
    lcd.print(volume_water);
    lcd.print("L "); 
}
