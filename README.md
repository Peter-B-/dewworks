# Dew works - Dew point controlled ventilation

This is software for an Arduino-based ventilation controller. It monitors temperature and humidity, both inside and outside and switches a fan based on differences in dew point. 

The project is inspired by and based on an atricle in [the Make magazin](https://www.heise.de/select/make/2022/1/2135511212557842576) with some adaptions. I use a 2x16 LCD display plus a rotary encoder to implement a menu. That way all parameters can be adapted directly on the device without the need for a software update.

It will output a 25kHz PWM signal that can be used with many PC fans. Those fans have proven reliable, are comparetively quiet and only need a 12V power supply.

## BOM
- [Arduino Nano](https://www.az-delivery.de/en/products/nano-v3-mit-ch340-arduino-kompatibel)
- [1602 LCD with I²C interface](https://www.az-delivery.de/en/products/bundlelcd-schnittstelle?pr_prod_strat=copurchase&pr_rec_id=8051c6a26&pr_rec_pid=8363198985&pr_ref_pid=8326078985&pr_seq=uniform)
- [KY-040 Rotary encoder](https://www.az-delivery.de/en/products/drehimpulsgeber-modul)
- [3 way switch](https://www.reichelt.de/miniatur-kippschalter-ein-aus-ein-3-a-250-v-goobay-10022-p285989.html)

## Arduion Libraries
- EasyButton by Evert Arias
- Encoder by Paul Stoffregen
- DHT sensor library by Adafruit
- hd44780 by Bill Perry
