        @   /0105
DUMPER  CN  2                   ; Despejar o byte superior do endereco inicial
        LD  LDADDRU
        IO  5
        MM  ADDRU
        MM  CHECK               ; Inicializar checksum
        CN  2                   ; Despejar o byte inferior do endereco inicial
        LD  LDADDRL
        IO  5
        MM  ADDRL
        +   CHECK               ; Atualizar checksum
        MM  CHECK
        CN  2                   ; Despejar tamanho
        LD  LDSIZE
        IO  5
        MM  SIZE
        +   CHECK               ; Atualizar checksum
        MM  CHECK
BODY    CN  2                   ; Despejar proximo byte
        LD  ADDRU
        IO  5
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
BODYEND CN  2                   ; Despejar o byte superior do endereco de execucao
        LD  LDEXECU
        IO  5
        +   CHECK               ; Atualizar checksum
        MM  CHECK
        CN  2                   ; Despejar o byte inferior do endereco de execucao
        LD  LDEXECL
        IO  5
        +   CHECK               ; Atualizar checksum
        MM  CHECK
        LD  ZERO                ; Calcular checksum
        -   CHECK
        IO  5                   ; Despejar checksum
        CN  0                   ; Fim do dumper
ZERO    K   0                   ; Constante zero
ONE     K   1                   ; Constante um
LDADDRU K   /01                 ; Endereco inicial indireto - byte superior
EXT1    K   /00
LDADDRL K   /01                 ; Endereco inicial indireto - byte inferior
EXT2    K   /01
LDSIZE  K   /01                 ; Tamanho indireto
EXT3    K   /02
LDEXECU K   /01                 ; Endereco de execucao inidreto - byte superior
EXT4    K   /03
LDEXECL K   /01                 ; Endereco de execucao inidreto - byte inferior
EXT5    K   /04
ADDRU   K   0                   ; Byte superior de endereco
ADDRL   K   0                   ; Byte inferior de endereco
SIZE    K   0                   ; Tamanho do programa
CHECK   K   0                   ; Checksum
        #   DUMPER
