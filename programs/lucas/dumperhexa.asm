        @   /0100
DUMPER  SC  GETARG              ; Despejar o byte superior do endereco inicial
        SC  PUTBYTE
        MM  ADDRU
        MM  CHECK               ; Inicializar checksum
        SC  GETARG              ; Despejar o byte inferior do endereco inicial
        SC  PUTBYTE
        MM  ADDRL
        +   CHECK               ; Atualizar checksum
        MM  CHECK
        IO  3                   ; Ignorar espaco
        SC  GETARG              ; Despejar tamanho
        SC  PUTBYTE
        MM  SIZE
        +   CHECK               ; Atualizar checksum
        MM  CHECK
BODY    CN  2                   ; Despejar proximo byte
        LD  ADDRU
        SC  PUTBYTE
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
        JZ  BODYEND             ; Parar de carregar o programa se SIZE = 0
        JP  BODY                ; Recomecar loop de carga do programa
BODYEND IO  3                   ; Ignorar espaco
        SC  GETARG              ; Despejar o byte superior do endereco de execucao
        SC  PUTBYTE
        +   CHECK               ; Atualizar checksum
        MM  CHECK
        SC  GETARG              ; Despejar o byte inferior do endereco de execucao
        SC  PUTBYTE
        +   CHECK               ; Atualizar checksum
        MM  CHECK
        LD  ZERO                ; Calcular checksum
        -   CHECK
        SC  PUTBYTE             ; Despejar checksum
        CN  0                   ; Fim do dumper
ZERO    K   0                   ; Constante zero
ONE     K   1                   ; Constante um
ADDRU   K   0                   ; Byte superior de endereco
ADDRL   K   0                   ; Byte inferior de endereco
SIZE    K   0                   ; Tamanho do programa
CHECK   K   0                   ; Checksum

GETARG  $   2                   ; Subrotina para pegar e converter byte hexa do argumento ASCII para binario
GETNIB1 IO  3                   ; Pegar primeiro nibble
        -   NH3A                ; Testar se entre 0-9 ou A-F
        JN  GAASC1              ; Pular se for entre 0-9
        -   NH7                 ; Compensar para entre A-F
GAASC1  +   NHA                 ; Corrigir para entre 0-9
        *   NH10                ; Deslocamento em 4 bits para a esquerda
        MM  UPNIB               ; Salvar nibble superior
GETNIB2 IO  3                   ; Pegar segundo nibble
        -   NH3A                ; Testar se entre 0-9 ou A-F
        JN  GAASC2              ; Pular se entre 0-9
        -   NH7                 ; Compensar para entre A-F
GAASC2  +   NHA                 ; Corrigir para entre 0-9
        +   UPNIB               ; Adicionar nibble superior, completando o byte a ser retornado
        CN  2                   ; Terminar subrotina
        JP  GETARG
PUTBYTE $   2                   ; Subrotina de dump de byte em hexadecimal ASCII
        MM  TMPBYTE             ; Guardar byte a ser dumpado
        /   NH10                ; Pegar nibble superior
        *   NH10
        MM  UPNIB               ; Armazenar nibble superior
        /   NH10
        -   NHA                 ; Testar se entre 0-9 ou A-F
        JN  ASCIIN1             ; Pular se for entre 0-9
        +   NH7                 ; Compensar para entre A-F
ASCIIN1 +   NH3A                ; Corrigir para entre 0-9
        IO  5                   ; Dump do nibble convertido
        LD  TMPBYTE             ; Carregar nibble inferior
        -   UPNIB
        -   NHA                 ; Testar se entre 0-9 ou A-F
        JN  ASCIIN2             ; Pular se for entre 0-9
        +   NH7                 ; Compensar para entre A-F
ASCIIN2 +   NH3A                ; Corrigir para entre 0-9
        IO  5                   ; Dump do nibble convertido
        LD  TMPBYTE             ; Carregar byte dumpado antes de retornar
        CN  2                   ; Terminar subrotina
        JP  PUTBYTE
NH7     K   /7                  ; Constante numerica /7
NHA     K   /A                  ; Constante numerica /A
NH10    K   /10                 ; Constante numerica /10
NH3A    K   /3A                 ; Constante numerica /3A
TMPBYTE K   0                   ; Byte a ser dumpado
UPNIB   K   0                   ; Nibble superior
        #   DUMPER
