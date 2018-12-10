# IF767-ipmt

## Pré-Requisitos

- Sistema Operacional UNIX (testado em macOS Mojave e Ubuntu 16.04)
- `g++ 5.4.0` ou superior / `clang++ 6.0` ou superior
- `cmake 3.5` ou superior

## Compilação

Execute o seguinte comando no root do projeto:

```bash
cmake .
make
```

Após a execução do comando, o binário `pmt` deverá estar disponível no diretório `bin`.

## Execução

Para executar o programa estando no root do projeto, deve-se executar:

```bash
./bin/ipmt
```

Uma mensagem contendo os argumentos e o uso da ferramenta deve ser exibida.

Uma possível chamada a ferramenta é:

```bash
./bin/ipmt index shakespeare.txt
```

ou

```bash
./bin/ipmt search unworthily shakespeare.idx
```

### Argumentos Padrão

Quando não especificados, os seguintes argumentos possuem os seguintes valores:

- `--indexing-algorithm` = `difference-cover`
- `--encoding-algorithm` = `lz77`
