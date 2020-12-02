TLDR: Criar duas LAN's virtuais, VLANY0 e VLANY1, associar o TUXY3 e o TUXY4 à VLANY0 e associar o TUXY2 à VLANY1.

1. Configurar VLANY0

Pré-Requisitos: Ligar TUX2, TUX3 e TUX4 ao switch

Para criar a LAN virtual executar os seguintes comandos no GTKTerm:
* configure terminal
* vlan y0
* end

Adicionar portas do Tux 3 e 4 fazendo:
* configure terminal
* interface fastethernet 0/port_number
* switchport mode acess
* switchport acess vlan y0
* end

2. Quantos domínios de transmissão existem e o que se pode concluir dos registos?
