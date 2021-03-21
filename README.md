# Rain-Gauge-Esp8266-NTP-RTC
# Mjerač padalina Esp8266 Blynk NTP RTC

Using Croduino Nova2 ESP8266, Blynk, Arduino OTA, NTP servers, RTC internal clock and precipitation sensor, a precipitation meter was made. Since the rainfall measurement is reset every day at midnight, the microcontroller needs to know what time it is. Each time the microcontroller is started and connected to the Internet, it pulls the time from the NTP server and starts the internal RTC clock with the obtained time value. Because if the time was constantly withdrawn from the NTP server in case of being left without an Internet network, the rainfall value would not be reseted at midnight, so it only withdraws time from the NTP server once and later relies on the internal RTC.
![](master/Images/mjerac_kise3.jpeg)
![](
