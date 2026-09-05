# Plašička kun
Firmware pro plašičku kun založený na STM32G031F4P6 se dvěma piezo měniči Kemo L010 v sérii. Střídájí se náhodně tóny v rozsahu frekvencí (30 – 40) kHz a náhodnou dobou trvání v rozmezí (150 – 900) ms, bez pauz mezi tóny. Posloupnost tónu je odvozená z UID čipu. LED dioda bliká s periodou 500 ms a slouží k indikaci běhu firmwaru.

## Nastaveni
- `FW/Core/App/Inc/cfg.hpp`: rozsah frekvencí a délka tónu
- `FW/PlasickaKun.ioc`: hodiny, piny a inicializace TIM1.
- PA8: `PWM_HIGH` / TIM1_CH1; PA7: `PWM_LOW` / TIM1_CH1N.
- TIM1: 64 MHz, prescaler 0, dead-time 3 us (DIV2, DTG 96).

`HBridgePwm` ovladá výstupy a frekvenci. `ChirpGenerator` vybíra tóny.
Milisekundový čítač aktualizuje `SysTick_Handler`.

## Build
Ve VS Code otevři `PlasickaKun.code-workspace` nebo přímo složku `FW`.
Konfigurace STM32Cube a debuggeru je v `FW/.vscode`.

```sh
cd FW
cmake --preset Debug
cmake --build --preset Debug
```

Výsledek: `FW/build/Debug/PlasickaKun.elf` nebo `FW/build/Release/PlasickaKun.elf`. Debugovaní ve VS Code použvívá konfiguraci STM32Cube a ST-Link.

Při regeneraci CubeMX ponechat volbu Keep User Code. Aplikační kód v `main.c` a obsluze přerušení patří do bloku USER CODE. `main.c` se kvůli C++ aplikaci překládá jako C++; ovladače ST zůstávájí v C.

# HW
 **Pozor nejedná se o ověřenou verzi**.
Schéma zapojení je ve složce `HW/Schematic.pdf`. S tím rozdílem, že můj prototyp je ze součástek co dům dal. Pro 3V3 napájení mám použité LDO, nepoužívám programovací konektor a místo tranzistorů BC817 v H-můstku používám BC639.
Jako výstupní piezo měniče jsou použité 2 kusy Kemo L010 v sérii. Napájecí napětí je 12 VDC.

# Krabička pro 3D tisk
https://www.printables.com/model/1833678-krabicka-pro-plasicku-kun
