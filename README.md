# PlasickaKun

Firmware pro STM32G031F4P6. Strida nahodne tony 30–40 kHz, kazdy po dobu
150–900 ms, bez pauz. LED meni stav kazdych 500 ms. Posloupnost tonu je
odvozena z UID cipu a po resetu se opakuje.

## Nastaveni

- `Core/App/Inc/cfg.hpp`: rozsah frekvenci a delka tonu.
- `PlasickaKun.ioc`: hodiny, piny a inicializace TIM1.
- PA8: `PWM_HIGH` / TIM1_CH1; PA7: `PWM_LOW` / TIM1_CH1N.
- TIM1: 64 MHz, prescaler 0, dead-time 3 us (DIV2, DTG 96).

`HBridgePwm` ovlada vystupy a frekvenci. `ChirpGenerator` vybira tony.
Milisekundovy citac aktualizuje `SysTick_Handler`.

## Build

S STM32Cube toolchainem a Ninja dostupnymi v PATH:

```sh
cmake --preset Debug
cmake --build --preset Debug
```

Vysledek: `build/Debug/PlasickaKun.elf`. Debugovani ve VS Code pouziva
konfiguraci STM32Cube a ST-Link.

Pri regeneraci CubeMX ponechat volbu Keep User Code. Aplikacni kod v
`main.c` a obsluze preruseni patri do bloku USER CODE. `main.c` se kvuli
C++ aplikaci preklada jako C++; ovladace ST zustavaji v C.
