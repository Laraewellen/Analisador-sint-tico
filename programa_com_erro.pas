program erro_declare_variavel;
var
    x: integer;
    y: real;
    z: integer real;  { Erro aqui: tentativa de declarar duas variáveis de tipos diferentes na mesma linha }
begin
    x := 10;
    y := 5.5;
end.
