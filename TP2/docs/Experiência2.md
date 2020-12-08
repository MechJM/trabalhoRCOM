# Experiência 2

## TLDR: Criar duas LAN's virtuais, VLANY0 e VLANY1, associar o TUXY3 e o TUXY4 à VLANY0 e associar o TUXY2 à VLANY1.
Para configurar uma rede IP os seguintes comandos foram executados na bancada 4 + os da experiência 1:
TUX42 - ifconfig eth0 172.16.41.1/24
TUX44 - ifconfig eth0 172.16.40.254/24

TUX43 - configure terminal
interface fastEthernet 0/14
switchport mode acess
switchport acess vlan 40
end

TUX44 - configure terminal
interface fastEthernet 0/15
switchport mode acess
switchport acess vlan 40
end

TUX42 - configure terminal
interface fastEthernet 0/16
switchport mode acess
switchport acess vlan 41
end

1. Como configurar VLANY0?

Executar os seguintes comandos no GTKTerm do TUX escolhido:
* configure terminal
* vlan y0
* end

Para adicionar a porta "N" executar à LAN virtual vlan y0 executar:
* configure terminal
* interface fastethernet 0/N
* switchport mode acess
* switchport acess vlan y0
* end

2. Quantos domínios de transmissão existem e o que se pode concluir dos registos?

Existem dois domínios de transmissão, um domínio que contêm o TUXY3 e TUXY4 e outro domínio que contém o TUXY2. Isto é justificado pelo facto de quando é executado um comando ping no TUXY3, o TUXY4 recebe respostas mas não do TUXY2. O TUXY2 não recebe respostas do TUXY3 nem do TUXY4.