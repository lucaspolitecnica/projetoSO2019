        @   /0000
        CN  0
LOADER  IO  1                   ; Carregar o byte superior do endereco inicial
        MM  ADDRU
        MM  CHECK               ; Inicializar checksum
        IO  1                   ; Carregar o byte inferior do endereco inicial
        MM  ADDRL
        +   CHECK               ; Atualizar checksum
        MM  CHECK
        IO  1                   ; Carregar tamanho em bytes
        MM  SIZE
        +   CHECK               ; Atualizar checksum
        MM  CHECK
BODY    IO  1                   ; Carregar proximo byte
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
LDRUN   IO  1                   ; Carregar o byte superior do endereco de execucao
        MM  ADDRU
        +   CHECK               ; Atualizar checksum
        MM  CHECK
        IO  1                   ; Carregar o byte inferior do endereco de execucao
        MM  ADDRL
        +   CHECK               ; Atualizar checksum
        MM  CHECK
        IO  1                   ; Conferir checksum
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
        #   LOADER
