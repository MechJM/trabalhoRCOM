# Experiência 2

## TLDR: Criar duas LAN's virtuais, VLANY0 e VLANY1, associar o TUXY3 e o TUXY4 à VLANY0 e associar o TUXY2 à VLANY1.

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