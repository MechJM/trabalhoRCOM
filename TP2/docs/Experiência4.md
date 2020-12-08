# Experiência 4

## TLDR: Configurar um router comercial e configurar NAT

Para configurar uma rede IP os seguintes comandos foram executados na bancada 4 + os da experiência 1, 2 e 3:
TUX42 - route del default gw 172.16.41.253
TUX42 - route add default gw 172.16.41.254
TUX44 - route add default gw 172.16.41.254

LIGAR ROUTER GE0/0 À PORTA Gi0/1
LIGAR ROUTER GE0/1 À PORTA 4.1

TUX44 - configure terminal
interface gigabitEthernet 0/1
switchport mode acess
switchport acess vlan 41
end