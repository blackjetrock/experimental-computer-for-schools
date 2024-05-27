EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 2 2
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
Text Notes 875  1375 0    50   ~ 0
Power Budget\n\nLCD Module Logic:  80mA\nLD backlight        :   200mA\nOLED                   :\nPico                     :\nSD card               :\nTouch Keyboard  :\n
Text Notes 3225 1175 0    50   ~ 0
Desktop ESC has the same hardware layout as the small ESC,\nexcept there's no local keyboard. Instead there's a connection to \nthe AT42QT1418 touch PCB. The OLED display is driven, as is a \nlarger LCD for the TV replica. The keboard scanning GPIOs are used\nto drive the LCD and the touch keyboard interfaces.\n
$EndSCHEMATC
