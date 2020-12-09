# Experiência 3

## TLDR: Configurar um router em Linux

Para configurar uma rede IP os seguintes comandos foram executados na bancada 4 + os da experiência 1 e 2:
TUX44 - ifconfig eth1 172.16.41.253/24
TUX44 - echo 1 > /proc/sys/net/ipv4/ip_forward
TUX42 - route add default gw 172.16.41.253

TUX44 - configure terminal
interface fastEthernet 0/15
switchport mode acess
switchport acess vlan41
end

1. Que rotas há nos TUX? Qual o seu significado?

Nesta experiência foi configurado o TUXY4 como um router em linux, estabelecendo uma ligação entre as duas LAN's virtuais criadas na experiência 2.

De acordo com o diagrama existem as seguintes rotas:
* TUXY3 para VLANY0
* TUXY4 para VLANY0
* TUXY4 para VLANY1
* TUXY2 para VLANY1

2. Que informação é que uma tabela de forwarding contêm?

* Destination, o destino final da rota
* Gateway, o endereço ip do próximo ponto de passagem da rota
* Netmask, para dividir uma rede em sub-redes
* Flags, informações da rota
* Metric, custo da rota
* Ref, referências da rota
* Use, número de falhas da cache (-F) ou número de sucessos (-C)
* Interface, placa de rede responsável pela rota

3. Que mensagens ARP e endereços MAC associados são observados?

4. Que pacotes ICMP são observados?

São observados pacotes ICMP de pedido e respostas, de maneira a que os três TUX's saibam os endereços uns dos outros, depois das rotas serem adicionadas.

5. Quais são os endereços IP e MAC associados a um pacote ICMP?

Cada pacote ICMP têm o endereço IP e MAC dos TUX's de origem e destino.