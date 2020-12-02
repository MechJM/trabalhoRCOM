TLDR: Criar duas LAN's virtuais, VLANY0 e VLANY1, associar o TUXY3 e o TUXY4 à VLANY0 e associar o TUXY2 à VLANY1.

Steps
1. Configure tuxy2 and register its IP and MAC addresses
2. Create vlany0 in the switch and add corresponding ports
3. Create vlany1 and add corresponding port
4. Start capture at tuxy3.eth0
5. In tuxy3, ping tuxy4 and then ping tuxy2
6. Stop capture and save log
7. Start new captures in tuxy3.eth0, tuxy4.eth0, and tuxy2.eth0
8. In tuxy3, do ping broadcast (ping –b 172.16.y0.255) for a few seconds
9. Observe results, stop captures and save logs
10. Repeat steps 7, 8 and 9, but now do ping broadcast in tuxy2 (ping –b 172.16.y1.255)

1. Configurar VLANY0:

Pré-Requisitos:

* configure terminal
* vlan y0
* end

Adicionar portas do Tux 3 e 4:
* configure terminal
* interface fastethernet 0/port_number
* switchport mode acess
* switchport acess vlan y0
* end

2. Quantos domínios de transmissão existem e o que se pode concluir dos registos?