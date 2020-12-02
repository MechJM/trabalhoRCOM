TLDR: Ligar o TUX3 ao TUX4 utilizando um switch.

Pré-Requisitos: Ligar físicamente TUX3 e TUX4 ao switch.

Re-inicialização do subsistema de comunicação
/etc/init.d/networking restart

Configuração TUXY3

activar interface eth0
– root# ifconfig eth0 up

listar configurações actuais das interfaces de rede
– root# ifconfig

configurar eth0 com endereço 192.168.0.1 e máscara 16 bits
– root# ifconfig eth0 192.168.0.1/16

adicionar rota para subrede
– root# route add -net 192.168.1.0/24 gw 172.16.4.254

adicionar rota default
– root# route add default gw 192.168.1.1

listar rotas actuais
– root# route -n

echo 1 > /proc/sys/net/ipv4/ip_forward
echo 0 > /proc/sys/net/ipv4/icmp_echo_ignore_broadcasts