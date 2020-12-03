# Experiência 1

## TLDR: Ligar o TUXY3 ao TUXY4 utilizando um switch.

1. O que são pacotes ARP e para que são usados?

O ART é a abreviatura de Address Resolution Protocol e trata-se de um protocolo de comunicação que serve para mapeiar um endereço de rede, como por exemplo um endereço IPv4, num endereço físico, como por exemplo um endereço Ethernet ou MAC.

2. Quais são os endereços MAC e IP dos pacotes ARB?

3. Quais os pacotes gerados pelo comando ping?

Os pacotes gerados pelo comando ping são em primeira instância pacotes ARP, de maneira a obter os endereços MAC e de seguida pacotes ICMP, Internet Control Message Protocol.

4. Quais os endereços MAC e IP dos pacotes ping?

Pela análise dos logs do wireshark desta experiência podemos escrever os endereços MAC e IP dos pacotes ping entre o TUX3 e TUX4. Estes logs foram registados na bancada 4.

Pacotes de pedido (request):
Endereço MAC da origem do pacote (TUX43): 00:21:5a:61:2f:d4
Endereço Mac do destino do pacote (TUX44): 00:21:5a:5a:7b:ea
Endereço IP da origem do pacote (TUX43): :
172.16.40.1
Endereço IP do destino do pacote (TUX44): :
172.16.40.254

![Exp1_1](https://raw.githubusercontent.com/MechJM/trabalhoRCOM/master/TP2/docs/logs/exp1_1.png?token=AIVHWXE5XUXUQ4HR3YCDNDK72JCFG)

Pacotes de resposta (reply):
Endereço MAC da origem do pacote (TUX44): 00:21:5a:5a:7b:ea
Endereço MAC do destino do pacote (TUX43):
00:21:5a:61:2f:d4
Endereço IP da origem do pacote (TUX44):
172.16.40.254
Endereço IP do destino do pacote (TUX43):
172.16.40.1

![Exp1_2](https://github.com/MechJM/trabalhoRCOM/tree/master/TP2/docs/logs/exp1_2.png)

5. Como saber se a trama recetor Ethernet é ARP, IP ou ICMP?

Se o header da trama for 0x0800, significa que é uma trama IP