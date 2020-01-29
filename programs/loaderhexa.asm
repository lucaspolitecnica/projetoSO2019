        @   /0000
        CN  0
LOADER  SC  GETBYTE             ; Carregar o byte superior do endereco inicial
        MM  ADDRU
        MM  CHECK               ; Inicializar checksum
        SC  GETBYTE             ; Carregar o byte inferior do endereco inicial
        MM  ADDRL
        +   CHECK               ; Atualizar checksum
        MM  CHECK
        SC  GETBYTE             ; Carregar tamanho em bytes
        MM  SIZE
        +   CHECK               ; Atualizar checksum
        MM  CHECK
BODY    SC  GETBYTE             ; Carregar proximo byte
        CN  2                   ; Salvar no endereco em ADDRU
        MM  ADDRU
        +   CHECK               ; Atualizar checksum
        MM  CHECK
        LD  ADDRL               ; Adicionar um ao byte inferior do endereco
        +   ONE
        MM  ADDRL
        JZ  OVERFL              ; Lidar com overflow do byte inferior do endereco
        JP  UPDSIZE
OVERFL  LD  ADDRU               ; Adicionar um ao byte superior do endereco
        +   ONE
        MM  ADDRU
UPDSIZE LD  SIZE                ; Subtrair um do tamanho do programa
        -   ONE
        MM  SIZE
        JZ  LDRUN               ; Parar de carregar o programa se SIZE = 0
        JP  BODY                ; Recomecar loop de carga do programa
LDRUN   SC  GETBYTE             ; Carregar o byte superior do endereco de execucao
        MM  ADDRU
        +   CHECK               ; Atualizar checksum
        MM  CHECK
        SC  GETBYTE             ; Carregar o byte inferior do endereco de execucao
        MM  ADDRL
        +   CHECK               ; Atualizar checksum
        MM  CHECK
        SC  GETBYTE             ; Conferir checksum
        +   CHECK
        JZ  RUN                 ; Comecar a execucao se checksum estiver correto
        JP  ERROR               ; Tratar erro de checksum
RUN     CN  2                   ; Pular para o comeco da execucao do programa
        JP  ADDRU
ERROR   LD  CH!                 ; Imprimir "!" para indicar erro
        IO  6
        CN  0                   ; Abortar execucao
ONE     K   1                   ; Constante um
CH!     K   "!"                 ; caracter exclamacao
ADDRU   K   0                   ; Byte superior de endereco
ADDRL   K   0                   ; Byte inferior de endereco
SIZE    K   0                   ; Tamanho do programa
CHECK   K   0                   ; Checksum
GETBYTE $   2                   ; Subrotina para pegar e converter byte hexa ASCII para binario
GETNIB1 IO  1                   ; Pegar primeiro nibble
        -   NH3A                ; Testar se entre 0-9 ou A-F
        JN  ASCIIN1             ; Pular se for entre 0-9
        -   NH7                 ; Compensar para entre A-F
ASCIIN1 +   NHA                 ; Corrigir para entre 0-9
        *   NH10                ; Deslocamento em 4 bits para a esquerda
        MM  UPNIB               ; Salvar nibble superior
GETNIB2 IO  1                   ; Pegar segundo nibble
        -   NH3A                ; Testar se entre 0-9 ou A-F
        JN  ASCIIN2             ; Pular se entre 0-9
        -   NH7                 ; Compensar para entre A-F
ASCIIN2 +   NHA                 ; Corrigir para entre 0-9
        +   UPNIB               ; Adicionar nibble superior, completando o byte a ser retornado
        CN  2                   ; Terminar subrotina
        JP  GETBYTE
NH7     K   /7                  ; Constante numerica /7
NHA     K   /A                  ; Constante numerica /A
NH10    K   /10                 ; Constante numerica /10
NH3A    K   /3A                 ; Constante numerica /3A
UPNIB   K   0                   ; Nibble superior
        #   LOADER
