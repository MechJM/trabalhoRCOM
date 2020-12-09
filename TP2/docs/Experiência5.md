# Experiência 5

## TLDR: Configurar DNS nos TUXY2, TUXY3 e TUXY4

TUX43 - nano /etc/resolv.conf
search netlab.fe.up.pt
nameserver 172.16.1.2

1. Como configurar um serviço DNS num host?

Para configurar um serviço DNS é necessário indicar no ficheiro "/etc/resolv.conf" uma linha contendo o nome do servidor DNS e o endereço IP deste servidor que disponibiliza o serviço. Para esta experiência foi utilizado o servidor "netlab.fe.up.pt" com o respetivo endereço IP "172.16.1.2", visto que nos encontravamos na sala I320.

2. Que pacotes são trocados pelo DNS e que informações são transportadas?

Um pacote é enviado do Host para o Server com o Hostname indicado. O objetivo deste pacote é obter o endereço IP do Hostname. O servidor envia um pacote com este endereço IP do Hostname posteriormente.